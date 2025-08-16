# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/Users/throvn/Documents/desktop.js/build/_deps/fontstash-src"
  "/Users/throvn/Documents/desktop.js/build/_deps/fontstash-build"
  "/Users/throvn/Documents/desktop.js/build/_deps/fontstash-subbuild/fontstash-populate-prefix"
  "/Users/throvn/Documents/desktop.js/build/_deps/fontstash-subbuild/fontstash-populate-prefix/tmp"
  "/Users/throvn/Documents/desktop.js/build/_deps/fontstash-subbuild/fontstash-populate-prefix/src/fontstash-populate-stamp"
  "/Users/throvn/Documents/desktop.js/build/_deps/fontstash-subbuild/fontstash-populate-prefix/src"
  "/Users/throvn/Documents/desktop.js/build/_deps/fontstash-subbuild/fontstash-populate-prefix/src/fontstash-populate-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/Users/throvn/Documents/desktop.js/build/_deps/fontstash-subbuild/fontstash-populate-prefix/src/fontstash-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/Users/throvn/Documents/desktop.js/build/_deps/fontstash-subbuild/fontstash-populate-prefix/src/fontstash-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
