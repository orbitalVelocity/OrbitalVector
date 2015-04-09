set(project_name orbitalvector)

cmake_minimum_required (VERSION 2.8)

# setting the compiler before declaring the project prevents an infinite loop
set (CMAKE_CXX_COMPILER "/usr/bin/clang++")

project (${project_name})

set (CMAKE_CXX_FLAGS    "${CMAKE_CXX_FLAGS} -std=c++11 -stdlib=libc++ -g")

# this function adds independent modules to the project
function(add_module name)
	add_subdirectory(${name})
	include_directories(${name}/include)
	target_link_libraries(${project_name} ${name})
endfunction(add_module)

find_package(OpenGL	REQUIRED)
find_package(GLEW	REQUIRED)
find_package(PkgConfig	REQUIRED)
pkg_search_module(GLFW	REQUIRED glfw3)

# TODO: add sub-folders elegantly
include_directories(src)
include_directories(src/ECS)
include_directories(src/GameObject)
include_directories(src/Rendering)
include_directories(src/Shaders)
include_directories(src/Systems)

# because of file globbing, cmake must be rerun when files are added or removed
file(GLOB src_files
	"src/*.cpp"
	"src/ECS/*.cpp"
	"src/GameObject/*.cpp"
	"src/Rendering/*.cpp"
	"src/Shaders/*.cpp"
	"src/Systems/*.cpp"
	"fontstash/*.cpp"
	"rendertest/*.cpp"
)

add_executable(${project_name} ${src_files})
target_link_libraries(${project_name} ${OPENGL_LIBRARIES} GL GLU X11)
target_link_libraries(${project_name} ${GLEW_LIBRARIES})
target_link_libraries(${project_name} glfw3 ${GLFW_LIBRARIES})
# linker errors say we need these
target_link_libraries(${project_name} Xxf86vm pthread Xrandr Xi)

add_module(physics)
add_module(orbitutils)

include_directories(glm/glm)

add_subdirectory(entityx)
include_directories(entityx)
target_link_libraries(${project_name} entityx)
