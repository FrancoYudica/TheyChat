#include <malloc.h>
#include <string.h>
#include "shared_file_list.h"
#include "collections/linked_list.h"

struct SharedFileList {
    LinkedList* list;
    uint32_t next_file_id;
};

struct SharedFileListIterator {
    LinkedListIterator* it;
};

SharedFileList* shared_file_list_create()
{
    SharedFileList* file_list = (SharedFileList*)malloc(sizeof(SharedFileList));
    file_list->next_file_id = 0;
    file_list->list = linked_list_create(sizeof(SharedFile));
    return file_list;
}
void shared_file_list_destroy(SharedFileList* list)
{
    linked_list_destroy(list->list);
    free(list);
}
SharedFile* shared_file_list_add(SharedFileList* list)
{
    SharedFile* file = (SharedFile*)linked_list_add(list->list);
    file->id = list->next_file_id++;
    return file;
}
bool shared_file_list_remove(SharedFileList* list, uint32_t id)
{
    SharedFile* file = NULL;
    bool found = false;
    uint32_t i = 0;
    SharedFileListIterator* it = shared_file_list_iterator_create(list);
    while (file = shared_file_list_iterator_next(it)) {
        if (file->id == id) {
            found = linked_list_remove(list->list, i);
            break;
        }

        i++;
    }

    shared_file_list_iterator_destroy(it);

    return found;
}

uint32_t shared_file_list_get_next_id(SharedFileList* list)
{
    return list->next_file_id;
}

SharedFile* shared_file_list_find_by_id(SharedFileList* list, uint32_t id)
{
    SharedFile* file = NULL;
    SharedFileListIterator* it = shared_file_list_iterator_create(list);
    while (file = shared_file_list_iterator_next(it)) {
        if (file->id == id)
            break;
    }

    shared_file_list_iterator_destroy(it);

    return file; // Will be NULL if not found
}

SharedFile* shared_file_list_find_by_name(SharedFileList* list, const char* name)
{
    SharedFile* file = NULL;
    SharedFileListIterator* it = shared_file_list_iterator_create(list);
    while (file = shared_file_list_iterator_next(it)) {
        if (!strcmp(file->filename, name))
            break;
    }

    shared_file_list_iterator_destroy(it);

    return file; // Will be NULL if not found
}

SharedFileListIterator* shared_file_list_iterator_create(SharedFileList* list)
{
    SharedFileListIterator* it = (SharedFileListIterator*)malloc(sizeof(SharedFileListIterator));
    it->it = linked_list_iterator_create(list->list);
    return it;
}
SharedFile* shared_file_list_iterator_next(SharedFileListIterator* iterator)
{
    return (SharedFile*)linked_list_iterator_next(iterator->it);
}
void shared_file_list_iterator_destroy(SharedFileListIterator* iterator)
{
    linked_list_iterator_destroy(iterator->it);
    free(iterator);
}