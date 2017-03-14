project(discreterivers)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

cmake_minimum_required(VERSION 2.8)

# Find SDL2
find_package(SDL2 REQUIRED)
if(NOT SDL2_FOUND)
        message(SEND_ERROR "Failed to find SDL2.")
endif()

find_package(FREETYPE REQUIRED)
if(NOT FREETYPE_FOUND)
        message(SEND_ERROR "Failed to find FREETYPE.")
endif()

# Find OpenGL
find_package(OpenGL REQUIRED)
if(NOT OPENGL_FOUND)
        message(SEND_ERROR "Failed to find OpenGL.")
endif()

find_package(GLEW REQUIRED)
if(NOT GLEW_FOUND)
	message(SEND_ERROR "Failed to find GLEW.")
endif()

if(WIN32)
	# add_definitions(-DGLEW_STATIC)
endif()

find_package(ASSIMP REQUIRED)
if(NOT ASSIMP_FOUND)
        message(SEND_ERROR "Failed to find Assimp.")
endif()

find_package(BULLET REQUIRED)
if(NOT BULLET_FOUND)
        message(SEND_ERROR "Failed to find Bullet.")
endif()

include_directories(
    ${SDL2_INCLUDE_DIR}
    ${GL_INCLUDE_DIRS}
    ${GLEW_INCLUDE_DIRS}
    ${FREETYPE_INCLUDE_DIRS}
)

# Create compilation database (for code completion tools)
set( CMAKE_EXPORT_COMPILE_COMMANDS 1 )

file(GLOB_RECURSE SRCS src/*.cpp src/*.h src/*.c src/*.hpp)

add_executable(${PROJECT_NAME} ${SRCS})

target_link_libraries(${PROJECT_NAME}
    ${SDL2MAIN_LIBRARY} ${SDL2_LIBRARY}
    ${OPENGL_LIBRARIES}
    ${GLEW_LIBRARIES}
    ${FREETYPE_LIBRARIES}
    # assimp
    # BulletDynamics BulletCollision LinearMath
)

#copy resource files into build directory
add_custom_command(TARGET ${PROJECT_NAME} PRE_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                   ${CMAKE_SOURCE_DIR}/res $<TARGET_FILE_DIR:${PROJECT_NAME}>/res)

message(STATUS "SDL2_INCLUDE_DIR: " ${SDL2_INCLUDE_DIR})
message(STATUS "SDL2_LIBRARY: " ${SDL2_LIBRARY})
message(STATUS "SDL2_LIBRARIES: " ${SDL2_LIBRARIES})
message(STATUS "SDL2MAIN_LIBRARY:" ${SDL2MAIN_LIBRARY})
# message(STATUS "OPENGL_LIBRARIES: " ${OPENGL_LIBRARIES})
message(STATUS "GLEW_LIBRARIES: " ${GLEW_LIBRARIES})
message(STATUS "GLEW_INCLUDE_DIRS}: " ${GLEW_INCLUDE_DIRS})
message(STATUS "MINGW32_LIBRARIES}: " ${MINGW32_LIBRARIES})
# message(STATUS "FREETYPE_INCLUDE_DIRS: " ${FREETYPE_INCLUDE_DIRS})
# message(STATUS "FREETYPE_LIBRARIES: " ${FREETYPE_LIBRARIES})
