project(discrete_rivers)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

cmake_minimum_required(VERSION 2.8)

# Find SDL2
find_package(SDL2 REQUIRED)

if(NOT SDL2_FOUND)
        message(SEND_ERROR "Failed to find SDL2.")
endif()

# Find OpenGL
find_package(OpenGL REQUIRED)

if(NOT OPENGL_FOUND)
        message(SEND_ERROR "Failed to find OpenGL.")
endif()

# Find GLEW
find_package(GLEW REQUIRED)

if(NOT GLEW_FOUND)
        message(SEND_ERROR "Failed to find GLEW.")
endif()

include_directories(
    ${SDL2_INCLUDE_DIRS}
    ${GL_INCLUDE_DIRS}
    ${GLEW_INCLUDE_DIRS}
)

# Create compilation database
set( CMAKE_EXPORT_COMPILE_COMMANDS 1 )

file(GLOB_RECURSE SRCS *.cpp *.h *.c *.hpp)

add_executable(${PROJECT_NAME} ${SRCS})

target_link_libraries(${PROJECT_NAME}
    ${SDL2_LIBRARY}
    ${OPENGL_LIBRARIES}
    ${GLEW_LIBRARIES}
)

# message(STATUS "SDL2_LIBRARY: " ${SDL2_LIBRARY})
# message(STATUS "OPENGL_LIBRARIES: " ${OPENGL_LIBRARIES})
# message(STATUS "GLEW_LIBRARIES: " ${GLEW_LIBRARIES})
