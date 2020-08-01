cmake_minimum_required (VERSION 3.10)
project (droid_arena C)

set (CMAKE_CXX_STANDARD 17)
set (CMAKE_C_STANDARD 11)

find_package (PkgConfig REQUIRED)

pkg_check_modules (SDL2 REQUIRED sdl2)
pkg_check_modules (SDL2GFX REQUIRED sdl2_gfx)

add_executable (droid_arena
  main.c
  droid.c
  game.c
  env.c
)
target_include_directories (droid_arena PRIVATE ${SDL2_INCLUDE_DIRS})
target_link_libraries (droid_arena ${SDL2_LINK_LIBRARIES} ${SDL2GFX_LINK_LIBRARIES})
