#include "robomongo/gui/widgets/explorer/ExplorerWidget.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QMovie>
#include <QKeyEvent>

#include "robomongo/core/AppRegistry.h"
#include "robomongo/core/domain/App.h"
#include "robomongo/core/utils/QtUtils.h"

#include "robomongo/gui/MainWindow.h"
#include "robomongo/gui/widgets/explorer/ExplorerTreeWidget.h"
#include "robomongo/gui/widgets/explorer/ExplorerServerTreeItem.h"
#include "robomongo/gui/widgets/explorer/ExplorerCollectionTreeItem.h"
#include "robomongo/gui/widgets/explorer/ExplorerDatabaseCategoryTreeItem.h"
#include "robomongo/gui/widgets/explorer/ExplorerReplicaSetTreeItem.h"
#include "robomongo/gui/widgets/explorer/ExplorerReplicaSetFolderItem.h"
#include "robomongo/gui/widgets/explorer/ExplorerUserTreeItem.h"
#include <QLineEdit>
#include <QVBoxLayout>
#include <QStandardItemModel>
#include <QDebug>
#include <QTime>

namespace Robomongo
{
    ExplorerWidget::ExplorerWidget(MainWindow *parentMainWindow) : BaseClass(parentMainWindow),
        _progress(0)
    {
        _treeWidget = new ExplorerTreeWidget(this);

        _searchField = new QLineEdit;
        _searchField->setPlaceholderText("Search...");

        QVBoxLayout* vLayout = new QVBoxLayout();
        QHBoxLayout* topLayout = new QHBoxLayout();
        QHBoxLayout* bottomLayout = new QHBoxLayout();

        vLayout->addLayout(topLayout);
        vLayout->addLayout(bottomLayout);

        vLayout->setMargin(0);
        topLayout->setMargin(0);
        bottomLayout->setMargin(0);

        topLayout->addWidget(_searchField);
        bottomLayout->addWidget(_treeWidget);

        VERIFY(connect(_treeWidget, SIGNAL(itemExpanded(QTreeWidgetItem *)),
                this, SLOT(ui_itemExpanded(QTreeWidgetItem *))));

        VERIFY(connect(_treeWidget, SIGNAL(itemDoubleClicked(QTreeWidgetItem *, int)),
                this, SLOT(ui_itemDoubleClicked(QTreeWidgetItem *, int))));

        VERIFY(connect(_searchField, SIGNAL(textChanged(const QString &)),
                this, SLOT(ui_searchTextChanged(const QString &))));

        // Temporarily disabling export/import feature
        //VERIFY(connect(_treeWidget, SIGNAL(currentItemChanged(QTreeWidgetItem *, QTreeWidgetItem *)),
        //               parentMainWindow, SLOT(onExplorerItemSelected(QTreeWidgetItem *))));

        setLayout(vLayout);

        QMovie *movie = new QMovie(":robomongo/icons/loading.gif", QByteArray(), this);
        _progressLabel = new QLabel(this);
        _progressLabel->setMovie(movie);
        _progressLabel->hide();
        movie->start();        
    }

    QTreeWidgetItem* ExplorerWidget::getSelectedTreeItem() const
    {
        return _treeWidget->currentItem();
    }

    void ExplorerWidget::keyPressEvent(QKeyEvent *event)
    {
        if ((event->key() == Qt::Key_Return) || (event->key() == Qt::Key_Enter))
        {
            QList<QTreeWidgetItem*> items = _treeWidget->selectedItems();

            if (items.count() != 1) {
                BaseClass::keyPressEvent(event);
                return;
            }

            QTreeWidgetItem *item = items[0];

            if (!item) {
                BaseClass::keyPressEvent(event);
                return;
            }

            ui_itemDoubleClicked(item, 0);

            return;
        }

        BaseClass::keyPressEvent(event);
    }

    void ExplorerWidget::increaseProgress()
    {
        ++_progress;
        _progressLabel->move(width() / 2 - 8, height() / 2 - 8);
        _progressLabel->show();
    }

    void ExplorerWidget::decreaseProgress()
    {
        --_progress;

        if (_progress < 0)
            _progress = 0;

        if (!_progress)
            _progressLabel->hide();
    }

    void ExplorerWidget::handle(ConnectingEvent *event)
    {
        increaseProgress();
    }

    void ExplorerWidget::handle(ConnectionEstablishedEvent *event)
    {
        // Do not make UI changes for non PRIMARY connections
        if (event->connectionType != ConnectionPrimary)
            return;

        decreaseProgress();

        auto item = new ExplorerServerTreeItem(_treeWidget, event->server, event->connInfo);
        _treeWidget->addTopLevelItem(item);
        _treeWidget->setCurrentItem(item);
        _treeWidget->setFocus();
    }

    void ExplorerWidget::handle(ConnectionFailedEvent *event)
    {
        decreaseProgress();
    }

    void ExplorerWidget::ui_itemExpanded(QTreeWidgetItem *item)
    {
        auto categoryItem = dynamic_cast<ExplorerDatabaseCategoryTreeItem *>(item);
        if (categoryItem) {
            categoryItem->expand();
            return;
        }

        auto serverItem = dynamic_cast<ExplorerServerTreeItem *>(item);
        if (serverItem) {
            serverItem->expand();
            return;
        }

        auto replicaSetFolder = dynamic_cast<ExplorerReplicaSetFolderItem *>(item);
        if (replicaSetFolder) {
            replicaSetFolder->expand();
            return;
        }
       
        auto dirItem = dynamic_cast<ExplorerCollectionDirIndexesTreeItem *>(item);
        if (dirItem) {
            dirItem->expand();
        }
    }

    void ExplorerWidget::ui_itemDoubleClicked(QTreeWidgetItem *item, int column)
    {        
        if (auto collectionItem = dynamic_cast<ExplorerCollectionTreeItem *>(item)) {
            AppRegistry::instance().app()->openShell(collectionItem->collection());
            return;
        }

        if (auto userItem = dynamic_cast<ExplorerUserTreeItem *>(item)) {
            userItem->ui_viewUser();
            return;
        }

        auto replicaMemberItem = dynamic_cast<ExplorerReplicaSetTreeItem*>(item);
        if (replicaMemberItem && replicaMemberItem->isUp()) {
            AppRegistry::instance().app()->openShell(replicaMemberItem->server(), 
                replicaMemberItem->connectionSettings(), ScriptInfo("", true));
            return;
        }

        // Toggle expanded state
        item->setExpanded(!item->isExpanded());
    }

    void ExplorerWidget::ui_searchTextChanged(const QString & inputValue)
    {
        _searchQuery = inputValue;

        if (_loading) {
            return;
        }

        // Fetch all items
        QList<QTreeWidgetItem *> items = _treeWidget->findItems(
                QString("*"),
                Qt::MatchWrap | Qt::MatchWildcard | Qt::MatchRecursive
        );

        // Collect expandable items
        QList<QTreeWidgetItem *> collectionItems;

        // @todo: tank - make the targeted collections to be based on configuration instead of hard-coding them
        for (QTreeWidgetItem *item : items)
        {
            if (item->text(0) != "Collections") {
                continue;
            }

            QTreeWidgetItem *parent = item->parent();

            if (parent) {
                QString parentTitle = parent->text(0);

                if (parentTitle == "admin" || parentTitle == "local" || parentTitle == "config") {
                    continue;
                }
            }

            collectionItems.append(item);
        }

        // Expand collection items & their parents
        for (QTreeWidgetItem *item : collectionItems)
        {
            item->setExpanded(true);

            QTreeWidgetItem *parent = item;

            while (parent)
            {
                parent->setHidden(false);
                parent->setExpanded(true);
                parent = parent->parent();
            }
        }

        // Wait for the Collection items to load properly
        _loading = true;
        while (true) {
            QCoreApplication::processEvents(QEventLoop::AllEvents, 100);

            bool loading = false;

            for (QTreeWidgetItem *item : collectionItems)
            {
                loading = loading || (item->isExpanded() && !item->childCount());

                if (loading) {
                    break;
                }
            }

            if (loading) {
                continue;
            }

            break;
        }

        _loading = false;

        QString searchQuery = _searchQuery;

        // Generate search query segments
        QStringList queryChunks = searchQuery.split("|");
        QStringList patternChunks;

        for (const QString& queryChunk : queryChunks)
        {
            if (!queryChunk.length()) {
                continue;
            }

            patternChunks.append(QString(".*%1.*").arg(
                    queryChunk.trimmed()
                            .replace("*", ".*")
                            .replace("/", ".*/.*")
            ));
        }

        QString searchPattern = QString("(%1)").arg(patternChunks.join("|"));

        // Repopulate items
        items = _treeWidget->findItems(
                QString("*"),
                Qt::MatchWrap | Qt::MatchWildcard | Qt::MatchRecursive
        );

        // Collect item paths
        std::map<QTreeWidgetItem *, QString> itemPaths;

        for (QTreeWidgetItem *item : items)
        {
            QTreeWidgetItem *parent = item;

            QStringList pathParts;

            while (parent)
            {
                pathParts.prepend(
                        QString(qPrintable(parent->text(0))).remove(QRegExp(R"(\s+(\([0-9]+\))$)"))
                );

                parent = parent->parent();
            }

            itemPaths.insert(itemPaths.end(), { item, pathParts.join('/')});
        }

        // Resolve matches
        QList<QTreeWidgetItem *> matches;

        auto pathsIterator = itemPaths.begin();

        QRegExp searchRegExp = QRegExp(searchPattern, Qt::CaseInsensitive);

        while (pathsIterator != itemPaths.end()) {
            QString path = pathsIterator->second;

            if (path.indexOf(searchRegExp) >= 0) {
                matches.append(pathsIterator->first);
            }

            pathsIterator++;
        }

        // Update item visibilities based on matches
        for (QTreeWidgetItem *item : items)
        {
            if (item->isHidden() && item->text(99) != "tmp") {
                continue;
            }

            item->setText(99, "tmp");

            item->setHidden(searchQuery.length() && !matches.contains(item));
        }

        for (QTreeWidgetItem *match : matches)
        {
            QTreeWidgetItem *parent = match;

            while (parent)
            {
                if (parent->isHidden() && parent->text(99) == "tmp") {
                    parent->setHidden(false);
                }

                parent = parent->parent();
            }
        }
    }
}
