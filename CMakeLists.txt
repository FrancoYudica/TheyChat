# Minimum CMake version required
cmake_minimum_required(VERSION 3.10)

# Project name and version
project(ChatApp VERSION 1.0)

# Specify the C standard
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)

add_subdirectory(libs)

# Define the source files for the library
set(SOURCES
    src/messages/message.c
    src/messages/file_message.c
    src/messages/user_message.c
    src/messages/messages_types.c
    src/server/client.c
    src/server/client_list.c
    src/net/net_communication.c
    src/net/net_stream.c
    src/net/serialization/net_serializer.c
    src/net/serialization/net_message_serializer.c
)


# Add the library
add_library(chat_lib ${SOURCES})

# Include directories for the library
target_include_directories(chat_lib PUBLIC
    "${PROJECT_SOURCE_DIR}/include"
    "${PROJECT_SOURCE_DIR}/include/messages"
    "${PROJECT_SOURCE_DIR}/include/server"
    "${PROJECT_SOURCE_DIR}/include/net"
    "${PROJECT_SOURCE_DIR}/include/net/serialization"
)

# Optionally, add compiler warnings
if (MSVC)
    target_compile_options(chat_lib PRIVATE /W4 /WX)
else()
    target_compile_options(chat_lib PRIVATE -Wall -Wextra -Wpedantic)
endif()


add_executable(server src/server/server.c)
target_link_libraries(server PRIVATE chat_lib)
target_link_libraries(server PRIVATE dependencies_lib)

add_executable(client src/client/client.c)
target_link_libraries(client PRIVATE chat_lib)
target_link_libraries(client PRIVATE dependencies_lib)

# Add the test executables

# Enable testing
enable_testing()

# Set output directory for test executables
set(TEST_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests)

# Automatically find and add test executables
file(GLOB TEST_SOURCES "src/tests/*.c" "src/tests/*/*.c")
foreach(TEST_SOURCE ${TEST_SOURCES})
    get_filename_component(TEST_NAME ${TEST_SOURCE} NAME_WE)
    add_executable(${TEST_NAME} ${TEST_SOURCE})
    target_link_libraries(${TEST_NAME} PRIVATE chat_lib)
    target_link_libraries(${TEST_NAME} PRIVATE dependencies_lib)
    set_target_properties(${TEST_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${TEST_OUTPUT_DIRECTORY})
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_OUTPUT_DIRECTORY}/${TEST_NAME})
endforeach()