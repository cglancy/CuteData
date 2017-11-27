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
#ifndef CGDATA_POST_H
#define CGDATA_POST_H
#pragma once

#include "dataobject.h"
#include "datatypes.h"

class Post : public cg::DataObject
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

#endif // CGDATA_POST_H