cmake_minimum_required(VERSION 3.14)

project(hw2)

set(CMAKE_CXX_STANDARD_REQUIRED 11)

# specify debug and release flags
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()
set(CMAKE_CXX_FLAGS "-Wall")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O2")

add_executable(sequential sequential.cpp)
add_executable(mpi mpi.cpp)
add_executable(pthread pthread.cpp)

# find and link MPI
find_package(MPI REQUIRED)
target_link_libraries(mpi PRIVATE MPI::MPI_CXX)

# find and link pthread
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads REQUIRED)
target_link_libraries(pthread PRIVATE Threads::Threads)

if(GUI)
    # find and link OpenGL
    find_package(OpenGL REQUIRED)
    find_package(GLUT REQUIRED)

    target_link_libraries(sequential PRIVATE ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES})
    target_link_libraries(mpi PRIVATE ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES})
    target_link_libraries(pthread PRIVATE ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES})
    
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DGUI")
endif()