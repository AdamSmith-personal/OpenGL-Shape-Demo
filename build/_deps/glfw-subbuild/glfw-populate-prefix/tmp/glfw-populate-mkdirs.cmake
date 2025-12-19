# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/home/adamsmith/dev/EOUClasses/LinearAlgebra/Final Project/build/_deps/glfw-src"
  "/home/adamsmith/dev/EOUClasses/LinearAlgebra/Final Project/build/_deps/glfw-build"
  "/home/adamsmith/dev/EOUClasses/LinearAlgebra/Final Project/build/_deps/glfw-subbuild/glfw-populate-prefix"
  "/home/adamsmith/dev/EOUClasses/LinearAlgebra/Final Project/build/_deps/glfw-subbuild/glfw-populate-prefix/tmp"
  "/home/adamsmith/dev/EOUClasses/LinearAlgebra/Final Project/build/_deps/glfw-subbuild/glfw-populate-prefix/src/glfw-populate-stamp"
  "/home/adamsmith/dev/EOUClasses/LinearAlgebra/Final Project/build/_deps/glfw-subbuild/glfw-populate-prefix/src"
  "/home/adamsmith/dev/EOUClasses/LinearAlgebra/Final Project/build/_deps/glfw-subbuild/glfw-populate-prefix/src/glfw-populate-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/adamsmith/dev/EOUClasses/LinearAlgebra/Final Project/build/_deps/glfw-subbuild/glfw-populate-prefix/src/glfw-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/adamsmith/dev/EOUClasses/LinearAlgebra/Final Project/build/_deps/glfw-subbuild/glfw-populate-prefix/src/glfw-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
