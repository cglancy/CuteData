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
#ifndef CUTEDATA_TAG_H
#define CUTEDATA_TAG_H
#pragma once

#include "dataobject.h"
#include "datatypes.h"

class Tag : public CuteData::DataObject
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

#endif // CUTEDATA_TAG_H