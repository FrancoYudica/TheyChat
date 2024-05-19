# Add the thread_pool library
add_subdirectory(thread_pool)

# Create a combined library for all dependencies
add_library(dependencies_lib INTERFACE)

# Link all sub-libraries to the combined library
target_link_libraries(dependencies_lib INTERFACE thread_pool)