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

# Ensure we are linking static libraries
if (THEY_CHAT_STATIC_LINK)
    set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
    set(BUILD_SHARED_LIBS OFF)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static")
    message("Compiling dependendies as static libraries")

else()
    message("Compiling dependendies as shared libraries")
endif()

# Finds libraries -------------------------------------------------------------------
# Find the ncurses library (static)
find_library(NCURSES_LIBRARY NAMES ncurses)
if(NOT NCURSES_LIBRARY)
  message(FATAL_ERROR "ncurses library not found")
endif()

# Find the tinfo library (static)
find_library(TINFO_LIBRARY NAMES tinfo)
if(NOT TINFO_LIBRARY)
  message(FATAL_ERROR "tinfo library not found")
endif()

# Finds headers ---------------------------------------------------------------------
# Find the ncurses headers
find_path(NCURSES_INCLUDE_DIR NAMES ncurses.h)
if(NOT NCURSES_INCLUDE_DIR)
  message(FATAL_ERROR "headers not found")
endif()

# Include ncurses headers
include_directories(${NCURSES_INCLUDE_DIR})

# Find the pthread
find_library(PTHREAD_LIBRARY NAMES pthread)
if(NOT PTHREAD_LIBRARY)
  message(FATAL_ERROR "ptherad library not found")
endif()


# Print out the libraries found
message(STATUS "--->NCURSES_LIBRARY: ${NCURSES_LIBRARY}")
message(STATUS "--->TINFO_LIBRARY: ${TINFO_LIBRARY}")
message(STATUS "--->PTHREAD_LIBRARY: ${PTHREAD_LIBRARY}")
message(STATUS "--->NCURSES_INCLUDE_DIR: ${NCURSES_INCLUDE_DIR}")

# Link the static ncurses library and its dependencies
target_link_libraries(
        dependencies_lib 
        INTERFACE 
        ${NCURSES_LIBRARY} ${TINFO_LIBRARY} ${PTHREAD_LIBRARY})

# Add OpenSSL library ------------------------------------------------
if (USING_SSL)
    # Find the ssl
    find_library(SSL_LIBRARY NAMES ssl)
    if(NOT SSL_LIBRARY)
        message(FATAL_ERROR "ssl library not found")
    else()
        message(STATUS "--->SSL_LIBRARY: ${SSL_LIBRARY}")
    endif()

    # Find crypyo
    find_library(CRYPTO_LIBRARY NAMES crypto)
    if(NOT CRYPTO_LIBRARY)
        message(FATAL_ERROR "crypto library not found")
    else()
        message(STATUS "--->CRYPTO_LIBRARY: ${CRYPTO_LIBRARY}")
    endif()

    # Links both
    target_link_libraries(dependencies_lib INTERFACE ${SSL_LIBRARY})
    target_link_libraries(dependencies_lib INTERFACE ${CRYPTO_LIBRARY})

endif()


# Windows socket libraries ------------------------------------------------
if (WIN32)
    target_link_libraries(dependencies_lib INTERFACE wsock32 ws2_32)
endif()
