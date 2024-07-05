#ifndef __CHAT_ENTRY_TYPES__
#define __CHAT_ENTRY_TYPES__

#include <time.h>
#include <stdbool.h>
#include <stdint.h>
#include "messages/message.h"
#include "string/string_list.h"

enum ChatEntryType {
    CHAT_ENTRY_USER_TEXT,
    CHAT_ENTRY_TEXT,
    CHAT_ENTRY_LIST
};

// User text chat, entry that holds data of user messages --------------------------------------
typedef struct
{
    char name[MAX_USERNAME_BYTES];
    char text[MAX_CHAT_TEXT_BYTES];
} UserTextChatEntry;

// Text entry, shows text with colors ----------------------------------------------------------
enum TextEntryType {
    TEXT_ENTRY_TYPE_LOG,
    TEXT_ENTRY_TYPE_SERVER,
    TEXT_ENTRY_TYPE_WARNING
};
typedef struct
{
    enum TextEntryType text_type;
    char text[MAX_CHAT_TEXT_BYTES];
} TextChatEntry;

// List entry, holds a list of strings with a header -------------------------------------------
typedef struct
{
    char header[MAX_CHAT_TEXT_BYTES];
    StringList* list;
} ListChatEntry;

// ---------------------------------------------------------------------------------------------
typedef union {
    UserTextChatEntry user_text;
    TextChatEntry text;
    ListChatEntry list;
} ChatEntryData;

typedef struct {
    enum ChatEntryType type;
    char time_str[80];
    ChatEntryData data;
} ChatEntry;

/// @brief Sets chat entry time to string
void chat_entry_format_time(ChatEntry* entry, time_t t);

ChatEntry* chat_entry_create_user(const char* name, const char* text);
ChatEntry* chat_entry_create_text(enum TextEntryType type, const char* text);
ChatEntry* chat_entry_create_list(const char* header);

void chat_entry_free(ChatEntry* entry);

#endif