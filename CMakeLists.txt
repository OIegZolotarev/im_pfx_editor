cmake_minimum_required(VERSION 3.14) # Specifies the minimum required CMake version
project(ImPFXEditor LANGUAGES C CXX)    # Defines the project name and enables C++ support

find_package(SDL2 REQUIRED)
find_package(OpenGL REQUIRED)


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)

FetchContent_Declare(
     glm
     GIT_REPOSITORY https://github.com/g-truc/glm.git
)

 FetchContent_MakeAvailable(glm)

# ImGUI setup

FetchContent_Declare(imgui_external
	URL https://github.com/ocornut/imgui/archive/refs/tags/v1.92.2b-docking.tar.gz
	EXCLUDE_FROM_ALL
)
FetchContent_MakeAvailable(imgui_external)

add_library(imgui
	${imgui_external_SOURCE_DIR}/imgui.cpp
	${imgui_external_SOURCE_DIR}/imgui_draw.cpp
	${imgui_external_SOURCE_DIR}/imgui_tables.cpp
	${imgui_external_SOURCE_DIR}/imgui_widgets.cpp
	${imgui_external_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp	
	${imgui_external_SOURCE_DIR}/backends/imgui_impl_sdl2.cpp	
	${imgui_external_SOURCE_DIR}/imgui_draw.cpp
	${imgui_external_SOURCE_DIR}/imgui_tables.cpp
	${imgui_external_SOURCE_DIR}/imgui_widgets.cpp
	${imgui_external_SOURCE_DIR}/imgui_demo.cpp

)


# ImGUI setup end


file(GLOB_RECURSE sources
     "src/*.h"
     "src/*.cpp"
     "src/glad/glad.c"     
)

include_directories("src/glad")
include_directories("src/")
include_directories(${imgui_external_SOURCE_DIR})

include_directories(${SDL2_INCLUDE_DIR})

# Add an executable target named 'ImPFXEditor' from the source file 'main.cpp'
add_executable(ImPFXEditor ${sources})

target_link_libraries(ImPFXEditor PUBLIC imgui)
target_link_libraries(ImPFXEditor PRIVATE SDL2::SDL2)
target_link_libraries(ImPFXEditor PRIVATE glm::glm) 

set_target_properties( ImPFXEditor
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/../bin"
)