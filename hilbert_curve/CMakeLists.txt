project("CMake-hilbert-curve")

cmake_minimum_required(VERSION 3.15)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories($(CMAKE_CURRENT_SOURCE_DIR)/source)

# BEGIN-OPENMP
#find_package(OpenMP)
#
#if (OPENMP_FOUND)
#    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
#    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
#    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
#endif()
#
#add_executable(openmp_example source/openmp_example.cpp)
# END-OPENMP
#
# BEGIN-PTHREAD
#find_package (Threads)
#
#add_executable(pthread_example source/pthread_example.cpp)
#target_link_libraries (pthread_example ${CMAKE_THREAD_LIBS_INIT})
# END-PTHREAD

add_executable(cmake_hilbert_curve source/main.cpp)

