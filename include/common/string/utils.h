#ifndef __STRING_UTILS_H__
#define __STRING_UTILS_H__
#include <stdbool.h>

/**
 * @brief Trims leading and trailing whitespace from a string.
 *
 * @param str The input string to be trimmed.
 * @return char* A pointer to the trimmed string.
 */
char* trim(char* str);

/**
 * @brief Splits a string at the first occurrence of a delimiter character.
 *
 * @param str The input string to be split.
 * @param delimiter The character at which to split the string.
 * @return char* Pointer to the second part of the split string.
 */
char* split_string_inplace(char* str, char delimiter);

/**
 * @brief Checks if a string starts with a specified prefix.
 *
 * @param str The input string.
 * @param prefix The prefix to check for.
 * @return true if the string starts with the prefix, false otherwise.
 */
bool starts_with(const char* str, const char* prefix);

#endif