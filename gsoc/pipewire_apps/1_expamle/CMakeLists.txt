cmake_minimum_required(VERSION 3.18)
project(cam_viewer C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -fPIC")

find_package(PkgConfig REQUIRED)
pkg_check_modules(PIPEWIRE REQUIRED libpipewire-0.3)
pkg_check_modules(SPA      REQUIRED libspa-0.2)

add_executable(cam_viewer main.c)

target_include_directories(cam_viewer PRIVATE
    ${PIPEWIRE_INCLUDE_DIRS}
    ${SPA_INCLUDE_DIRS}
)

target_link_directories(cam_viewer PRIVATE
    ${PIPEWIRE_LIBRARY_DIRS}
)

target_link_libraries(cam_viewer
    ${PIPEWIRE_LIBRARIES}
)
