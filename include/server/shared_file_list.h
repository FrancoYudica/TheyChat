#ifndef __SHARED_FILE_LIST_H__
#define __SHARED_FILE_LIST_H__

#include <stdint.h>
#include <stdbool.h>
#include "shared_file.h"

typedef struct SharedFileList SharedFileList;
typedef struct SharedFileListIterator SharedFileListIterator;

SharedFileList* shared_file_list_create();
void shared_file_list_destroy(SharedFileList* list);

SharedFile* shared_file_list_add(SharedFileList* list);
bool shared_file_list_remove(SharedFileList* list, uint32_t id);
SharedFile* shared_file_list_find_by_id(SharedFileList* list, uint32_t id);
SharedFile* shared_file_list_find_by_name(SharedFileList* list, const char*);
uint32_t shared_file_list_count_name(SharedFileList* list, const char*);

uint32_t shared_file_list_get_next_id(SharedFileList*);

SharedFileListIterator* shared_file_list_iterator_create(SharedFileList* list);
SharedFile* shared_file_list_iterator_next(SharedFileListIterator* iterator);
void shared_file_list_iterator_destroy(SharedFileListIterator* iterator);

#endif