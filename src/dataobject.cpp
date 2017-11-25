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
#include "dataobject.h"
#include "datamanager.h"

#include <QMetaObject>
#include <QMetaClassInfo>

namespace CuteData
{
    DataObject::DataObject()
        : m_pDataManager(nullptr), m_id(0)
    {
    }

    DataObject::~DataObject()
    {
    }

    void DataObject::read()
    {
        if (m_pDataManager)
            m_pDataManager->readObject(sharedFromThis());
    }

    void DataObject::update()
    {
        if (m_pDataManager)
            m_pDataManager->updateObject(sharedFromThis());
    }

    void DataObject::del(bool cascade)
    {
        if (m_pDataManager)
            m_pDataManager->deleteObject(sharedFromThis(), cascade);
    }

    void DataObject::setOne(const QString &relationshipName, DataObjectPtr pObject)
    {
        if (m_pDataManager)
            m_pDataManager->setOne(sharedFromThis(), relationshipName, pObject);
    }

    void DataObject::add(const QString &relationshipName, DataObjectPtr pObject)
    {
        if (m_pDataManager)
            m_pDataManager->add(sharedFromThis(), relationshipName, pObject);
    }

    void DataObject::remove(const QString &relationshipName, DataObjectPtr pObject)
    {
        if (m_pDataManager)
            m_pDataManager->remove(sharedFromThis(), relationshipName, pObject);
    }

    void DataObject::removeAll(const QString &relationshipName)
    {
        if (m_pDataManager)
            m_pDataManager->removeAll(sharedFromThis(), relationshipName);
    }

    DataObjectPtr DataObject::one(const QMetaObject *pMetaObject, const QString &name) const
    {
        if (m_pDataManager)
            return m_pDataManager->one(sharedFromThis(), pMetaObject, name);

        return DataObjectPtr();
    }

    DataObjects DataObject::many(const QMetaObject *pMetaObject, const QString &name) const
    {
        if (m_pDataManager)
            return m_pDataManager->many(sharedFromThis(), pMetaObject, name);

        return DataObjects();
    }

}