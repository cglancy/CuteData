#include "dataobject.h"
#include "datamanager.h"

#include <QMetaObject>
#include <QMetaClassInfo>

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

