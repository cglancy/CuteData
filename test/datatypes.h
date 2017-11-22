#pragma once
#include <QSharedPointer>
#include <QList>

class Comment;
typedef QSharedPointer<Comment> CommentPtr;
typedef QList<CommentPtr> Comments;

class Post;
typedef QSharedPointer<Post> PostPtr;
typedef QList<PostPtr> Posts;

class Tag;
typedef QSharedPointer<Tag> TagPtr;
typedef QList<TagPtr> Tags;

class User;
typedef QSharedPointer<User> UserPtr;
typedef QList<UserPtr> Users;

class UserProfile;
typedef QSharedPointer<UserProfile> UserProfilePtr;
typedef QList<UserProfilePtr> UserProfiles;

