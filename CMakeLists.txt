cmake_minimum_required(VERSION 3.10)
project(test VERSION 0.0.1 LANGUAGES CXX)

# Link this 'library' to set the c++ standard / compile-time options requested
add_library(project_options INTERFACE)
add_library(project_warnings INTERFACE)

set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=thread")
set (CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_LINKER_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=thread")

# set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")
# set (CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_LINKER_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

target_compile_features(project_options INTERFACE cxx_std_17 )
target_include_directories(project_options INTERFACE include)
target_compile_options(project_warnings
INTERFACE -Wall
          -Werror # treat all warnings as errors
          -Wextra # reasonable and standard
          -Wshadow # warn the user if a variable declaration shadows one
                   # from a parent context
          -Wnon-virtual-dtor # warn the user if a class with virtual
                             # functions has a non-virtual destructor. This
                             # helps catch hard to track down memory errors
          -Wold-style-cast # warn for c-style casts
          -Wcast-align # warn for potential performance problem casts
          -Wunused # warn on anything being unused
          -Woverloaded-virtual # warn if you overload (not override) a
                               # virtual function
          -Wpedantic # warn if non-standard C++ is used
          -Wconversion # warn on type conversions that may lose data
          -Wsign-conversion # warn on sign conversions
          -Wnull-dereference # warn if a null dereference is detected
          -Wdouble-promotion # warn if float is implicit promoted to double
          -Wformat=2 # warn on security issues around functions that format
                     # output (ie printf)
)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
add_executable(a main.cpp)
target_link_libraries(a
                        PRIVATE project_options
                        Threads::Threads)

add_executable(test 
                tests/test-main.cpp 
                tests/test-LimitedDataQueue.cpp)
target_link_libraries(test project_options)
