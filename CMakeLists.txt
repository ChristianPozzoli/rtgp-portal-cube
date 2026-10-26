cmake_minimum_required(VERSION 3.20)
project(MyApp C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(APP_DIR ${CMAKE_SOURCE_DIR}/src)   # cartella con main.cpp

# Come /MT del Makefile (in Debug /MTd). Va PRIMA di FetchContent.
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

include(FetchContent)

# --- GLFW ---
set(GLFW_BUILD_DOCS     OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS    OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL        OFF CACHE BOOL "" FORCE)
set(USE_MSVC_RUNTIME_LIBRARY_DLL OFF CACHE BOOL "" FORCE)
FetchContent_Declare(glfw
  GIT_REPOSITORY https://github.com/glfw/glfw.git GIT_TAG 3.4 GIT_SHALLOW TRUE)

# --- GLM (header-only) ---
FetchContent_Declare(glm
  GIT_REPOSITORY https://github.com/g-truc/glm.git GIT_TAG 1.0.1 GIT_SHALLOW TRUE)

# --- Assimp (include già zlib, minizip, draco, pugixml, poly2tri, ecc.) ---
set(ASSIMP_BUILD_TESTS        OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "" FORCE)
set(ASSIMP_INSTALL            OFF CACHE BOOL "" FORCE)
set(ASSIMP_WARNINGS_AS_ERRORS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ZLIB         ON  CACHE BOOL "" FORCE)
set(USE_STATIC_CRT            ON  CACHE BOOL "" FORCE)
# Opzionale: solo i formati che usi, build molto più veloce
# set(ASSIMP_NO_EXPORT ON CACHE BOOL "" FORCE)
# set(ASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT OFF CACHE BOOL "" FORCE)
# set(ASSIMP_BUILD_OBJ_IMPORTER ON CACHE BOOL "" FORCE)
# set(ASSIMP_BUILD_GLTF_IMPORTER ON CACHE BOOL "" FORCE)
FetchContent_Declare(assimp
  GIT_REPOSITORY https://github.com/assimp/assimp.git GIT_TAG v5.4.3 GIT_SHALLOW TRUE)

FetchContent_MakeAvailable(glfw glm assimp)

# --- Applicazione (stessi sorgenti del Makefile) ---
file(GLOB IMGUI_SRC  CONFIGURE_DEPENDS include/imgui/*.cpp)
file(GLOB UTILS_SRC  CONFIGURE_DEPENDS include/utils/*.cpp)

add_executable(main
  ${APP_DIR}/main.cpp
  include/glad/glad.c
  ${IMGUI_SRC}
  ${UTILS_SRC})

target_include_directories(main PRIVATE include)
target_link_libraries(main PRIVATE glfw glm::glm assimp ${CMAKE_DL_LIBS})

# main.exe / main finisce accanto a main.cpp come prima (percorsi relativi ad asset/shader invariati)
set_target_properties(main PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY "$<1:${APP_DIR}>"
  VS_DEBUGGER_WORKING_DIRECTORY "${APP_DIR}")