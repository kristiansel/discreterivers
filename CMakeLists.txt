project(discreterivers)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

cmake_minimum_required(VERSION 2.8)

set(HAVE_LIBUDEV_H 0)
add_subdirectory(dep/sdl2)

if(WIN32)
	set(MINGW32_LIBRARIES mingw32)
endif()

add_subdirectory(dep/freetype)

# Find OpenGL
find_package(OpenGL REQUIRED)
if(NOT OPENGL_FOUND)
        message(SEND_ERROR "Failed to find OpenGL.")
endif()

# Find GLEW
if(WIN32)
	set(CMAKE_PREFIX_PATH "C:/Users/krist/prog/libraries/MinGW-x64/glew-2.0.0/include")
	set(CMAKE_LIBRARY_PATH "C:/Users/krist/prog/libraries/MinGW-x64/glew-2.0.0/bin/Release/x64")
endif()


find_package(GLEW REQUIRED)
if(NOT GLEW_FOUND)
	message(SEND_ERROR "Failed to find GLEW.")
endif()

if(WIN32)
	# add_definitions(-DGLEW_STATIC)
endif()

option(ASSIMP_BUILD_ASSIMP_TOOLS OFF)
option(ASSIMP_BUILD_SAMPLES OFF)
option(ASSIMP_BUILD_TESTS OFF)
add_subdirectory(dep/assimp)

option(BUILD_BULLET2_DEMOS OFF)
option(BUILD_CPU_DEMOS OFF)
option(BUILD_EXTRAS OFF)
option(BUILD_OPENGL3_DEMOS OFF)
option(BUILD_UNIT_TESTS OFF)
add_subdirectory(dep/bullet)

include_directories(
    dep/sdl2/include
    ${GL_INCLUDE_DIRS}
    ${GLEW_INCLUDE_DIRS}
    dep/freetype/include
)

# Create compilation database (for code completion tools)
set( CMAKE_EXPORT_COMPILE_COMMANDS 1 )

file(GLOB_RECURSE SRCS src/*.cpp src/*.h src/*.c src/*.hpp)

add_executable(${PROJECT_NAME} ${SRCS})

target_link_libraries(${PROJECT_NAME}
	${MINGW32_LIBRARIES}
    SDL2main SDL2
    ${OPENGL_LIBRARIES}
    ${GLEW_LIBRARIES}
    freetype
    # assimp
    # BulletDynamics BulletCollision LinearMath
)

#copy resource files into build directory
add_custom_command(TARGET ${PROJECT_NAME} PRE_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                   ${CMAKE_SOURCE_DIR}/res $<TARGET_FILE_DIR:${PROJECT_NAME}>/res)

# message(STATUS "SDL2_LIBRARY: " ${SDL2_LIBRARY})
# message(STATUS "OPENGL_LIBRARIES: " ${OPENGL_LIBRARIES})
message(STATUS "GLEW_LIBRARIES: " ${GLEW_LIBRARIES})
message(STATUS "GLEW_INCLUDE_DIRS}: " ${GLEW_INCLUDE_DIRS})
message(STATUS "MINGW32_LIBRARIES}: " ${MINGW32_LIBRARIES})
# message(STATUS "FREETYPE_INCLUDE_DIRS: " ${FREETYPE_INCLUDE_DIRS})
# message(STATUS "FREETYPE_LIBRARIES: " ${FREETYPE_LIBRARIES})
