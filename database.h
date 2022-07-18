#ifndef DATABASE_H
#define DATABASE_H

#include <QString>
#include <QSqlDatabase>
#include <QSqlQueryModel>
#include <QSqlDriver>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlError>
#include <QSqlRecord>
#include <QSqlQueryModel>


class Database {
private:
    QSqlDatabase m_db;

public:
    Database();
    bool createTable(QSqlQuery* query);
    bool setEntry(const QString& name, QSqlQuery* query);
    QByteArray getEntry(const QString& entry, QSqlQuery* query);
    void getAllEntries(QSqlQuery* query);
    bool getTransaction();
    bool dropTable(QSqlQuery* query);
    bool dropEntry(QSqlQuery* query, const QString& entry);
};

#endif // DATABASE_H
