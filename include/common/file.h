#ifndef __THEY_CHAT_FILE_H__
#define __THEY_CHAT_FILE_H__

#ifdef _WIN32
#define PATH_SEPARATOR '\\'
#elif defined(__unix__)
#define PATH_SEPARATOR '/'
#endif

const char* filepath_get_filename(const char* filepath);

#endif