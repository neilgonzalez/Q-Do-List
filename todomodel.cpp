#include "todomodel.h"
#include "qsqlquery.h"
#include "database.h"

ToDoList::ToDoList(QObject *parent) : QObject(parent)
{
    //data that loads when app starts
    Database db;
    QSqlQuery *query = new QSqlQuery;
    query->prepare("SELECT * FROM people");
    query->exec();
    int idName = query->record().indexOf("name");
    while (query->next())
    {
       QString name = query->value(idName).toString();
       mItems.append({false, name});

       qDebug() << name;
    }

}


QVector<ToDoItem> ToDoList::items() const
{
    return mItems;
}

//takes index and enum ToDoItem and places it into mItems
bool ToDoList::setItemAt(int index, const ToDoItem &item)
{
    /*
     * check if the index is valid
     */
    if(index<0 || index >= mItems.size())
        return false;

    const ToDoItem &oldItem = mItems.at(index);
    /*
     * checks if new item has identical
     * values or not to check whether we are changing
     * the data or not
     */
    if(item.done == oldItem.done && item.description == oldItem.description){
        return false;
    }

    mItems[index] = item;
    return true;
}

//Once a button is clicked, append a ToDoItem
void ToDoList::appendItem()
{
    emit preItemAppend();
    ToDoItem item;
    item.done = false;
    mItems.append(item);
    emit postItemAppended();
}
//check on all items and remove the ones that are done
void ToDoList::removeCompletedItems()
{
    for (int i=0;i<mItems.size();) {
        if(mItems.at(i).done){
            Database db;
            QSqlQuery* query = new QSqlQuery;
            db.dropEntry(query, mItems.at(i).description);
            emit preItemRemoved(i);
            mItems.removeAt(i);

            emit postItemRemoved();
        }
        else{
            ++i;
        }
    }
}

//------------------------------------------------


ToDoModel::ToDoModel(QObject *parent)
    : QAbstractListModel(parent)
    , mList(nullptr)
{
}

int ToDoModel::rowCount(const QModelIndex &parent) const
{
    // For list models only the root node (an invalid parent) should return the list's size. For all
    // other (valid) parents, rowCount() should return 0 so that it does not become a tree model.
    // safe gaurd against null pointer accesses
    if (parent.isValid() || !mList)
        return 0;

    return mList->items().size();
}

QVariant ToDoModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || !mList)
        return QVariant();

    const ToDoItem item = mList->items().at(index.row());
    switch (role) {
        case DoneRole:
            /*
             * QVariant class acts like a union
             * for most common qt data types
             */
            return QVariant(item.done);
        case DescriptionRole:
            return QVariant(item.description);
    }
    return QVariant();
}

//given a role, assign done or description to the item in mList
bool ToDoModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if(!mList){
        return false;
    }
    ToDoItem item = mList->items().at(index.row());
    switch (role) {
        case DoneRole:
            //now we convert back the QVariant by typecasting
            item.done = value.toBool();
            break;
        case DescriptionRole:
            item.description = value.toString();
            break;
    }

    //check whether data has changed
    if (mList->setItemAt(index.row(),item)) {

        Database db;
        QSqlQuery* query = new QSqlQuery;

        // db.dropTable(query);

        db.createTable(query);

        QString name2 = value.toString();
        if (name2 != "true")
            db.setEntry(name2, query);


        db.getAllEntries(query);

        emit dataChanged(index, index, QList<int>() << role);
        return true;
    }
    return false;
}

Qt::ItemFlags ToDoModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    return Qt::ItemIsEditable;
}

QHash<int, QByteArray> ToDoModel::roleNames() const
{
    QHash<int, QByteArray> names;
    names[DoneRole] = "done";
    names[DescriptionRole] = "description";
    return names;
}

ToDoList *ToDoModel::list() const
{
    return mList;
}


//connect slots and signals to mList
void ToDoModel::setList(ToDoList *list)
{
    beginResetModel();

    if(mList){
        //disconnet from the old list when assigning to
        // a new list
        mList->disconnect(this);
    }
    mList = list;

    if(mList){
        connect(mList, &ToDoList::preItemAppend, this, [=](){
           const int index = mList->items().size();
           beginInsertRows(QModelIndex(), index, index);
        });

        connect(mList, &ToDoList::postItemAppended, this, [=](){
            endInsertRows();
        });

        connect(mList, &ToDoList::preItemRemoved, this, [=](int index){
            beginRemoveRows(QModelIndex(), index, index);
        });

        connect(mList, &ToDoList::postItemRemoved, this, [=](){
           endRemoveRows();
        });

    }

    endResetModel();
}
