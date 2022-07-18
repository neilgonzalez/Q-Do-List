#include "database.h"
#include <QDir>

//TODO: make query inactive by calling finish or clear, isactive

Database::Database(){
    QDir dbPath;
    QString path = dbPath.currentPath() +"/myDB.db";
    m_db  = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName(path);
    if (!m_db.open()) {
        qDebug() << "ERROR";
    }else{
        qDebug() << "Database initialized.";
        }
}


//creates the table using SQL query
bool Database::createTable(QSqlQuery* query)
{


        query->prepare("CREATE TABLE IF NOT EXISTS people(ids integer primary key, name text)");
        bool success  =  query->exec();
        if (!success){
            qDebug() << "Error: cannot create table";
        }
            qDebug() << "Table created.";

        return success;

    qDebug() << "Error: query inactive while trying to make table...";
    return false;
}


//sets the entry in the table using passed SQL query
bool Database::setEntry(const QString &name, QSqlQuery* query)
{

    bool success = false;

    query->prepare("INSERT INTO people (name) VALUES (:name)");
    query->bindValue(":name", name);

    success = query->exec();
    bool readSuccess = false;
    if (!success) {
        qDebug() << "Error: cannot set entry";
        QSqlError err = query->lastError();
        qDebug() << err;
        return false;
    }
        qDebug() << "Entry set into table.";
        return true;



}

QByteArray Database::getEntry(const QString &entry, QSqlQuery* query)
{

    QSqlQueryModel queryModel;
    QByteArray arr;
    query->prepare(entry);
   bool readSuccess = query->exec();
   if (readSuccess){
       qDebug() << "Query read success";
       query->first();
       arr = query->value(1).toByteArray();

   }
   else{
       qDebug() << "Error: Cannot read.";
   }

   return arr;
}

void Database::getAllEntries(QSqlQuery *query)
{
    query->prepare("SELECT * FROM people");
    query->exec();
    int idName = query->record().indexOf("name");
    while (query->next())
    {
       QString name = query->value(idName).toString();
       qDebug() << name;
    }

}

bool Database::getTransaction()
{
    return m_db.transaction();
}

bool Database::dropTable(QSqlQuery *query)
{
    query->prepare("DELETE FROM people");
    if (query->exec()) {
        qDebug() << "TABLE DROPPED";
        return true;
    }
    qDebug() << "Error: couldnt drop table.";
    return false;

}

bool Database::dropEntry(QSqlQuery* query, const QString &entry)
{
    query->prepare("DELETE FROM people WHERE name=:entry");
    query->bindValue(":entry", entry);
    if(query->exec()) {
        qDebug() << "ENTRY DELETED";
        return true;
    }
    qDebug() << "Error: Could not delete entry";
    return false;

}




