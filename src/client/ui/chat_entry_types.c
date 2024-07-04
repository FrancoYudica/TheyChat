#include <assert.h>
#include "ui/chat_entries.h"

void chat_entry_format_time(ChatEntry* entry, time_t t)
{
    struct tm* time = localtime((time_t*)&t);
    strftime(
        entry->time_str,
        sizeof(entry->time_str),
        "%Y/%m/%d %H:%M:%S",
        time);
}

static ChatEntry* create_entry()
{
    return (ChatEntry*)malloc(sizeof(ChatEntry));
}

ChatEntry* chat_entry_create_user(const char* name, const char* text)
{
    ChatEntry* entry = create_entry();
    chat_entry_format_time(entry, time(NULL));
    entry->type = CHAT_ENTRY_USER_TEXT;
    strcpy(entry->data.user_text.name, name);
    strcpy(entry->data.user_text.text, text);
    return entry;
}
ChatEntry* chat_entry_create_text(enum TextEntryType type, const char* text)
{
    ChatEntry* entry = create_entry();
    chat_entry_format_time(entry, time(NULL));
    entry->type = CHAT_ENTRY_TEXT;
    entry->data.text.text_type = type;
    strcpy(entry->data.text.text, text);
    return entry;
}
ChatEntry* chat_entry_create_list(const char* header)
{
    ChatEntry* entry = create_entry();
    chat_entry_format_time(entry, time(NULL));
    entry->type = CHAT_ENTRY_LIST;
    strcpy(entry->data.list.header, header);
    entry->data.list.list = string_list_create();
    return entry;
}

void chat_entry_free(ChatEntry* entry)
{

    switch (entry->type) {
    case CHAT_ENTRY_USER_TEXT:
    case CHAT_ENTRY_TEXT:
        break;

    case CHAT_ENTRY_LIST:
        string_list_destroy(entry->data.list.list);
        break;

    default:
        assert(false);
        break;
    }
    free(entry);
}