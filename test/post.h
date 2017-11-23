#pragma once
#include "dataobject.h"
#include "datatypes.h"

class Post : public CuteData::DataObject
{
    Q_OBJECT
    QD_PROPERTY(title, QString, m_title)
    QD_PROPERTY(body, QString, m_body)
    QD_MANY_TO_ONE_RELATIONSHIP(user, User, posts)
    QD_ONE_TO_MANY_RELATIONSHIP(comments, Comment, post)
    QD_MANY_TO_MANY_RELATIONSHIP(tags, Tag, posts)

public:
    Q_INVOKABLE Post() {}

    void init(UserPtr pUser, const QString &title, const QString &body);

    QString title() const { return m_title; }
    void setTitle(const QString &title) { m_title = title; }

    QString body() const { return m_body; }
    void setBody(const QString &body) { m_body = body; }
};
