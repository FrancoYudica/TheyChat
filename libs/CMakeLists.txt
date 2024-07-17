# Add the thread_pool library ----------------------------------------
add_subdirectory(thread_pool)

# Create a combined library for all dependencies
add_library(dependencies_lib INTERFACE)

# Link all sub-libraries to the combined library
target_link_libraries(dependencies_lib INTERFACE thread_pool)


# Add Curses library -------------------------------------------------
# Find the ncurses library
# Manually specify the paths for ncurses library and include files
if (WIN32)
    set(CURSES_INCLUDE_PATH "C:\\msys64\\mingw64\\include\\ncursesw")
    set(CURSES_LIBRARY "C:\\msys64\\mingw64\\lib\\libncursesw.dll.a")
endif()

find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})

# Link the curses library
target_link_libraries(dependencies_lib INTERFACE ${CURSES_LIBRARY})

# Add OpenSSL library ------------------------------------------------
if (USING_SSL)
    find_package(OpenSSL REQUIRED)
    include_directories(${OPENSSL_INCLUDE_DIR})
    target_link_libraries(dependencies_lib INTERFACE ${OPENSSL_LIBRARIES})
endif()


# Windows socket libraries ------------------------------------------------
if (WIN32)
    target_link_libraries(dependencies_lib INTERFACE wsock32 ws2_32)
endif()