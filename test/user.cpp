#include "user.h"
#include "datamanager.h"
#include "post.h"
#include "comment.h"

void User::init(const QString &name, const QString &email)
{
    m_name = name;
    m_email = email;
}
