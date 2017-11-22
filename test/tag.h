#pragma once
#include "dataobject.h"
#include "datatypes.h"

class Tag : public DataObject
{
    Q_OBJECT
    QD_PROPERTY(name, QString, m_name)
    QD_MANY_TO_MANY_RELATIONSHIP(posts, Post, tags)
    QD_TO_ONE_RELATIONSHIP(user, User)

public:
    Q_INVOKABLE Tag() {}
 
    QString name() const { return m_name; }
    void setName(const QString &name) { m_name = name; }
};
