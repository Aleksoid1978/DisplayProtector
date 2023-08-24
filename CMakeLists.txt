cmake_minimum_required(VERSION 3.7)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

project(DisplayProtector)

set(CMAKE_CONFIGURATION_TYPES
    "Debug"
    "Release"
    CACHE STRING "" FORCE
)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING
        "Choose the type of build, options are: Debug Release."
        FORCE)
endif()

add_subdirectory(dependencies)

add_executable(${PROJECT_NAME} ${PROJECT_SOURCE_DIR}/src/main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE ${SDL_LIBRARIES})

if(MSVC)
    target_compile_options(${PROJECT_NAME} PUBLIC
        "$<$<CONFIG:Debug>:/MTd>"
        "$<$<CONFIG:Release>:/MT>"
    )

    set_property(TARGET ${PROJECT_NAME} PROPERTY VS_DPI_AWARE "PerMonitor")
endif()

set_property(TARGET SDL2_image       PROPERTY FOLDER "External")
set_property(TARGET SDL2-static      PROPERTY FOLDER "External")
set_property(TARGET sdl_headers_copy PROPERTY FOLDER "External")
set_property(TARGET uninstall        PROPERTY FOLDER "External")

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})