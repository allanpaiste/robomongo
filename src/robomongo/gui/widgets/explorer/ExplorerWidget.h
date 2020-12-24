#pragma once

#include <QWidget>
#include <QLineEdit>
#include <QSortFilterProxyModel>

QT_BEGIN_NAMESPACE
class QTreeWidget;
class QTreeWidgetItem;
class QLabel;
QT_END_NAMESPACE

#include "robomongo/core/events/MongoEvents.h"
#include "ExplorerDatabaseTreeItem.h"
#include "ExplorerServerTreeItem.h"

namespace Robomongo
{
    class MainWindow;

    /**
     * @brief Explorer widget (usually you'll see it at the left of main window)
     */
    class ExplorerWidget : public QWidget
    {
        Q_OBJECT

    public:
        typedef QWidget BaseClass;
        ExplorerWidget(MainWindow *parent);
        QTreeWidgetItem* getSelectedTreeItem() const;

    protected Q_SLOTS:
        void handle(ConnectingEvent *event);
        void handle(ConnectionEstablishedEvent *event);
        void handle(ConnectionFailedEvent *event);
        void handle(MongoExplorerTreePopulated *event);
        void handle(MongoExplorerTreeServerAdded *event);
    private Q_SLOTS:
        void ui_itemExpanded(QTreeWidgetItem *item);
        void ui_itemDoubleClicked(QTreeWidgetItem *item, int column);
        void ui_searchTextChanged(const QString &searchQuery);

    protected:
        virtual void keyPressEvent(QKeyEvent *event);   

    private:
        int _progress;
        void increaseProgress();
        void decreaseProgress();
        void ensureConnections(const QStringList &searchQuery);
        void expandSearchableFolders(QTreeWidget *treeWidget);
        void applySearchFilter(QTreeWidget *treeWidget, const QString &searchQuery);
        static MongoServer* resolveConnectionRecord(const QString &connectionName);
        QLabel *_progressLabel;
        QTreeWidget *_treeWidget;
        QLineEdit *_searchField;
        QString _searchQuery;
        bool _loading;
    };
}
