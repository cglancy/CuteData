#pragma once
#include "dataobject.h"
#include "datatypes.h"

class Comment : public DataObject
{
    Q_OBJECT
    QD_PROPERTY(body, QString, m_body)
    QD_MANY_TO_ONE_RELATIONSHIP(user, User, comments)
    QD_MANY_TO_ONE_RELATIONSHIP(post, Post, comments)

public:
    Q_INVOKABLE Comment() {}
    
    void init(UserPtr pUser, PostPtr pPost, const QString &body);
 
    QString body() const { return m_body; }
    void setBody(const QString &body) { m_body = body; }
};
