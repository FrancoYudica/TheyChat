#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "string/string_list.h"

void test_string_list_create()
{
    StringList* list = string_list_create();
    assert(list != NULL);
    assert(string_list_length(list) == 0);
    string_list_destroy(list);
    printf("test_string_list_create passed.\n");
}

void test_string_list_add()
{
    StringList* list = string_list_create();
    string_list_add(list, "Test1");
    string_list_add(list, "Test2");
    assert(string_list_length(list) == 2);

    StringListIterator* iterator = string_list_iterator_create(list);
    assert(strcmp(string_list_iterator_next(iterator), "Test1") == 0);
    assert(strcmp(string_list_iterator_next(iterator), "Test2") == 0);
    assert(string_list_iterator_next(iterator) == NULL);

    string_list_iterator_destroy(iterator);
    string_list_destroy(list);
    printf("test_string_list_add passed.\n");
}

void test_string_list_remove()
{
    StringList* list = string_list_create();
    string_list_add(list, "Test1");
    string_list_add(list, "Test2");
    string_list_add(list, "Test3");

    assert(string_list_length(list) == 3);
    assert(string_list_remove(list, 1) == true);
    assert(string_list_length(list) == 2);

    StringListIterator* iterator = string_list_iterator_create(list);
    assert(strcmp(string_list_iterator_next(iterator), "Test1") == 0);
    assert(strcmp(string_list_iterator_next(iterator), "Test3") == 0);
    assert(string_list_iterator_next(iterator) == NULL);

    string_list_iterator_destroy(iterator);
    string_list_destroy(list);
    printf("test_string_list_remove passed.\n");
}

void test_string_list_remove_invalid_index()
{
    StringList* list = string_list_create();
    string_list_add(list, "Test1");
    string_list_add(list, "Test2");

    assert(string_list_length(list) == 2);
    assert(string_list_remove(list, 3) == false);
    assert(string_list_remove(list, -1) == false);
    assert(string_list_length(list) == 2);

    string_list_destroy(list);
    printf("test_string_list_remove_invalid_index passed.\n");
}

void test_string_list_iterator()
{
    StringList* list = string_list_create();
    string_list_add(list, "Test1");
    string_list_add(list, "Test2");
    string_list_add(list, "Test3");

    StringListIterator* iterator = string_list_iterator_create(list);
    assert(strcmp(string_list_iterator_next(iterator), "Test1") == 0);
    assert(strcmp(string_list_iterator_next(iterator), "Test2") == 0);
    assert(strcmp(string_list_iterator_next(iterator), "Test3") == 0);
    assert(string_list_iterator_next(iterator) == NULL);

    string_list_iterator_destroy(iterator);
    string_list_destroy(list);
    printf("test_string_list_iterator passed.\n");
}

void test_string_list_length()
{
    StringList* list = string_list_create();
    assert(string_list_length(list) == 0);

    string_list_add(list, "Test1");
    assert(string_list_length(list) == 1);

    string_list_add(list, "Test2");
    assert(string_list_length(list) == 2);

    string_list_remove(list, 0);
    assert(string_list_length(list) == 1);

    string_list_destroy(list);
    printf("test_string_list_length passed.\n");
}

int main()
{
    test_string_list_create();
    test_string_list_add();
    test_string_list_remove();
    test_string_list_remove_invalid_index();
    test_string_list_iterator();
    test_string_list_length();

    printf("All tests passed.\n");
    return 0;
}
