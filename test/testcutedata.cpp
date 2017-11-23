#include "testcutedata.h"
#include "datamanager.h"
#include "user.h"
#include "post.h"
#include "comment.h"
#include "tag.h"
#include "userprofile.h"

#include <QFile>
#include <QTest>
#include <QScopedPointer>

using namespace CuteData;

TestCuteData::TestCuteData()
    : m_pDataManager(nullptr)
{
}

TestCuteData::~TestCuteData()
{
}

void TestCuteData::init()
{
    if (m_pDataManager)
        return;

    QList<const QMetaObject*> metaObjects;
    metaObjects << &Class1::staticMetaObject;
    metaObjects << &Class2::staticMetaObject;
    metaObjects << &User::staticMetaObject;
    metaObjects << &Post::staticMetaObject;
    metaObjects << &Comment::staticMetaObject;
    metaObjects << &Tag::staticMetaObject;
    metaObjects << &UserProfile::staticMetaObject;

    m_pDataManager = new DataManager(metaObjects);

    QString filePath = "C:\\Temp\\database.db";
    QFile file(filePath);
    file.remove();

    m_pDataManager->open(filePath);
}

void TestCuteData::cleanup()
{
    if (m_pDataManager)
    {
        m_pDataManager->close();
        delete m_pDataManager;
        m_pDataManager = nullptr;
    }
}

void TestCuteData::testClass1Class2()
{
    QString testString("String1");
    bool testBool = true;
    double testDouble = 0.123456789;
    //int testInt = 42;
    QDate testDate(1967, 8, 9);
    QTime testTime(1, 57, 34);
    QDateTime testDateTime(testDate, testTime);
    QColor testColor(123, 231, 255);

    Class1Ptr pObject1 = m_pDataManager->createObject<Class1>();
    pObject1->setStringValue(testString);
    pObject1->setBoolValue(testBool);
    pObject1->setDoubleValue(testDouble);
    pObject1->setDateValue(testDate);
    pObject1->setTimeValue(testTime);
    pObject1->setDateTimeValue(testDateTime);
    pObject1->setColorValue(testColor);
    m_pDataManager->updateObject(pObject1);

    qint64 id1 = pObject1->id();
    pObject1.reset();

    Class1Ptr pObject2 = m_pDataManager->createObject<Class1>();
    pObject2->setStringValue("String2");
    m_pDataManager->updateObject(pObject2);

    Class1Ptr pFoundObject = m_pDataManager->object<Class1>(id1);
    QVERIFY(pFoundObject != nullptr);

    QCOMPARE(pFoundObject->stringValue(), testString);
    QCOMPARE(pFoundObject->boolValue(), testBool);
    QCOMPARE(pFoundObject->doubleValue(), testDouble);
    QCOMPARE(pFoundObject->dateValue(), testDate);
    QCOMPARE(pFoundObject->timeValue(), testTime);
    QCOMPARE(pFoundObject->dateTimeValue(), testDateTime);
    QCOMPARE(pFoundObject->colorValue(), testColor);

    auto class1List= m_pDataManager->all<Class1>();
    int count = class1List.size();
    QCOMPARE(count, 2);

    Class2Ptr pObject3 = m_pDataManager->createObject<Class2>();
    QCOMPARE(pObject3->intValue(), 1);
    pObject3->setStringValue("String3");
    pObject3->setIntValue(2);
    m_pDataManager->updateObject(pObject3);

    m_pDataManager->deleteObject(pFoundObject);
    m_pDataManager->deleteObject(pObject2);

    class1List = m_pDataManager->all<Class1>();
    count = class1List.size();
    QCOMPARE(count, 0);

    auto class2List = m_pDataManager->all<Class2>();
    count = class2List.size();
    QCOMPARE(count, 1);

    m_pDataManager->deleteObject(pObject3);
    pObject3.reset();

    class2List = m_pDataManager->all<Class2>();
    count = class2List.size();
    QCOMPARE(count, 0);
}

void TestCuteData::testDataModel()
{
    // create
    {
        UserPtr pUser1 = m_pDataManager->createObject<User>();
        pUser1->init("User1", "user1@example.com");
        pUser1->update();

        UserProfilePtr pUserProfile1 = m_pDataManager->createObject<UserProfile>();
        pUserProfile1->init(pUser1, "123 Address St.");
        pUserProfile1->update();

        UserPtr pUser2 = m_pDataManager->createObject<User>();
        pUser2->init("User2", "user2@example.com");
        pUser2->add("followers", pUser1);
        pUser2->update();

        PostPtr pPost1 = m_pDataManager->createObject<Post>();
        pPost1->init(pUser1, "My first post", "The body of post 1.");
        pPost1->update();

        PostPtr pPost2 = m_pDataManager->createObject<Post>();
        pPost2->init(pUser1, "My second post", "The body of post 2.");
        pPost2->update();

        CommentPtr pComment1 = m_pDataManager->createObject<Comment>();
        pComment1->init(pUser2, pPost1, "First comment!");
        pComment1->update();

        TagPtr pTag1 = m_pDataManager->createObject<Tag>();
        pTag1->setName("Tag1");
        pTag1->setOne("user", pUser1);
        pTag1->update();

        TagPtr pTag2 = m_pDataManager->createObject<Tag>();
        pTag2->setName("Tag2");
        pTag2->setOne("user", pUser2);
        pTag2->update();

        pPost1->add("tags", pTag1);

        pPost2->add("tags", pTag1);
        pPost2->add("tags", pTag2);
    }

    // query all
    {
        Users users = m_pDataManager->all<User>();
        QCOMPARE(users.size(), 2);

        UserProfiles profiles = m_pDataManager->all<UserProfile>();
        QCOMPARE(profiles.size(), 1);

        Posts posts = m_pDataManager->all<Post>();
        QCOMPARE(posts.size(), 2);

        Tags tags = m_pDataManager->all<Tag>();
        QCOMPARE(tags.size(), 2);

        Comments comments = m_pDataManager->all<Comment>();
        QCOMPARE(comments.size(), 1);

        QVariantMap map;
        map["name"] = "User1";
        Users foundUsers = m_pDataManager->find<User>(map);
        QCOMPARE(foundUsers.size(), 1);
    }

    // query
    {
        UserPtr pUser1 = m_pDataManager->object<User>(1);
        QVERIFY(pUser1 != nullptr);

        UserProfilePtr pUserProfile1 = pUser1->one<UserProfile>("profile");
        QVERIFY(pUserProfile1 != nullptr);

        Posts posts = pUser1->many<Post>("posts");
        QCOMPARE(posts.size(), 2);

        PostPtr pPost1 = m_pDataManager->object<Post>(1);
        Tags tags = pPost1->many<Tag>("tags");
        QCOMPARE(tags.size(), 1);

        UserPtr pTagUser = tags.at(0)->one<User>("user");
        QVERIFY(pTagUser == pUser1);
    }

    // delete
    {
        QVariantMap map;
        map["name"] = "User1";
        Users users = m_pDataManager->find<User>(map);
        QCOMPARE(users.size(), 1);
        UserPtr pUser1 = users.at(0);
        QVERIFY(pUser1 != nullptr);
        qint64 user1Id = pUser1->id();

        map["name"] = "User2";
        users = m_pDataManager->find<User>(map);
        QCOMPARE(users.size(), 1);
        UserPtr pUser2 = users.at(0);
        QVERIFY(pUser2 != nullptr);

        Users followers = pUser2->many<User>("followers");
        QCOMPARE(followers.size(), 1);
        QCOMPARE(followers.at(0)->id(), user1Id);

        map.clear();
        map["user"] = user1Id;
        Posts posts = m_pDataManager->find<Post>(map);
        QCOMPARE(posts.size(), 2);

        PostPtr pPost1 = posts.at(0);
        Tags tags = pPost1->many<Tag>("tags");
        QCOMPARE(tags.size(), 1);

        pPost1->remove("tags", tags.at(0));
        tags = pPost1->many<Tag>("tags");
        QCOMPARE(tags.size(), 0);

        PostPtr pPost2 = posts.at(1);
        tags = pPost2->many<Tag>("tags");
        QCOMPARE(tags.size(), 2);

        pPost2->removeAll("tags");
        tags = pPost2->many<Tag>("tags");
        QCOMPARE(tags.size(), 0);

        pUser1->del();

        users = m_pDataManager->all<User>();
        QCOMPARE(users.size(), 1);

        // verify cascade delete
        followers = pUser2->many<User>("followers");
        QCOMPARE(followers.size(), 0);

        posts = m_pDataManager->find<Post>(map);
        QCOMPARE(posts.size(), 0);
    }
}
