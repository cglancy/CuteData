/**
* Copyright 2017 Charles Glancy (charles@glancyfamily.net)
*
* Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation
* files (the "Software"), to deal in the Software without restriction, including  without limitation the rights to use, copy,
* modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software
* is furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
* WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
* COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
* ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include "datamanager.h"
#include "dataobject.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QFileInfo>
#include <QDataStream>
#include <QMetaProperty>
#include <QDateTime>
#include <QColor>
#include <QDebug>

namespace cg
{

class Relationship
{
public:
    enum Type
    {
        ToOneType,
        OneToOneType,
        OneToManyType,
        ManyToOneType,
        ManyToManyType
    };

public:
    Relationship(Type type, const QMetaObject *pMetaObject, const QString &name)
        : m_type(type), m_pMetaObject(pMetaObject), m_name(name), m_pInverseRelationship(nullptr)
    {
        m_className = pMetaObject->className();
    }

    Type type() { return m_type; }
    QString name() { return m_name; }
    const QMetaObject * metaObject() { return m_pMetaObject; }

    void setInverseRelationship(Relationship *pInverse) { m_pInverseRelationship = pInverse; }
    Relationship * inverseRelationship() const { return m_pInverseRelationship; }

private:
    Type m_type;
    const QMetaObject *m_pMetaObject;
    QString m_name, m_className;
    Relationship *m_pInverseRelationship;
};

class Table
{
public:
    Table(const QMetaObject *pMetaObject) 
        : m_pMetaObject(pMetaObject), m_pRelationship1(nullptr), m_pRelationship2(nullptr)
    {
        m_name = pMetaObject->className();
        //m_name += "_table";
    }

    Table(Relationship *pRelationship1, Relationship *pRelationship2, const QString &name)
        : m_pMetaObject(nullptr), m_pRelationship1(pRelationship1), m_pRelationship2(pRelationship2), m_name(name)
    {
    }

    const QMetaObject * metaObject() const { return m_pMetaObject; }
    Relationship * relationship1() const { return m_pRelationship1; }
    Relationship * relationship2() const { return m_pRelationship2; }
    QString name() const { return m_name; }

    void addRelationship(const QString &name, Relationship *pRelationship) { m_relationshipMap.insert(name, pRelationship); }
    Relationship * relationship(const QString &name) const { return m_relationshipMap.value(name); }

    void addDependentPair(const QString &tableName, const QString &columnName) { m_dependentPairs.append(QPair<QString, QString>(tableName, columnName)); }
    QList<QPair<QString, QString>> dependentPairs() const { return m_dependentPairs; }

    QList<Relationship*> relationships() const { return m_relationshipMap.values(); }

private:
    const QMetaObject *m_pMetaObject;
    Relationship *m_pRelationship1, *m_pRelationship2;
    QString m_name;
    QMap<QString, Relationship*> m_relationshipMap;
    QList<QPair<QString, QString>> m_dependentPairs;
};



DataManager::DataManager(QList<const QMetaObject*> &metaObjectList)
{
    for (auto & pMetaObject : metaObjectList)
    {
        m_classObjectMap.insert(pMetaObject->className(), ObjectMap());

        Table *pClassTable = new Table(pMetaObject);
        m_tableMap.insert(pMetaObject->className(), pClassTable);
    }

    for (auto & pMetaObject1 : metaObjectList)
    {
        int count = pMetaObject1->classInfoCount();
        for (int i = 0; i < count; i++)
        {
            QMetaClassInfo classInfo = pMetaObject1->classInfo(i);
            QString name1 = classInfo.name();

            Table *pTable1 = m_tableMap.value(pMetaObject1->className());
            Relationship *pRelationship1 = pTable1->relationship(name1);

            if (!pRelationship1)
            {
                QStringList list = QString(classInfo.value()).split(":");
                if (list.size() == 2)
                {
                    QString typeStr = list.at(0);
                    QString className = list.at(1);

                    Table *pTable2 = m_tableMap.value(className);

                    if (typeStr == "-1")
                    {
                        pRelationship1 = new Relationship(Relationship::ToOneType, pMetaObject1, name1);
                        pTable2->addDependentPair(pMetaObject1->className(), name1);
                        pTable1->addRelationship(pRelationship1->name(), pRelationship1);
                        m_relationships.append(pRelationship1);
                    }
                }
                else if (list.size() == 3)
                {
                    QString typeStr = list.at(0);
                    QString className = list.at(1);
                    QString name2 = list.at(2);

                    Table *pTable2 = m_tableMap.value(className);
                    const QMetaObject *pMetaObject2 = pTable2->metaObject();
                    Relationship *pRelationship2 = nullptr;

                    if (typeStr == "1-1")
                    {
                        pRelationship1 = new Relationship(Relationship::OneToOneType, pMetaObject1, name1);
                        pRelationship2 = new Relationship(Relationship::OneToOneType, pMetaObject2, name2);
                        pRelationship1->setInverseRelationship(pRelationship2);
                        pRelationship2->setInverseRelationship(pRelationship1);

                        pTable1->addDependentPair(pMetaObject2->className(), name2);
                        pTable2->addDependentPair(pMetaObject1->className(), name1);
                    }
                    else if (typeStr == "N-1")
                    {
                        pRelationship1 = new Relationship(Relationship::ManyToOneType, pMetaObject1, name1);
                        pRelationship2 = new Relationship(Relationship::OneToManyType, pMetaObject2, name2);
                        pRelationship1->setInverseRelationship(pRelationship2);
                        pRelationship2->setInverseRelationship(pRelationship1);

                        pTable2->addDependentPair(pMetaObject1->className(), name1);
                    }
                    else if (typeStr == "1-N")
                    {
                        pRelationship1 = new Relationship(Relationship::OneToManyType, pMetaObject1, name1);
                        pRelationship2 = new Relationship(Relationship::ManyToOneType, pMetaObject2, name2);
                        pRelationship1->setInverseRelationship(pRelationship2);
                        pRelationship2->setInverseRelationship(pRelationship1);

                        pTable1->addDependentPair(pMetaObject2->className(), name2);
                    }
                    else if (typeStr == "N-N")
                    {
                        pRelationship1 = new Relationship(Relationship::ManyToManyType, pMetaObject1, name1);
                        pRelationship2 = new Relationship(Relationship::ManyToManyType, pMetaObject2, name2);
                        pRelationship1->setInverseRelationship(pRelationship2);
                        pRelationship2->setInverseRelationship(pRelationship1);

                        QString name = tableName(pMetaObject1, name1, pMetaObject2, name2);
                        Table *pManyToManyTable = new Table(pRelationship1, pRelationship2, name);
                        m_tableMap.insert(name, pManyToManyTable);

                        pTable1->addDependentPair(name, name2);
                        pTable2->addDependentPair(name, name1);
                    }

                    if (pRelationship1)
                    {
                        pTable1->addRelationship(pRelationship1->name(), pRelationship1);
                        m_relationships.append(pRelationship1);
                    }

                    if (pRelationship2)
                    {
                        pTable2->addRelationship(pRelationship2->name(), pRelationship2);
                        m_relationships.append(pRelationship2);
                    }
                }
            }
        }
    }
}

DataManager::~DataManager()
{
    close();

    for (auto & key : m_tableMap.keys())
        delete m_tableMap.value(key);

    for (auto & pRelationship : m_relationships)
        delete pRelationship;
}

void DataManager::close()
{
    clearObjects();

    if (m_database.isOpen())
        m_database.close();

    emit databaseClosed();
}

void DataManager::clearObjects()
{
    for (auto &className : m_classObjectMap.keys())
    {
        auto & objectMap = m_classObjectMap[className];
        objectMap.clear();
    }
}

bool DataManager::isOpen() const
{
    return m_database.isOpen();
}

bool DataManager::open(const QString &path)
{
    if (m_database.isOpen())
        return false;

    clearObjects();

    QFileInfo fi(path);
    bool dbExists = fi.exists();

    m_database = QSqlDatabase::addDatabase("QSQLITE");
    m_database.setDatabaseName(path);

    if (!m_database.open())
    {
        qDebug() << "Error: unable to open database.";
    }
    else if (!dbExists)
    {
        for (auto & className : m_tableMap.keys())
        {
            Table *pTable = m_tableMap.value(className);
            const QMetaObject *pMetaObject = pTable->metaObject();

            if (pMetaObject)
            {
                QString columnsString;
                int count = pMetaObject->propertyCount();

                for (int i = 0; i < count; ++i)
                {
                    QMetaProperty property = pMetaObject->property(i);
                    QString name = property.name();
                    QString type = toSQLiteTypeString(property.type());

                    columnsString += name + " " + type;
                    if (name == "id")
                        columnsString += " primary key";
                    if (i != count - 1)
                        columnsString += ", ";
                }

                QSqlQuery query;
                query.prepare(QString("CREATE TABLE %1 (%2)").arg(pTable->name()).arg(columnsString));
                if (query.exec())
                {
                    //qDebug() << "Table created for " << pTable->name();
                }
                else
                {
                    qDebug() << "Error: Unable to create table for " << pTable->name();
                }
            }
            else if (pTable->relationship1() && pTable->relationship2())
            {
                Relationship *pRelationship1 = pTable->relationship1();
                Relationship *pRelationship2 = pTable->relationship2();

                QSqlQuery query;
                query.prepare(QString("CREATE TABLE %1 (%2 INTEGER, %3 INTEGER)")
                    .arg(pTable->name())
                    .arg(pRelationship1->name())
                    .arg(pRelationship2->name()));
                if (query.exec())
                {
                    //qDebug() << "Table created for " << pTable->name();
                }
                else
                {
                    qDebug() << "Error: Unable to create table for " << pTable->name();
                }
            }
        }
    }

    emit databaseOpened();

    return true;
}

DataObjectPtr DataManager::newObject(const QMetaObject *pMetaObject)
{
    if (!pMetaObject)
        return nullptr;

    DataObjectPtr pDataObject = constructObject(pMetaObject);
    if (!pDataObject)
        return nullptr;

    QString columnsStr, valuesStr;
    QVariantMap map = insertData(pDataObject, columnsStr, valuesStr);

    Table *pTable = m_tableMap.value(pMetaObject->className());

    QSqlQuery query;
    query.prepare(QString("INSERT INTO %1 (%2) VALUES (%3)").arg(pTable->name()).arg(columnsStr).arg(valuesStr));

    for (auto &key : map.keys())
        query.bindValue(":" + key, map.value(key));

    if (query.exec())
    {
        //qDebug() << "Success: " << query.executedQuery();

        qint64 id = query.lastInsertId().toLongLong();
        pDataObject->setProperty("id", id);
        mapObject(pMetaObject, pDataObject);

        emit objectCreated(pDataObject);
        emit databaseChanged();
    }
    else
    {
        qDebug() << "Error: newObject, " << query.lastError();
        qDebug() << "Query = " << query.executedQuery();
    }

    return pDataObject;
}

void DataManager::readObject(DataObjectPtr pDataObject)
{
    if (!pDataObject)
        return;

    QString selectStr;
    QMap<QString, QVariant::Type> typeMap;
    QStringList list = selectData(pDataObject->metaObject(), selectStr, typeMap);

    Table *pTable = m_tableMap.value(pDataObject->metaObject()->className());

    QSqlQuery query;
    query.prepare(QString("SELECT %1 FROM %2 WHERE id = :id").arg(selectStr).arg(pTable->name()));
    query.bindValue(":id", pDataObject->id());
    if (query.exec())
    {
        if (query.next())
        {
            for (int i = 0; i < list.size(); i++)
            {
                QString name = list[i];
                QVariant::Type type = typeMap.value(name);
                QVariant value = fromSQLiteVariant(type, query.value(i));
                if (value.isValid())
                    pDataObject->setProperty(name.toUtf8(), value);
            }
        }
    }
    else
    {
        qDebug() << "Error: readObject, " << query.lastError();
    }
}

DataObjectPtr DataManager::constructObject(const QMetaObject *pMetaObject) const
{
    if (!pMetaObject)
        return nullptr;

    QObject *pObject = pMetaObject->newInstance();
    if (!pObject)
        return nullptr; // ERROR

    DataObject* pDataObject = nullptr;
    pDataObject = qobject_cast<DataObject*>(pObject);

    if (!pDataObject)
        return nullptr; // ERROR

    // TODO: is there a way around const_cast here?
    pDataObject->m_pDataManager = const_cast<DataManager*>(this);

    // initialize any foreign keys
    Table *pTable = m_tableMap.value(pMetaObject->className());

    for (auto & pRelationship : pTable->relationships())
    {
        if (pRelationship->type() == Relationship::OneToOneType ||
            pRelationship->type() == Relationship::ManyToOneType)
        {
            pDataObject->setProperty(pRelationship->name().toLocal8Bit(), QVariant(0));
        }
    }

    return QSharedPointer<DataObject>(pDataObject);
}

void DataManager::mapObject(const QMetaObject *pMetaObject, DataObjectPtr pObject) const
{
    if (!m_classObjectMap.contains(pMetaObject->className()))
        return;

    auto & objectMap = m_classObjectMap[pMetaObject->className()];
    objectMap.insert(pObject->id(), pObject);
}

DataObjectPtr DataManager::findObject(const QMetaObject *pMetaObject, qint64 id) const
{
    DataObjectPtr pObject = nullptr;

    QString className = pMetaObject->className();

    if (!m_classObjectMap.contains(className))
        return nullptr;

    auto & objectMap = m_classObjectMap[className];

    if (objectMap.contains(id) && !objectMap.value(id).isNull())
    {
        pObject = objectMap.value(id).lock();
    }
    else
    {
        QString selectStr;
        QMap<QString, QVariant::Type> typeMap;
        QStringList list = selectData(pMetaObject, selectStr, typeMap);

        Table *pTable = m_tableMap.value(pMetaObject->className());

        QSqlQuery query;
        query.prepare(QString("SELECT %1 FROM %2 WHERE id = :id").arg(selectStr).arg(pTable->name()));
        query.bindValue(":id", id);
        if (query.exec())
        {
            if (query.next())
            {
                pObject = constructObject(pMetaObject);
                if (!pObject)
                    return nullptr; // ERROR

                pObject->setProperty("id", id);
                mapObject(pMetaObject, pObject);

                for (int i = 0; i < list.size(); i++)
                {
                    QString name = list[i];
                    QVariant::Type type = typeMap.value(name);
                    QVariant value = fromSQLiteVariant(type, query.value(i));
                    if (value.isValid())
                        pObject->setProperty(name.toUtf8(), value);
                }
            }
        }
        else
        {
            qDebug() << "Error: findObject, id = " << id << ", not found" << query.lastError();
        }
    }

    return pObject;
}

DataObjects DataManager::findAllObjects(const QMetaObject *pMetaObject) const
{
    DataObjects objectList;

    QString className = pMetaObject->className();
    Table *pTable = m_tableMap.value(className);

    QString selectStr;
    QMap<QString, QVariant::Type> typeMap;
    QStringList list = selectData(pMetaObject, selectStr, typeMap, true);

    QSqlQuery query;
    query.prepare(QString("SELECT %1 FROM %2").arg(selectStr).arg(pTable->name()));

    if (query.exec())
    {
        while (query.next())
        {
            QVariantMap map;

            for (int i = 0; i < list.size(); i++)
            {
                QString name = list[i];
                QVariant::Type type = typeMap.value(name);
                QVariant value = fromSQLiteVariant(type, query.value(i));
                if (value.isValid())
                    map.insert(name, value);
            }

            qint64 id = map.value("id").toLongLong();

            auto & objectMap = m_classObjectMap[className];

            if (objectMap.contains(id) && !objectMap.value(id).isNull())
            {
                DataObjectPtr pObject = objectMap.value(id).lock();
                objectList.append(pObject);
            }
            else
            {
                DataObjectPtr pObject = constructObject(pMetaObject);
                if (pObject)
                {
                    setObjectProperties(pObject, map);
                    mapObject(pMetaObject, pObject);
                    objectList.append(pObject);
                }
            }
        }
    }

    return objectList;
}

DataObjects DataManager::findObjects(const QMetaObject *pMetaObject, const QVariantMap &map) const
{
    DataObjects objectList;

    QString className = pMetaObject->className();
    Table *pTable = m_tableMap.value(className);

    QString selectStr;
    QMap<QString, QVariant::Type> typeMap;
    QStringList list = selectData(pMetaObject, selectStr, typeMap, true);

    QString whereClause;
    QStringList keys = map.keys();

    for (int i = 0; i < keys.size(); i++)
    {
        whereClause += QString("%1 = :%1").arg(keys[i]);
        if (i != keys.size() - 1)
            whereClause += ", ";
    }

    QSqlQuery query;
    query.prepare(QString("SELECT %1 FROM %2 WHERE %3").arg(selectStr).arg(pTable->name()).arg(whereClause));

    for (auto &key : keys)
        query.bindValue(":" + key, map.value(key));

    if (query.exec())
    {
        while (query.next())
        {
            QVariantMap map;

            for (int i = 0; i < list.size(); i++)
            {
                QString name = list[i];
                QVariant::Type type = typeMap.value(name);
                QVariant value = fromSQLiteVariant(type, query.value(i));
                if (value.isValid())
                    map.insert(name, value);
            }

            qint64 id = map.value("id").toLongLong();

            auto & objectMap = m_classObjectMap[className];

            if (objectMap.contains(id) && !objectMap.value(id).isNull())
            {
                DataObjectPtr pObject = objectMap.value(id).lock();
                objectList.append(pObject);
            }
            else
            {
                DataObjectPtr pObject = constructObject(pMetaObject);
                if (pObject)
                {
                    setObjectProperties(pObject, map);
                    mapObject(pMetaObject, pObject);
                    objectList.append(pObject);
                }
            }
        }
    }

    return objectList;
}

QStringList DataManager::selectData(const QMetaObject *pMetaObject, QString &selectString, QMap<QString, QVariant::Type> &typeMap, bool includeId)
{
    QStringList list;

    selectString.clear();
    typeMap.clear();

    int count = pMetaObject->propertyCount();

    for (int i = 0; i < count; ++i)
    {
        QMetaProperty property = pMetaObject->property(i);
        QString name = property.name();
        typeMap.insert(name, property.type());

        if (name != "id" || includeId)
        {
            list.append(name);
            selectString += name;

            if (i != count - 1)
                selectString += ", ";
        }
    }

    return list;
}

void DataManager::deleteObject(DataObjectPtr pObject, bool cascade)
{
    if (!pObject)
        return;

    QString className = pObject->metaObject()->className();
    Table *pTable = m_tableMap.value(className);

    if (!m_classObjectMap.contains(className))
        return;

    auto & objectMap = m_classObjectMap[className];
    objectMap.remove(pObject->id());

    QSqlQuery query;
    query.prepare(QString("DELETE FROM %1 WHERE id = (:id)").arg(pTable->name()));
    query.bindValue(":id", pObject->id());
    if (query.exec())
    {
        emit objectDeleted(pObject);
        emit databaseChanged();
    }
    else
    {
        qDebug() << "Error: deleteObject, " << query.lastError();
    }

    // cascade delete
    if (cascade)
    {
        Table *pTable = m_tableMap.value(pObject->metaObject()->className());
        auto & dependencyList = pTable->dependentPairs();
        for (auto & pair : dependencyList)
        {
            QString subClassName = pair.first;
            QString columnName = pair.second;

            Table *pSubTable = m_tableMap.value(subClassName);

            QSqlQuery subquery;
            subquery.prepare(QString("DELETE FROM %1 WHERE %2 = (:%2)").arg(pSubTable->name()).arg(columnName));
            subquery.bindValue(":" + columnName, pObject->id());
            if (subquery.exec())
            {
                //emit objectDeleted(?);
                emit databaseChanged();
            }
            else
            {
                qDebug() << "Error: cascade deleteObject, " << query.lastError();
            }
        }
    }
}

void DataManager::updateObject(DataObjectPtr pObject)
{
    if (!pObject)
        return;

    QString className = pObject->metaObject()->className();
    Table *pTable = m_tableMap.value(className);

    if (!m_classObjectMap.contains(className))
        return;

    QString updateStr;
    QVariantMap map = updateData(pObject, updateStr);

    QSqlQuery query;
    query.prepare(QString("UPDATE %1 SET %2 WHERE id = :id").arg(pTable->name()).arg(updateStr));

    for (auto &key : map.keys())
        query.bindValue(":" + key, map.value(key));

    if (query.exec())
    {
        //qDebug() << "Success: " << query.executedQuery();
        emit objectUpdated(pObject);
        emit databaseChanged();
    }
    else
    {
        qDebug() << "Error: saveObject, " << query.lastError();
        qDebug() << "Query = " << query.executedQuery();
    }

    return;
}

QVariantMap DataManager::insertData(DataObjectPtr pObject, QString &columnsString, QString &valuesString)
{
    QVariantMap map;

    columnsString.clear();
    valuesString.clear();

    const QMetaObject *pMetaObject = pObject->metaObject();
    int count = pMetaObject->propertyCount();

    for (int i = 0; i < count; ++i)
    {
        QMetaProperty property = pMetaObject->property(i);
        QString name = property.name();

        if (name != "id")
        {
            columnsString += name;
            valuesString += ":" + name;
            map.insert(name, toSQLiteVariant(pObject->property(property.name())));

            if (i != count - 1)
            {
                columnsString += ", ";
                valuesString += ", ";
            }
        }
    }

    return map;
}

QVariantMap DataManager::updateData(DataObjectPtr pObject, QString &updateString)
{
    QVariantMap map;

    updateString.clear();

    const QMetaObject *pMetaObject = pObject->metaObject();
    int count = pMetaObject->propertyCount();

    for (int i = 0; i < count; ++i)
    {
        QMetaProperty property = pMetaObject->property(i);
        QString name = property.name();
        map.insert(name, toSQLiteVariant(pObject->property(property.name())));

        if (name != "id")
        {
            updateString += QString("%1 = :%1").arg(name);

            if (i != count - 1)
                updateString += ", ";
        }
    }

    return map;
}

QVariantMap DataManager::objectProperties(DataObjectPtr pObject)
{
    QVariantMap map;

    const QMetaObject *pMetaObject = pObject->metaObject();
    int count = pMetaObject->propertyCount();

    for (int i = 0; i < count; ++i)
    {
        QMetaProperty property = pMetaObject->property(i);
        map.insert(property.name(), pObject->property(property.name()));
    }

    return map;
}

void DataManager::setObjectProperties(DataObjectPtr pObject, const QVariantMap &map)
{
    for (auto &key : map.keys())
        pObject->setProperty(key.toUtf8(), map.value(key));
}

QString DataManager::tableName(const QMetaObject * pMetaObject1, const QString & name1, const QMetaObject * pMetaObject2, const QString & name2)
{
    if (!pMetaObject1 || !pMetaObject2)
        return QString();

    QString part1 = QString("%1_%2").arg(pMetaObject1->className()).arg(name1);
    QString part2 = QString("%1_%2").arg(pMetaObject2->className()).arg(name2);

    QString name;
    if (part2 < part1)
        name = part2 + "__" + part1;
    else
        name = part1 + "__" + part2;

    return name;
}

QString DataManager::toSQLiteTypeString(QVariant::Type type)
{
    QString typeStr;

    switch (type)
    {
    case QVariant::Int:
    case QVariant::UInt:
    case QVariant::LongLong:
    case QVariant::ULongLong:
    case QVariant::Bool:
        typeStr = "INTEGER";
        break;
    case QVariant::String:
    case QVariant::Char:
    case QVariant::Uuid:
    case QVariant::Date:
    case QVariant::DateTime:
    case QVariant::Time:
    case QVariant::Color:
        typeStr = "TEXT";
        break;
    case QVariant::Double:
        typeStr = "REAL";
        break;
    case QVariant::ByteArray:
    default:
        typeStr = "BLOB";
    }

    return typeStr;
}

QVariant DataManager::toSQLiteVariant(const QVariant &value)
{
    QVariant returnValue;

    switch (value.type())
    {
    case QVariant::Date:
        returnValue = value.toDate().toString(Qt::ISODate);
        break;
    case QVariant::Time:
        returnValue = value.toTime().toString(Qt::ISODate);
        break;
    case QVariant::DateTime:
        returnValue = value.toDateTime().toString(Qt::ISODate);
        break;
    case QVariant::Color:
        returnValue = value.value<QColor>().name();
        break;
    case QVariant::Bool:
    case QVariant::Int:
        returnValue = value.toInt();;
        break;
    case QVariant::UInt:
        returnValue = value.toUInt();;
        break;
    case QVariant::LongLong:
        returnValue = value.toLongLong();;
        break;
    case QVariant::ULongLong:
        returnValue = value.toULongLong();;
        break;
    case QVariant::String:
    case QVariant::Char:
    case QVariant::Uuid:
        returnValue = value.toString();
        break;
    case QVariant::Double:
        returnValue = value.toDouble();
        break;
    case QVariant::ByteArray:
    default:
        returnValue = value.toByteArray();
    }

    return returnValue;
}

QVariant DataManager::fromSQLiteVariant(QVariant::Type propertyType, const QVariant &value)
{
    QVariant returnValue;

    switch (propertyType)
    {
    case QVariant::DateTime:
        returnValue = QDateTime::fromString(value.toString(), Qt::ISODate);
        break;
    case QVariant::Date:
        returnValue = QDate::fromString(value.toString(), Qt::ISODate);
        break;
    case QVariant::Time:
        returnValue = QTime::fromString(value.toString(), Qt::ISODate);
        break;
    case QVariant::Color:
        returnValue = QColor(value.toString());
        break;
    default:
        returnValue = value;
        returnValue.convert(propertyType);
    }

    return returnValue;
}

DataObjectPtr DataManager::one(ConstDataObjectPtr pObject, const QMetaObject *pMetaObject, const QString &relationshipName) const
{
    Table *pObjectTable = m_tableMap.value(pObject->metaObject()->className());
    if (pObjectTable)
    {
        Relationship *pRelationship = pObjectTable->relationship(relationshipName);
        if (pRelationship)
        {
            if (pRelationship->type() == Relationship::ToOneType ||
                pRelationship->type() == Relationship::OneToOneType ||
                pRelationship->type() == Relationship::ManyToOneType)
            {
                qint64 id = pObject->property(relationshipName.toLocal8Bit()).toLongLong();
                return findObject(pMetaObject, id);
            }
        }
    }

    return DataObjectPtr();
}

DataObjects DataManager::many(ConstDataObjectPtr pObject, const QMetaObject *pMetaObject, const QString &relationshipName) const
{
    Q_UNUSED(pMetaObject);
    DataObjects objects;

    if (!pObject)
        return objects;

    Table *pObjectTable = m_tableMap.value(pObject->metaObject()->className());
    if (pObjectTable)
    {
        Relationship *pRelationship = pObjectTable->relationship(relationshipName);
        Relationship *pInverseRelationship = pRelationship->inverseRelationship();

        if (pRelationship && pInverseRelationship && pRelationship->type() == Relationship::OneToManyType)
        {
            QVariantMap map;
            map.insert(pInverseRelationship->name(), pObject->id());
            objects = findObjects(pInverseRelationship->metaObject(), map);
        }
        else if (pRelationship && pInverseRelationship && pRelationship->type() == Relationship::ManyToManyType)
        {
            QString inverseName = pInverseRelationship->name();
            QString manyToManyName = tableName(pObject->metaObject(), relationshipName, pInverseRelationship->metaObject(), inverseName);

            QString whereStr = QString("%1 = :%1").arg(inverseName);

            QSqlQuery query;
            query.prepare(QString("SELECT %1 FROM %2 WHERE %3").arg(relationshipName).arg(manyToManyName).arg(whereStr));
            query.bindValue(":" + inverseName, pObject->id());
            if (query.exec())
            {
                QList<qint64> ids;
                while (query.next())
                    ids.append(query.value(0).toLongLong());

                for (auto id : ids)
                {
                    DataObjectPtr pObject = findObject(pInverseRelationship->metaObject(), id);
                    if (pObject)
                        objects.append(pObject);
                }
            }
            else
            {
                qDebug() << "DataManager::many() query failed.";
            }
        }
    }

    return objects;
}

void DataManager::setOne(DataObjectPtr pObject, const QString &relationshipName, DataObjectPtr pTargetObject)
{
    if (!pTargetObject)
        return;

    Table *pTable = m_tableMap.value(pObject->metaObject()->className());
    if (pTable)
    {
        Relationship *pRelationship = pTable->relationship(relationshipName);
        if (pRelationship)
        {
            if (pRelationship->type() == Relationship::ToOneType || 
                pRelationship->type() == Relationship::OneToOneType ||
                pRelationship->type() == Relationship::ManyToOneType)
            {
                pObject->setProperty(pRelationship->name().toLocal8Bit(), pTargetObject->id());
            }
        }
    }
}

void DataManager::add(DataObjectPtr pObject, const QString &relationshipName, DataObjectPtr pTargetObject)
{
    if (!pObject)
        return;

    Table *pTable = m_tableMap.value(pObject->metaObject()->className());
    if (pTable)
    {
        Relationship *pRelationship = pTable->relationship(relationshipName);
        if (pRelationship)
        {
            Relationship *pInverseRelationship = pRelationship->inverseRelationship();
            if (pInverseRelationship)
            {
                if (pRelationship->type() == Relationship::ManyToManyType)
                {
                    QString manyToManyName = tableName(pObject->metaObject(), pRelationship->name(), pTargetObject->metaObject(), pInverseRelationship->name());
                    Table *pManyToManyTable = m_tableMap.value(manyToManyName);
                    if (pManyToManyTable)
                    {
                        QString name2 = pInverseRelationship->name();

                        QString columnsStr, valuesStr;
                        columnsStr = relationshipName + ", ";
                        columnsStr += name2;

                        valuesStr = ":" + relationshipName + ", ";
                        valuesStr += ":" + name2;

                        QSqlQuery query;
                        query.prepare(QString("INSERT INTO %1 (%2) VALUES (%3)").arg(pManyToManyTable->name()).arg(columnsStr).arg(valuesStr));
                        query.bindValue(":" + relationshipName, pTargetObject->id());
                        query.bindValue(":" + name2, pObject->id());
                        if (query.exec())
                        {
                        }
                        else
                        {
                            qDebug() << "DataManager::add() query failed.";
                        }
                    }
                }
            }
        }
    }
}

void DataManager::remove(DataObjectPtr pObject, const QString &relationshipName, DataObjectPtr pTargetObject)
{
    if (!pObject || !pTargetObject)
        return;

    Table *pTable = m_tableMap.value(pObject->metaObject()->className());
    if (pTable)
    {
        Relationship *pRelationship = pTable->relationship(relationshipName);
        if (pRelationship)
        {
            Relationship *pInverseRelationship = pRelationship->inverseRelationship();
            if (pInverseRelationship)
            {
                if (pRelationship->type() == Relationship::ManyToManyType)
                {
                    QString manyToManyName = tableName(pObject->metaObject(), pRelationship->name(), pTargetObject->metaObject(), pInverseRelationship->name());
                    Table *pManyToManyTable = m_tableMap.value(manyToManyName);
                    if (pManyToManyTable)
                    {
                        QString inverseName = pInverseRelationship->name();

                        QString whereStr;
                        whereStr = QString("%1 = :%1 AND %2 = :%2").arg(relationshipName).arg(inverseName);

                        QSqlQuery query;
                        query.prepare(QString("DELETE FROM %1 WHERE %2").arg(manyToManyName).arg(whereStr));
                        query.bindValue(":" + inverseName, pObject->id());
                        query.bindValue(":" + relationshipName, pTargetObject->id());

                        if (query.exec())
                        {
                        }
                        else
                        {
                            qDebug() << "DataManager::remove() query failed.";
                        }
                    }
                }
            }
        }
    }
}

void DataManager::removeAll(DataObjectPtr pObject, const QString &relationshipName)
{
    if (!pObject)
        return;

    Table *pTable = m_tableMap.value(pObject->metaObject()->className());
    if (pTable)
    {
        Relationship *pRelationship = pTable->relationship(relationshipName);
        if (pRelationship)
        {
            Relationship *pInverseRelationship = pRelationship->inverseRelationship();
            if (pInverseRelationship)
            {
                if (pRelationship->type() == Relationship::ManyToManyType)
                {
                    QString manyToManyName = tableName(pObject->metaObject(), pRelationship->name(), pInverseRelationship->metaObject(), pInverseRelationship->name());
                    Table *pManyToManyTable = m_tableMap.value(manyToManyName);
                    if (pManyToManyTable)
                    {
                        QString inverseName = pInverseRelationship->name();

                        QSqlQuery query;
                        query.prepare(QString("DELETE FROM %1 WHERE %2 = :%2").arg(manyToManyName).arg(inverseName));
                        query.bindValue(":" + inverseName, pObject->id());

                        if (query.exec())
                        {
                        }
                        else
                        {
                            qDebug() << "DataManager::removeAll() query failed.";
                        }
                    }
                }
            }
        }
    }
}
}
