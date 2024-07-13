#ifndef __TASK_TYPES_H__
#define __TASK_TYPES_H__

enum TaskType {
    TASK_NULL = 0,
    TASK_USERS,
    TASK_CLIENT_UPLOAD_FILE,
    TASK_CLIENT_DOWNLOAD_FILE,
    TASK_FILES,
    TASK_REMOVE_FILE
};

const char* task_get_name(enum TaskType type);

#endif