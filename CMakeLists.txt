project(discrete_rivers)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

cmake_minimum_required(VERSION 2.8)

# Find SDL2
find_package(SDL2 REQUIRED)

if(NOT SDL2_FOUND)
        message(SEND_ERROR "Failed to find SDL2.")
endif()

# Find SDL2_image
find_package(SDL2_image REQUIRED)

if(NOT SDL2_IMAGE_FOUND)
        message(SEND_ERROR "Failed to find SDL2_image.")
endif()

# Find Zlib
find_package(ZLIB REQUIRED)

if(NOT ZLIB_FOUND)
        message(SEND_ERROR "Failed to find ZLIB.")
endif()

# Find FreeType
find_package(Freetype REQUIRED)

if(NOT FREETYPE_FOUND)
        message(SEND_ERROR "Failed to find Freetype.")
endif()

# Find libPNG
find_package(PNG REQUIRED)

if(NOT PNG_FOUND)
        message(SEND_ERROR "Failed to find libPNG.")
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
    ${SDL2_IMAGE_INCLUDE_DIR}
    ${GL_INCLUDE_DIRS}
    ${GLEW_INCLUDE_DIRS}
    ${FREETYPE_INCLUDE_DIRS}
)

# Create compilation database
set( CMAKE_EXPORT_COMPILE_COMMANDS 1 )

file(GLOB_RECURSE SRCS src/*.cpp src/*.h src/*.c src/*.hpp)

add_executable(${PROJECT_NAME} ${SRCS})

add_custom_command(TARGET ${PROJECT_NAME} PRE_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                       ${CMAKE_SOURCE_DIR}/res $<TARGET_FILE_DIR:${PROJECT_NAME}>/res)

target_link_libraries(${PROJECT_NAME}
    ${SDL2_LIBRARY}
    ${SDL2_IMAGE_LIBRARY}
    ${ZLIB_LIBRARIES}
    ${PNG_LIBRARIES}
    ${OPENGL_LIBRARIES}
    ${GLEW_LIBRARIES}
    ${FREETYPE_LIBRARIES}
)

# message(STATUS "SDL2_LIBRARY: " ${SDL2_LIBRARY})
# message(STATUS "OPENGL_LIBRARIES: " ${OPENGL_LIBRARIES})
# message(STATUS "GLEW_LIBRARIES: " ${GLEW_LIBRARIES})
# message(STATUS "FREETYPE_INCLUDE_DIRS: " ${FREETYPE_INCLUDE_DIRS})
