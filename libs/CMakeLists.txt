# Add the thread_pool library ----------------------------------------
add_subdirectory(thread_pool)

# Create a combined library for all dependencies
add_library(dependencies_lib INTERFACE)

# Link all sub-libraries to the combined library
target_link_libraries(dependencies_lib INTERFACE thread_pool)


# Add Curses library -------------------------------------------------
# Find the ncurses library
find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})

# Link the combined library to the new project executable
target_link_libraries(dependencies_lib INTERFACE ${CURSES_LIBRARIES})

# Add OpenSSL library ------------------------------------------------
if (THEY_CHAT_USE_SSL)
    find_package(OpenSSL REQUIRED)
    include_directories(${OPENSSL_INCLUDE_DIR})
    target_link_libraries(dependencies_lib INTERFACE ${OPENSSL_LIBRARIES})
endif()