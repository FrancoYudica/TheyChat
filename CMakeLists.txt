# Minimum CMake version required
cmake_minimum_required(VERSION 3.10)

# Project name and version
project(ChatApp VERSION 1.0)

# Specify the C standard
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)

# Define the source files for the library
set(SOURCES
    src/message.c
    src/messages/file_message.c
    src/messages/user_message.c
    src/server/client.c
    src/server/client_list.c
    src/net/net_communication.c
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

# Add the main executable
# add_executable(message_test src/message_test.c)
#target_link_libraries(message_test PRIVATE chat_lib)

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
    set_target_properties(${TEST_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${TEST_OUTPUT_DIRECTORY})
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_OUTPUT_DIRECTORY}/${TEST_NAME})
endforeach()