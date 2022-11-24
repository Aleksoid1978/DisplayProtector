include(FetchContent)

option(LINK_DEPS_STATIC CACHE ON)

if(LINK_DEPS_STATIC)
  set(BUILD_SHARED_LIBS OFF)
else()
  set(BUILD_SHARED_LIBS ON)
endif()

#======================================
FetchContent_Declare(
  SDL
  GIT_REPOSITORY "https://github.com/libsdl-org/SDL"
  GIT_TAG        "release-2.26.x"
)
#======================================

# SDL
if (LINK_DEPS_STATIC)
  set(SDL_STATIC ON CACHE BOOL "Build SDL2 as shared lib")
  set(SDL_FORCE_STATIC_VCRT ON CACHE BOOL "Build SDL2 as shared lib")
else()
  set(SDL_SHARED ON CACHE BOOL "Build SDL2 as shared lib")
endif()

FetchContent_MakeAvailable(SDL)

# Define alias for what we used
if (LINK_DEPS_STATIC)
  add_library(SDL2::SDL2 ALIAS SDL2-static)
else()
  add_library(SDL2::SDL2 ALIAS SDL2)
endif()
