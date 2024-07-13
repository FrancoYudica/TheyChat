cmake_minimum_required(VERSION 3.10)

project(ChatApp VERSION 1.0)

# Specify the C standard
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED True)

# Define an option to compile with or without SSL
option(THEY_CHAT_SSL "Compile with SSL support" ON)
option(THEY_CHAT_COMPILE_TESTS "Compile TheyChat tests" ON)

add_subdirectory(libs)

# Define the source files for the library
set(COMMON_SOURCES

    # Messages
    src/common/application_states.c
    src/common/they_chat_error.c

    src/common/messages/message.c
    src/common/messages/messages_types.c
    
    # Networking
    src/common/net/net_communication.c
    src/common/net/net_connection.c
    src/common/net/net_stream.c
    src/common/net/file_transfer.c
    src/common/net/serialization/net_serializer.c
    src/common/net/serialization/net_message_serializer.c

    # Collections
    src/common/linked_list.c

    # File
    src/common/file.c

    # Utils
    src/common/utils/string_utils.c
    src/common/utils/string_list.c
    src/common/utils/string_list_transferring.c

)

# Changes encryption at compile time
if(THEY_CHAT_SSL)
    # If SSL is enabled, add ssl_primitives.c to the common sources
    list(APPEND COMMON_SOURCES src/common/net/net_primitives/ssl_primitives.c)
else()
    # If SSL is not enabled, add standard_primitives.c to the common sources
    list(APPEND COMMON_SOURCES src/common/net/net_primitives/standard_primitives.c)
endif()

# Add the library
add_library(chat_lib ${COMMON_SOURCES})

# Conditionally add the preprocessor definition
if(THEY_CHAT_SSL)
    target_compile_definitions(chat_lib INTERFACE THEY_CHAT_SSL)
endif()

# Include directories for the library
target_include_directories(chat_lib PUBLIC "${PROJECT_SOURCE_DIR}/include/common")

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
    src/server/server.c
    src/server/client.c
    src/server/client_list.c
    src/server/client_handler.c
    src/server/server_state_data.c
    src/server/broadcast_message.c
    src/server/shared_file.c
    src/server/shared_file_list.c
    
    src/server/state_handlers/state_connect.c
    src/server/state_handlers/state_login.c
    src/server/state_handlers/state_chat.c
    src/server/state_handlers/state_disconnect.c

    src/server/task/task_request_handler.c
    src/server/task/tasks/server_task_users.c
    src/server/task/tasks/server_task_files.c
    src/server/task/tasks/server_task_upload_file.c
    src/server/task/tasks/server_task_download_file.c
    src/server/task/tasks/server_task_remove_file.c
)
add_executable(server src/server/server_main.c ${SERVER_SOURCES})
target_include_directories(server PRIVATE "${PROJECT_SOURCE_DIR}/include/server")
target_link_libraries(server PRIVATE chat_lib)

# Sets output directory to build/server/
set_target_properties(server PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/server/)

# Copy the resources directory to the build directory
add_custom_command(
    TARGET server POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/src/server/resources $<TARGET_FILE_DIR:server>/resources
)


# Creates client executable -----------------------------------------------------
# Specific sources for client
set(CLIENT_SOURCES
    src/client/client_data.c
    src/client/states_fsm.c
    src/client/command/command.c
    src/client/command/commands/connect_handler.c
    src/client/command/commands/disconnect_handler.c
    src/client/command/commands/quit_handler.c
    src/client/command/commands/users_handler.c
    src/client/command/commands/upload_handler.c
    src/client/command/commands/download_handler.c
    src/client/command/commands/files_handler.c
    src/client/command/commands/remove_handler.c

    src/client/server_task/server_task_handler.c
    src/client/server_task/tasks/server_task_users.c
    src/client/server_task/tasks/server_task_files.c
    src/client/server_task/tasks/server_task_upload_file.c
    src/client/server_task/tasks/server_task_download_file.c
    src/client/server_task/tasks/server_task_remove_file.c
    
    src/client/state_handlers/state_offline.c
    src/client/state_handlers/state_connect.c
    src/client/state_handlers/state_queue.c
    src/client/state_handlers/state_login.c
    src/client/state_handlers/state_disconnect.c
    src/client/state_handlers/state_chat.c
    src/client/state_handlers/state_quit.c
    
    src/client/ui/ui.c
    src/client/ui/chat_entry_types.c
    src/client/ui/chat_entries.c
    src/client/ui/input_handler.c
    src/client/ui/ui_windows/header_window.c
    src/client/ui/ui_windows/chat_window.c
    src/client/ui/ui_windows/log_window.c
    src/client/ui/ui_windows/input_window.c
)
add_executable(client src/client/client.c ${CLIENT_SOURCES})
target_include_directories(client PRIVATE "${PROJECT_SOURCE_DIR}/include/client")
target_link_libraries(client PRIVATE chat_lib)

# Sets output directory to build/client/
set_target_properties(client PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/client/)

# Copy the resources directory to the build directory
add_custom_command(
    TARGET client POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/src/client/resources $<TARGET_FILE_DIR:client>/resources
)

# Creates test executables ------------------------------------------------------
if (THEY_CHAT_COMPILE_TESTS)
    enable_testing()
    # Set output directory for test executables
    set(TEST_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests)

    # Add common tests
    file(GLOB COMMON_TEST_SOURCES "src/tests/common/*.c" "src/tests/common/file_transferring/*.c")
    foreach(TEST_SOURCE ${COMMON_TEST_SOURCES})
        get_filename_component(TEST_NAME ${TEST_SOURCE} NAME_WE)
        add_executable(${TEST_NAME} ${TEST_SOURCE})
        target_include_directories(${TEST_NAME} PRIVATE "${PROJECT_SOURCE_DIR}/include/common")
        target_link_libraries(${TEST_NAME} PRIVATE chat_lib)
        set_target_properties(${TEST_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${TEST_OUTPUT_DIRECTORY}/common)
        add_test(NAME ${TEST_NAME} COMMAND ${TEST_OUTPUT_DIRECTORY}/common/${TEST_NAME})
    endforeach()

    # Add server tests
    file(GLOB SERVER_TEST_SOURCES "src/tests/server/*.c")
    foreach(TEST_SOURCE ${SERVER_TEST_SOURCES})
        get_filename_component(TEST_NAME ${TEST_SOURCE} NAME_WE)
        add_executable(${TEST_NAME} ${TEST_SOURCE} ${SERVER_SOURCES})
        target_include_directories(${TEST_NAME} PRIVATE "${PROJECT_SOURCE_DIR}/include/server")
        target_link_libraries(${TEST_NAME} PRIVATE chat_lib)
        set_target_properties(${TEST_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${TEST_OUTPUT_DIRECTORY}/server)
        add_test(NAME ${TEST_NAME} COMMAND ${TEST_OUTPUT_DIRECTORY}/server/${TEST_NAME})
    endforeach()

    # Add client tests
    file(GLOB CLIENT_TEST_SOURCES "src/tests/client/*.c")
    foreach(TEST_SOURCE ${CLIENT_TEST_SOURCES})
        get_filename_component(TEST_NAME ${TEST_SOURCE} NAME_WE)
        add_executable(${TEST_NAME} ${TEST_SOURCE} ${CLIENT_SOURCES})
        target_include_directories(${TEST_NAME} PRIVATE "${PROJECT_SOURCE_DIR}/include/client")
        target_link_libraries(${TEST_NAME} PRIVATE chat_lib)
        set_target_properties(${TEST_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${TEST_OUTPUT_DIRECTORY}/client)
        add_test(NAME ${TEST_NAME} COMMAND ${TEST_OUTPUT_DIRECTORY}/client/${TEST_NAME})
    endforeach()

    # Copies common resources folder
    add_custom_command(
        TARGET server POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/src/tests/common/resources ${TEST_OUTPUT_DIRECTORY}/common/resources
    )

endif()
