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
#ifndef CUTEDATA_COMMENT_H
#define CUTEDATA_COMMENT_H
#pragma once

#include "dataobject.h"
#include "datatypes.h"

class Comment : public CuteData::DataObject
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

#endif // CUTEDATA_COMMENT_H