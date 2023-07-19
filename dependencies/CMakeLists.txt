include(FetchContent)

FetchContent_Declare(
    SDL2
    GIT_REPOSITORY  "https://github.com/libsdl-org/SDL"
    GIT_TAG         "release-2.28.1"
)

FetchContent_Declare(
    SDL2_IMAGE
    GIT_REPOSITORY  "https://github.com/libsdl-org/SDL_image"
    GIT_TAG         "release-2.6.3"
)

set(BUILD_SHARED_LIBS OFF)
set(SDL_STATIC ON CACHE BOOL "Build SDL2 as static lib")
set(SDL_FORCE_STATIC_VCRT ON CACHE BOOL "Build SDL2 as static lib")
set(SDL_TEST OFF CACHE BOOL "Disable building the SDL2_test library")
set(SDL2_DISABLE_SDL2MAIN ON CACHE BOOL "Disable building/installation of SDL2main")

set(SDL2IMAGE_SAMPLES OFF CACHE BOOL "Disable building the SDL2_image sample program(s)")
set(SDL2IMAGE_INSTALL OFF CACHE BOOL "Disable SDL2_image install target")

FetchContent_MakeAvailable(SDL2 SDL2_IMAGE)

set(SDL_LIBRARIES SDL2-static SDL2_image PARENT_SCOPE)

if(MSVC)
    target_compile_options(SDL2_image PRIVATE
        "$<$<CONFIG:Debug>:/MTd>"
        "$<$<CONFIG:Release>:/MT>"
    )
endif()

