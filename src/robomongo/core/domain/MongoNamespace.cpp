#include <QString>
#include <QVariant>
#include "robomongo/core/domain/MongoNamespace.h"
#include "robomongo/core/AppRegistry.h"
#include "robomongo/core/settings/SettingsManager.h"

namespace Robomongo
{
    MongoNamespace::MongoNamespace(const std::string &ns) :
        _ns(ns)
    {
        size_t dot = ns.find_first_of('.');
        _collectionName = ns.substr(dot + 1);
        _databaseName = ns.substr(0, dot);
    }

    MongoNamespace::MongoNamespace(const std::string &database, const std::string &collection) :
        _databaseName(database),
        _collectionName(collection)
    {
        _ns = _databaseName + ".";
        _ns += _collectionName;
    }

    MongoNamespace::MongoNamespace(
        const std::string &database, const std::string &collection, const std::string &connection
    ) :
        _databaseName(database),
        _collectionName(collection),
        _connectionName(connection)
    {
        _ns = _databaseName + ".";
        _ns += _collectionName;

        auto const& settingsManager = Robomongo::AppRegistry::instance().settingsManager();
        auto connectionAliases = settingsManager->connectionAliases();

        QMapIterator<QString, QVariant> iConnectionAliases(connectionAliases);

        QString connGroup;
        QString connSubGroup;

        while (iConnectionAliases.hasNext()) {
            iConnectionAliases.next();

            connGroup = iConnectionAliases.key();

            auto aliasGroupItems = iConnectionAliases.value().toMap();

            connSubGroup = aliasGroupItems.key(QString::fromStdString(connection), "");

            if (connSubGroup.length()) {
                break;
            }
        }

        _connectionPath = {
            QString("%1:%2").arg(connGroup, connSubGroup).toStdString(),
            QString("%1:%2").arg(connGroup, "*").toStdString(),
            "*:*",
        };
    }
}
