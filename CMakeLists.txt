cmake_minimum_required(VERSION 3.10)

project(ChatApp VERSION 1.0)

# Specify the C standard
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)

add_subdirectory(libs)

# Define the source files for the library
set(COMMON_SOURCES

    # Messages
    src/messages/message.c
    src/messages/file_message.c
    src/messages/user_message.c
    src/messages/client_message.c
    src/messages/status_message.c
    src/messages/messages_types.c
    
    # Networking
    src/net/net_communication.c
    src/net/net_stream.c
    src/net/net_error.c
    src/net/serialization/net_serializer.c
    src/net/serialization/net_message_serializer.c
)

# Add the library
add_library(chat_lib ${COMMON_SOURCES})

# Include directories for the library
target_include_directories(chat_lib PUBLIC "${PROJECT_SOURCE_DIR}/include")

# Optionally, add compiler warnings
if (MSVC)
    target_compile_options(chat_lib PRIVATE /W4 /WX)
else()
    target_compile_options(chat_lib PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Links with dependencies
target_link_libraries(chat_lib INTERFACE dependencies_lib)

# Creates server executable -----------------------------------------------------
# Specific sources for server
set(SERVER_SOURCES
    src/server/client.c
    src/server/client_list.c
    src/server/client_handler.c
    src/server/state_handlers/state_connect.c
    src/server/state_handlers/state_login.c
    src/server/state_handlers/state_chat.c
    src/server/state_handlers/state_disconnect.c
)
add_executable(server src/server/server.c ${SERVER_SOURCES})
target_include_directories(server PRIVATE "${PROJECT_SOURCE_DIR}/include/server")
target_link_libraries(server PRIVATE chat_lib)

# Creates client executable -----------------------------------------------------
# Specific sources for server
set(CLIENT_SOURCES
    src/client/state_handlers/state_connect.c
    src/client/state_handlers/state_login.c
    src/client/state_handlers/state_chat.c
    src/client/state_handlers/state_disconnect.c
)
add_executable(client src/client/client.c ${CLIENT_SOURCES})
target_link_libraries(client PRIVATE chat_lib)

# Creates test executables ------------------------------------------------------
# Enable testing
enable_testing()

# # Set output directory for test executables
set(TEST_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests)

# # Automatically find and add test executables
# file(GLOB TEST_SOURCES "src/tests/*.c" "src/tests/*/*.c")
# foreach(TEST_SOURCE ${TEST_SOURCES})

#     get_filename_component(TEST_NAME ${TEST_SOURCE} NAME_WE)
    
#     # Adds sources of client and server
#     add_executable(${TEST_NAME} ${TEST_SOURCE} ${SERVER_SOURCES} ${CLIENT_SOURCES})

#     # Adds include directories of client and server
#     target_include_directories(${TEST_NAME} PRIVATE "${PROJECT_SOURCE_DIR}/include/server")
#     target_include_directories(${TEST_NAME} PRIVATE "${PROJECT_SOURCE_DIR}/include/client")

#     target_link_libraries(${TEST_NAME} PRIVATE chat_lib)
#     set_target_properties(${TEST_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${TEST_OUTPUT_DIRECTORY})
#     add_test(NAME ${TEST_NAME} COMMAND ${TEST_OUTPUT_DIRECTORY}/${TEST_NAME})
# endforeach()