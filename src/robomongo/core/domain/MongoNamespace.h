#pragma once

#include <string>
#include <QStringList>

namespace Robomongo
{
    class MongoNamespace
    {
    public:
        MongoNamespace(const std::string &ns);
        MongoNamespace(const std::string &database, const std::string &collection, const std::string &connection);
        MongoNamespace(const std::string &database, const std::string &collection);
        MongoNamespace() {}
        std::string toString() const { return _ns; }
        std::string databaseName() const { return _databaseName; }
        std::string collectionName() const { return _collectionName; }
        std::string connectionName() const { return _connectionName; }
        bool isValid() const { return !_ns.empty(); }
        std::vector<std::string> connectionPath() { return _connectionPath; }
    private:
        std::string _ns;
        std::string _databaseName;
        std::string _collectionName;
        std::string _connectionName;
        std::vector<std::string> _connectionPath;
    };
}
