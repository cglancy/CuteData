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
#ifndef CGDATA_DATAMANAGER_H
#define CGDATA_DATAMANAGER_H
#pragma once

#include "cgdata.h"
#include "dataobject.h"

#include <QPointer>
#include <QSqlDatabase>
#include <QMap>
#include <QVariant>
#include <QPair>

namespace cg
{

    class Table;
    class Relationship;

    class CGDATA_API DataManager : public QObject
    {
        Q_OBJECT
    public:
        DataManager(QList<const QMetaObject*> &metaObjectList);
        ~DataManager();

        bool isOpen() const;
        bool open(const QString &path);
        void close();

        template <class T>
        QSharedPointer<T> createObject()
        {
            DataObjectPtr pDataObject = newObject(&T::staticMetaObject);
            return pDataObject.dynamicCast<T>();
        }

        void readObject(DataObjectPtr pObject);
        void updateObject(DataObjectPtr pObject);
        void deleteObject(DataObjectPtr pObject, bool cascade = true);

        template <class T>
        QSharedPointer<T> object(qint64 id) const
        {
            DataObjectPtr pDataObject = findObject(&T::staticMetaObject, id);
            return pDataObject.dynamicCast<T>();
        }

        template <class T>
        QList<QSharedPointer<T>> all() const
        {
            DataObjects objects = findAllObjects(&T::staticMetaObject);

            QList<QSharedPointer<T>> list;
            for (auto &pObject : objects)
                list.append(pObject.dynamicCast<T>());

            return list;
        }

        template <class T>
        QList<QSharedPointer<T>> find(const QVariantMap &map) const
        {
            DataObjects objects = findObjects(&T::staticMetaObject, map);

            QList<QSharedPointer<T>> list;
            for (auto &pObject : objects)
                list.append(pObject.dynamicCast<T>());

            return list;
        }

        template <class T>
        QList<QSharedPointer<T>> textSearch(const QString &text) const
        {
            DataObjects objects = textSearch(&T::staticMetaObject, text);

            QList<QSharedPointer<T>> list;
            for (auto &pObject : objects)
                list.append(pObject.dynamicCast<T>());

            return list;
        }

        DataObjectPtr one(ConstDataObjectPtr pObject, const QMetaObject *pMetaObject, const QString &name) const;
        DataObjects many(ConstDataObjectPtr pObject, const QMetaObject *pMetaObject, const QString &name) const;
        void setOne(DataObjectPtr pObject, const QString &relationshipName, DataObjectPtr pTargetObject);
        void add(DataObjectPtr pObject, const QString &relationshipName, DataObjectPtr pTargetObject);
        void remove(DataObjectPtr pObject, const QString &relationshipName, DataObjectPtr pTargetObject);
        void removeAll(DataObjectPtr pObject, const QString &relationshipName);

    signals:
        void databaseOpened();
        void databaseClosed();
        void databaseChanged();
        void objectCreated(DataObjectPtr pObject);
        void objectUpdated(DataObjectPtr pObject);
        void objectDeleted(DataObjectPtr pObject);

    private:
        DataObjectPtr newObject(const QMetaObject *pMetaObject);
        DataObjectPtr constructObject(const QMetaObject *pMetaObject) const;
        void mapObject(const QMetaObject *pMetaObject, DataObjectPtr pObject) const;
        DataObjectPtr findObject(const QMetaObject *pMetaObject, qint64 id) const;
        DataObjects findAllObjects(const QMetaObject *pMetaObject) const;
        DataObjects findObjects(const QMetaObject *pMetaObject, const QVariantMap &map) const;
        void clearObjects();

        void createVirtualTable(const QString &tableName, const QStringList &textColumnList);
        DataObjects textSearch(const QMetaObject *pMetaObject, const QString &text) const;

    private:
        static QString toSQLiteTypeString(QVariant::Type type);
        static QVariant toSQLiteVariant(const QVariant &value);
        static QVariant fromSQLiteVariant(QVariant::Type propertyType, const QVariant &value);
        static QVariantMap insertData(DataObjectPtr pObject, QString &columnsString, QString &valuesString);
        static QVariantMap updateData(DataObjectPtr pObject, QString &updateString);
        static QStringList selectData(const QMetaObject *pMetaObject, QString &selectString, QMap<QString, QVariant::Type> &typeMap, bool includeId = false);
        static QVariantMap objectProperties(DataObjectPtr pObject);
        static void setObjectProperties(DataObjectPtr pObject, const QVariantMap &values);
        static QString tableName(const QMetaObject *pMetaObject1, const QString &name1, const QMetaObject *pMetaObject2, const QString &name2);

    private:
        QSqlDatabase m_database;
        QMap<QString, Table*> m_tableMap;
        QList<Relationship*> m_relationships;
        typedef QMap<qint64, QWeakPointer<DataObject>> ObjectMap;
        mutable QMap<QString, ObjectMap> m_classObjectMap;
    };

}

#endif // CGDATA_DATAMANAGER_H