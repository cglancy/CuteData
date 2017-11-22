#include "userprofile.h"
#include "user.h"

void UserProfile::init(UserPtr pUser, const QString &address)
{
    setOne("user", pUser);
    m_address = address;

    pUser->setOne("profile", sharedFromThis());
    pUser->update();
}
