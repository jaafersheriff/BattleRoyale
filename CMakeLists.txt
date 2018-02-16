cmake_minimum_required(VERSION 2.7)

# Name of the project
project(BattleRoyale)

# Use glob to get the list of all source files.
file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "src/*.h" "src/*.hpp")
file(GLOB_RECURSE GLSL "resources/*.glsl" "src/*.glsl")

# Set the executable.
add_executable(${CMAKE_PROJECT_NAME} ${SOURCES} ${HEADERS} ${GLSL})

# Visual Studio macro
macro(GroupSources curdir)
    file(GLOB children RELATIVE ${PROJECT_SOURCE_DIR}/${curdir} ${PROJECT_SOURCE_DIR}/${curdir}/*)
    foreach(child ${children})
        if(IS_DIRECTORY ${PROJECT_SOURCE_DIR}/${curdir}/${child})
            GroupSources(${curdir}/${child})
        else()
            string(REPLACE "/" "\\" groupname ${curdir})
            # I would like to call the src root folder in a different name, only in visual studio (not mandatory requirement)
            source_group(${groupname} FILES ${PROJECT_SOURCE_DIR}/${curdir}/${child})
        endif()
    endforeach()
endmacro()
GroupSources(src)

# GLEW
set(GLEW_DIR "$ENV{GLEW_DIR}")
if(NOT GLEW_DIR)
  message(FATAL_ERROR "Please point the environment variable GLEW_DIR to the root directory of your GLEW installation.")
else()
  message(STATUS "GLEW DIR: ${GLEW_DIR}")
endif()
if(WIN32)
  # With prebuilt binaries
  link_directories(${GLEW_DIR}/lib/Release/Win32)
endif()
# Get the GLEW environment variable.
include_directories(${GLEW_DIR}/include)
if(WIN32)
  if(MINGW)
    target_link_libraries(${CMAKE_PROJECT_NAME} C:/MinGW/msys/1.0/lib/glew-2.1.0/lib/glew32.dll)
  else()
    target_link_libraries(${CMAKE_PROJECT_NAME} ${GLEW_DIR}/lib/Release/Win32/glew32s.lib)
  endif()
else()
  target_link_libraries(${CMAKE_PROJECT_NAME} ${GLEW_DIR}/lib/libGLEW.a)
endif()

# GLM
set(GLM_INCLUDE_DIR "$ENV{GLM_INCLUDE_DIR}")
if(NOT GLM_INCLUDE_DIR)
  MESSAGE(FATAL_ERROR "Please point the environment variable GLM_INCLUDE_DIR to the include directory of your GLM installation.")
else()
  message(STATUS "GLM DIR: ${GLM_INCLUDE_DIR}")
endif()
include_directories(${GLM_INCLUDE_DIR})

# GLFW
set(GLFW_DIR "$ENV{GLFW_DIR}")
if(NOT GLFW_DIR)
  message(FATAL_ERROR "Please point the environment variable GLFW_DIR to the root directory of your GLFW3 installation.")
else()
   message(STATUS "GLFW DIR: ${GLFW_DIR}")
endif()
option(GLFW_BUILD_EXAMPLES "GLFW_BUILD_EXAMPLES" OFF)
option(GLFW_BUILD_TESTS "GLFW_BUILD_TESTS" OFF)
option(GLFW_BUILD_DOCS "GLFW_BUILD_DOCS" OFF)
if(CMAKE_BUILD_TYPE MATCHES Release)
  add_subdirectory(${GLFW_DIR} ${GLFW_DIR}/release)
else()
  add_subdirectory(${GLFW_DIR} ${GLFW_DIR}/debug)
endif()
include_directories(${GLFW_DIR}/include)
target_link_libraries(${CMAKE_PROJECT_NAME} glfw ${GLFW_LIBRARIES})

# FMOD
set(FMOD_DIR "${PROJECT_SOURCE_DIR}/libraries/fmod")
if(NOT FMOD_DIR)
  message(STATUS "FMOD_DIR not set. No audio.")
else()
  message(STATUS "FMOD_DIR: ${FMOD_DIR}")
  if(WIN32)
    #with prebuilt binaries
    link_directories(${FMOD_DIR}/api/lowlevel/lib)
    link_directories(${FMOD_DIR}/api/studio/lib)
  endif()
  #Get the FMOD environment variable.
  include_directories(${FMOD_DIR}/api/lowlevel/inc)
  include_directories(${FMOD_DIR}/api/studio/inc)
  if(WIN32)
    if(MINGW)
      target_link_libraries(${CMAKE_PROJECT_NAME} ${FMOD_DIR}/api/lowlevel/lib/fmod.dll)
      target_link_libraries(${CMAKE_PROJECT_NAME} ${FMOD_DIR}/api/studio/lib/fmodstudio.dll)
      message(STATUS "MINGW. Hopefully this doesn't explode right >_<.")
    else()
      target_link_libraries(${CMAKE_PROJECT_NAME} ${FMOD_DIR}/api/lowlevel/lib/fmod_vc.lib)
      target_link_libraries(${CMAKE_PROJECT_NAME} ${FMOD_DIR}/api/studio/lib/fmodstudio_vc.lib)
      #target_link_libraries(${CMAKE_PROJECT_NAME} ${FMOD_DIR}/api/studio/lib/fmodstudio.dll)
      #target_link_libraries(${CMAKE_PROJECT_NAME} ${FMOD_DIR}/api/lowlevel/lib/fmod.dll)
      message(STATUS "Using .lib files")
    endif()
  else()
    target_link_libraries(${CMAKE_PROJECT_NAME} ${FMOD_DIR}/api/studio/lib/fmodstudio.a)
    target_link_libraries(${CMAKE_PROJECT_NAME} ${FMOD_DIR}/api/lowlevel/lib/fmod.a)
  endif()
endif()
# OS specific options and libraries
if(WIN32)
  # c++0x is enabled by default.
  # -Wall produces way too many warnings.
  # -pedantic is not supported.
  target_link_libraries(${CMAKE_PROJECT_NAME} opengl32.lib)
else()
  # Enable all pedantic warnings.
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -Wall -pedantic")
  if(APPLE)
    # Add required frameworks for GLFW.
    target_link_libraries(${CMAKE_PROJECT_NAME} "-framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo")
  else()
    #Link the Linux OpenGL library
    target_link_libraries(${CMAKE_PROJECT_NAME} "GL")
  endif()
endif()

include_directories(${PROJECT_SOURCE_DIR}/src)
include_directories(${PROJECT_SOURCE_DIR}/src/Engine)

set (CMAKE_CXX_STANDARD 11)