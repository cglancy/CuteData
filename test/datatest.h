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
#ifndef CGDATA_DATATEST_H
#define CGDATA_DATATEST_H
#pragma once

#include "dataobject.h"

#include <QDate>
#include <QTime>
#include <QDateTime>
#include <QColor>

namespace cg
{
    class DataManager;
}

class Class1 : public cg::DataObject
{
    Q_OBJECT
    Q_PROPERTY(QString stringValue READ stringValue WRITE setStringValue)
    Q_PROPERTY(bool boolValue READ boolValue WRITE setBoolValue)
    Q_PROPERTY(double doubleValue READ doubleValue WRITE setDoubleValue)
    Q_PROPERTY(QDate dateValue READ dateValue WRITE setDateValue)
    Q_PROPERTY(QTime timeValue READ timeValue WRITE setTimeValue)
    Q_PROPERTY(QDateTime dateTimeValue READ dateTimeValue WRITE setDateTimeValue)
    Q_PROPERTY(QColor colorValue READ colorValue WRITE setColorValue)

public:
    Q_INVOKABLE Class1() {}

    QString stringValue() const { return m_stringValue; }
    void setStringValue(const QString &sValue) { m_stringValue = sValue; }

    bool boolValue() const { return m_boolValue; }
    void setBoolValue(bool bValue) { m_boolValue = bValue; }

    double doubleValue() const { return m_doubleValue; }
    void setDoubleValue(double dValue) { m_doubleValue = dValue; }

    QDate dateValue() const { return m_dateValue; }
    void setDateValue(const QDate &dValue) { m_dateValue = dValue; }

    QTime timeValue() const { return m_timeValue; }
    void setTimeValue(const QTime &tValue) { m_timeValue = tValue; }

    QDateTime dateTimeValue() const { return m_dateTimeValue; }
    void setDateTimeValue(const QDateTime &dtValue) { m_dateTimeValue = dtValue; }

    QColor colorValue() const { return m_colorValue; }
    void setColorValue(const QColor &color) { m_colorValue = color; }

private:
    QString m_stringValue;
    bool m_boolValue;
    double m_doubleValue;
    QDate m_dateValue;
    QTime m_timeValue;
    QDateTime m_dateTimeValue;
    QColor m_colorValue;
};

typedef QSharedPointer<Class1> Class1Ptr;

class Class2 : public Class1
{
    Q_OBJECT
    Q_PROPERTY(int intValue READ intValue WRITE setIntValue)

public:
    Q_INVOKABLE Class2() 
        : m_intValue(1) {}

    int intValue() const { return m_intValue; }
    void setIntValue(int value) { m_intValue = value; }

private:
    int m_intValue;
};

typedef QSharedPointer<Class2> Class2Ptr;

class DataTest : public QObject
{
    Q_OBJECT
public:
    DataTest();
    ~DataTest();

private slots:
    void init();
    void cleanup();
    void testClass1Class2();
    void testDataModel();
    void testTextSearch();

private:
    cg::DataManager *m_pDataManager;
};

#endif // CGDATA_DATATEST_H