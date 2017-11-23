#pragma once
#include "dataobject.h"
#include "datatypes.h"

class User : public CuteData::DataObject
{
    Q_OBJECT
    QD_PROPERTY(name, QString, m_name)
    QD_PROPERTY(email, QString, m_email)
    QD_ONE_TO_MANY_RELATIONSHIP(posts, Post, user)
    QD_ONE_TO_MANY_RELATIONSHIP(comments, Comment, user)
    QD_MANY_TO_MANY_RELATIONSHIP(followers, User, followees)
    QD_ONE_TO_ONE_RELATIONSHIP(profile, UserProfile, user)

public:
    Q_INVOKABLE User() {}

    void init(const QString &name, const QString &email);

    QString name() const { return m_name; }
    void setName(const QString &name) { m_name = name; }

    QString email() const { return m_email; }
    void setEmail(const QString &email) { m_email = email; }
};

