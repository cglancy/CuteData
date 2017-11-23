#pragma once
#include "dataobject.h"
#include "datatypes.h"

class UserProfile : public CuteData::DataObject
{
    Q_OBJECT
    QD_PROPERTY(address, QString, m_address)
    QD_ONE_TO_ONE_RELATIONSHIP(user, User, profile)

public:
    Q_INVOKABLE UserProfile() {}
    
    void init(UserPtr pUser, const QString &address);

    QString address() const { return m_address; }
    void setAddress(const QString &address) { m_address = address; }
};