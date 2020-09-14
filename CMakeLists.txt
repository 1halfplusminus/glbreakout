cmake_minimum_required(VERSION 3.7)
project(glbreakout VERSION 0.1.0)

include(CTest)
enable_testing()

INCLUDE_DIRECTORIES(./glfw/include)
INCLUDE_DIRECTORIES(./glad/include)
INCLUDE_DIRECTORIES(./stb)


add_subdirectory (glfw)
add_subdirectory (glm)
add_subdirectory (freetype2)
add_subdirectory (entt)

# set variables that are needed
set(ZLIB_LIBRARIES zlibstatic)
set(ENABLE_BOOST_WORKAROUND ON)
set(BUILD_STATIC_LIB ON)
set(BUILD_ASSIMP_TOOLS  ON)
set(ASSIMP_BUILD_STATIC_LIB ON)

add_subdirectory (assimp)

set(ALL_LIBS
	${OPENGL_LIBRARY}
	glfw
	glm
	assimp
	freetype
	EnTT
)

set(HEADER_FILES 
./stb/stb_image.h 
./constants.h 
./ecs.h 
./game.h 
./game.cpp
./src/graphic/graphic_image.hpp 
./src/graphic/graphic_image.cpp 
./src/graphic/graphic_texture.hpp 
./src/graphic/graphic_texture.cpp 
./src/graphic/component_graphic_pipeline.hpp 
./src/graphic/component_render.hpp 
./src/graphic/system_graphic_pipeline.hpp
./src/graphic/system_graphic_pipeline.cpp
./src/graphic/helper_graphic_pipeline.hpp
./src/graphic/helper_graphic_pipeline.cpp
./src/physic/component_physic.hpp
./src/physic/helper_physic.hpp
./src/physic/helper_physic.cpp
./src/physic/system_physic.hpp
./src/physic/system_physic.cpp
./src/gameplay/component_gameplay.hpp 
./src/gameplay/helper_gameplay.hpp 
./src/gameplay/helper_gameplay.cpp 
)

add_library(libs STATIC ./glad/src/glad.c ./ecs.h  )
target_include_directories(libs PRIVATE ./stb ${ALL_LIBS})


add_executable(glbreakout main.cpp ${HEADER_FILES} )
target_include_directories(glbreakout  
	PRIVATE
	"${CMAKE_SOURCE_DIR}"
	"${CMAKE_SOURCE_DIR}/src"
	"${CMAKE_SOURCE_DIR}/src/graphic"
	"${CMAKE_SOURCE_DIR}/src/gameplay"
	"${CMAKE_SOURCE_DIR}/src/physic"
)
target_link_libraries(glbreakout libs ${ALL_LIBS})

file(COPY "./shader" DESTINATION  "./Debug")
file(COPY "./texture" DESTINATION  "./Debug")
file(COPY "./fonts" DESTINATION  "./Debug")
file(COPY "./assets" DESTINATION  "./Debug")

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
