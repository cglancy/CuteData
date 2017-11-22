#include "post.h"
#include "user.h"
#include "datamanager.h"
#include "comment.h"
#include "tag.h"

void Post::init(UserPtr pUser, const QString & title, const QString & body)
{
    setOne("user", pUser);
    m_title = title;
    m_body = body;
}

