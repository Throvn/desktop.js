# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/Users/throvn/Documents/desktop.js/build/_deps/sdl_ttf-src"
  "/Users/throvn/Documents/desktop.js/build/_deps/sdl_ttf-build"
  "/Users/throvn/Documents/desktop.js/build/_deps/sdl_ttf-subbuild/sdl_ttf-populate-prefix"
  "/Users/throvn/Documents/desktop.js/build/_deps/sdl_ttf-subbuild/sdl_ttf-populate-prefix/tmp"
  "/Users/throvn/Documents/desktop.js/build/_deps/sdl_ttf-subbuild/sdl_ttf-populate-prefix/src/sdl_ttf-populate-stamp"
  "/Users/throvn/Documents/desktop.js/build/_deps/sdl_ttf-subbuild/sdl_ttf-populate-prefix/src"
  "/Users/throvn/Documents/desktop.js/build/_deps/sdl_ttf-subbuild/sdl_ttf-populate-prefix/src/sdl_ttf-populate-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/Users/throvn/Documents/desktop.js/build/_deps/sdl_ttf-subbuild/sdl_ttf-populate-prefix/src/sdl_ttf-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/Users/throvn/Documents/desktop.js/build/_deps/sdl_ttf-subbuild/sdl_ttf-populate-prefix/src/sdl_ttf-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
