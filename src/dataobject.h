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
#ifndef CGDATA_DATAOBJECT_H
#define CGDATA_DATAOBJECT_H
#pragma once

#include "cgdata.h"
#include <QObject>
#include <QEnableSharedFromThis>
#include <QList>

#define QD_PROPERTY(name, type, variable) \
    Q_PROPERTY(type name MEMBER variable) \
    type variable;

#define QD_TO_ONE_RELATIONSHIP(name, classname) \
    Q_CLASSINFO(#name, "-1:" #classname) \
    Q_PROPERTY(qint64 name MEMBER qd_##name) \
    qint64 qd_##name;

#define QD_ONE_TO_ONE_RELATIONSHIP(name, classname, inverse) \
    Q_CLASSINFO(#name, "1-1:" #classname ":" #inverse) \
    Q_PROPERTY(qint64 name MEMBER qd_##name) \
    qint64 qd_##name;

#define QD_MANY_TO_ONE_RELATIONSHIP(name, classname, inverse) \
    Q_CLASSINFO(#name, "N-1:" #classname ":" #inverse) \
    Q_PROPERTY(qint64 name MEMBER qd_##name) \
    qint64 qd_##name;

#define QD_ONE_TO_MANY_RELATIONSHIP(name, classname, inverse) \
    Q_CLASSINFO(#name, "1-N:" #classname ":" #inverse)

#define QD_MANY_TO_MANY_RELATIONSHIP(name, classname, inverse) \
    Q_CLASSINFO(#name, "N-N:" #classname ":" #inverse)


namespace cg
{
    class DataManager;
    class DataObject;
    typedef QSharedPointer<DataObject> DataObjectPtr;
    typedef QSharedPointer<const DataObject> ConstDataObjectPtr;
    typedef QList<DataObjectPtr> DataObjects;

    class CGDATA_API DataObject : public QObject, public QEnableSharedFromThis<DataObject>
    {
        Q_OBJECT
            QD_PROPERTY(id, qint64, m_id)

    public:
        Q_INVOKABLE DataObject();
        virtual ~DataObject();

        DataManager * dataManager() const { return m_pDataManager; }
        qint64 id() const { return m_id; }

        void read();
        void update();
        void del(bool cascade = true);

        template <class T>
        QSharedPointer<T> one(const QString &relationshipName) const
        {
            DataObjectPtr pDataObject = one(&T::staticMetaObject, relationshipName);
            return pDataObject.dynamicCast<T>();
        }

        void setOne(const QString &relationshipName, DataObjectPtr pTargetObject);

        template <class T>
        QList<QSharedPointer<T>> many(const QString &relationshipName) const
        {
            DataObjects objects = many(&T::staticMetaObject, relationshipName);

            QList<QSharedPointer<T>> list;
            for (auto &pObject : objects)
                list.append(pObject.dynamicCast<T>());

            return list;
        }

        void add(const QString &relationshipName, DataObjectPtr pObject);
        void remove(const QString &relationshipName, DataObjectPtr pObject);
        void removeAll(const QString &relationshipName);

    private:
        DataObjectPtr one(const QMetaObject *pMetaObject, const QString &name) const;
        DataObjects many(const QMetaObject *pMetaObject, const QString &name) const;

    private:
        friend class DataManager;
        DataManager *m_pDataManager;
    };

}

#endif // CGDATA_DATAOBJECT_H