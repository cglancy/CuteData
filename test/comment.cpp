#include "comment.h"
#include "user.h"
#include "post.h"
#include "datamanager.h"

void Comment::init(UserPtr pUser, PostPtr pPost, const QString &body)
{
    m_body = body;
    setOne("user", pUser);
    setOne("post", pPost);
}

