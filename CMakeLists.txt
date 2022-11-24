cmake_minimum_required(VERSION 3.7)

project(DisplayProtector LANGUAGES CXX C)

add_subdirectory(dependencies)

add_executable(${PROJECT_NAME} ${PROJECT_SOURCE_DIR}/src/main.cpp)

target_link_libraries(${PROJECT_NAME} PUBLIC SDL2::SDL2)
if(WIN32)
    target_link_libraries(${PROJECT_NAME} PUBLIC SDL2main)
endif()

if(MSVC)
    target_compile_options(${PROJECT_NAME} PUBLIC
        "$<$<CONFIG:Debug>:/MTd>"
        "$<$<CONFIG:Release>:/MT>"
    )
endif()
