#include "task_types.h"

const char* task_get_name(enum TaskType type)
{
    static const char* s_task_names[] = {
        [TASK_NULL] = "NULL",
        [TASK_USERS] = "Users",
        [TASK_CLIENT_UPLOAD_FILE] = "Upload",
        [TASK_CLIENT_DOWNLOAD_FILE] = "Download",
        [TASK_FILES] = "Files",
        [TASK_REMOVE_FILE] = "Remove"
    };
    return s_task_names[type];
}