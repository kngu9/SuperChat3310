#include "SuperChatSplDcps.h"
#include "ccpp_SuperChat.h"
#include "dds_type_aliases.h"

const char *
__SuperChat_message__name(void)
{
    return (const char*)"SuperChat::message";
}

const char *
__SuperChat_message__keys(void)
{
    return (const char*)"";
}

const char *
__SuperChat_chatroom__name(void)
{
    return (const char*)"SuperChat::chatroom";
}

const char *
__SuperChat_chatroom__keys(void)
{
    return (const char*)"";
}

const char *
__SuperChat_user__name(void)
{
    return (const char*)"SuperChat::user";
}

const char *
__SuperChat_user__keys(void)
{
    return (const char*)"";
}

#include <v_kernel.h>
#include <v_topic.h>
#include <os_stdlib.h>
#include <string.h>
#include <os_report.h>

c_bool
__SuperChat_message__copyIn(
    c_base base,
    struct ::SuperChat::message *from,
    struct _SuperChat_message *to)
{
    c_bool result = OS_C_TRUE;
    (void) base;

    {
        typedef c_char _DestType[144];
        _DestType *dest = &to->message;
        memcpy (dest, from->message, sizeof (*dest));
    }
    to->uuid = (c_ulonglong)from->uuid;
    to->cksum = (c_ulonglong)from->cksum;
    to->chatroom_idx = (c_ulong)from->chatroom_idx;
    return result;
}

c_bool
__SuperChat_chatroom__copyIn(
    c_base base,
    struct ::SuperChat::chatroom *from,
    struct _SuperChat_chatroom *to)
{
    c_bool result = OS_C_TRUE;
    (void) base;

    to->chatroom_idx = (c_ulong)from->chatroom_idx;
    {
        typedef c_char _DestType[25];
        _DestType *dest = &to->chatroom_name;
        memcpy (dest, from->chatroom_name, sizeof (*dest));
    }
    return result;
}

c_bool
__SuperChat_user__copyIn(
    c_base base,
    struct ::SuperChat::user *from,
    struct _SuperChat_user *to)
{
    c_bool result = OS_C_TRUE;
    (void) base;

    {
        typedef c_char _DestType[8];
        _DestType *dest = &to->nick;
        memcpy (dest, from->nick, sizeof (*dest));
    }
    to->uuid = (c_ulonglong)from->uuid;
    to->chatroom_idx = (c_ulong)from->chatroom_idx;
    return result;
}

void
__SuperChat_message__copyOut(
    void *_from,
    void *_to)
{
    struct _SuperChat_message *from = (struct _SuperChat_message *)_from;
    struct ::SuperChat::message *to = (struct ::SuperChat::message *)_to;
    {
        typedef c_char _DestType[144];
        _DestType *src = &from->message;

        memcpy (to->message, src, sizeof (*src));
    }
    to->uuid = (::DDS::ULongLong)from->uuid;
    to->cksum = (::DDS::ULongLong)from->cksum;
    to->chatroom_idx = (::DDS::ULong)from->chatroom_idx;
}

void
__SuperChat_chatroom__copyOut(
    void *_from,
    void *_to)
{
    struct _SuperChat_chatroom *from = (struct _SuperChat_chatroom *)_from;
    struct ::SuperChat::chatroom *to = (struct ::SuperChat::chatroom *)_to;
    to->chatroom_idx = (::DDS::ULong)from->chatroom_idx;
    {
        typedef c_char _DestType[25];
        _DestType *src = &from->chatroom_name;

        memcpy (to->chatroom_name, src, sizeof (*src));
    }
}

void
__SuperChat_user__copyOut(
    void *_from,
    void *_to)
{
    struct _SuperChat_user *from = (struct _SuperChat_user *)_from;
    struct ::SuperChat::user *to = (struct ::SuperChat::user *)_to;
    {
        typedef c_char _DestType[8];
        _DestType *src = &from->nick;

        memcpy (to->nick, src, sizeof (*src));
    }
    to->uuid = (::DDS::ULongLong)from->uuid;
    to->chatroom_idx = (::DDS::ULong)from->chatroom_idx;
}

