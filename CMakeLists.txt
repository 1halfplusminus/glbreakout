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
add_subdirectory (SFML)
link_directories(./SFML/extlibs/libs-msvc/x64)
file(GLOB MY_HEADERS ./SFML/extlibs/bin/x64/*.dll)

# set variables that are needed
set(ZLIB_LIBRARIES zlibstatic)
set(ENABLE_BOOST_WORKAROUND ON)
set(BUILD_ASSIMP_TOOLS  ON)
set(ASSIMP_BUILD_STATIC_LIB ON)
set(ASSIMP_BUILD_DRACO OFF)
set(BUILD_STATIC_LIB ON)
add_subdirectory (assimp)

set(ALL_LIBS
	${OPENGL_LIBRARY}
	glfw
	glm
	assimp
	freetype
	EnTT
	sfml-system
	sfml-audio
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
./src/graphic/system_postprocessing.hpp
./src/graphic/system_opengl_postprocessing.cpp
./src/graphic/system_graphic_pipeline.hpp
./src/graphic/system_graphic_pipeline.cpp
./src/graphic/helper_graphic_pipeline.hpp
./src/graphic/helper_graphic_pipeline.cpp
./src/graphic/system_particule.hpp
./src/graphic/system_particule.cpp
./src/graphic/my_opengl.hpp
./src/audio/audio_sound.hpp
./src/audio/audio_sound.cpp
./src/audio/component_audio.hpp
./src/audio/system_audio.hpp
./src/audio/system_audio.cpp
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
target_include_directories(libs PRIVATE ./stb ${ALL_LIBS} )


macro(MY_ADD_EXECUTABLE execName source)

add_executable(${execName} ${source} ${HEADER_FILES} )

target_include_directories(${execName}  
	PRIVATE
	"${CMAKE_SOURCE_DIR}"
	"${CMAKE_SOURCE_DIR}/src"
	"${CMAKE_SOURCE_DIR}/src/graphic"
	"${CMAKE_SOURCE_DIR}/src/gameplay"
	"${CMAKE_SOURCE_DIR}/src/physic"
	"${CMAKE_SOURCE_DIR}/src/audio"
)
target_link_libraries(${execName} libs ${ALL_LIBS})

endmacro()


MY_ADD_EXECUTABLE(main main.cpp)

foreach(CurrentHeaderFile IN LISTS MY_HEADERS)
	add_custom_command(TARGET main POST_BUILD        # Adds a post-build event to MyTest
	COMMAND ${CMAKE_COMMAND} -E copy_if_different  # which executes "cmake - E copy_if_different..."
		${CurrentHeaderFile}     # <--this is in-file
		$<TARGET_FILE_DIR:main>)                 # <--this is out-file path
endforeach()


file(COPY "./shader" DESTINATION  "./Debug")
file(COPY "./texture" DESTINATION  "./Debug")
file(COPY "./fonts" DESTINATION  "./Debug")
file(COPY "./assets" DESTINATION  "./Debug")

file(COPY "./shader" DESTINATION  "./Release")
file(COPY "./texture" DESTINATION  "./Release")
file(COPY "./fonts" DESTINATION  "./Release")
file(COPY "./assets" DESTINATION  "./Release")

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)


