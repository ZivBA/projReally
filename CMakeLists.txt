cmake_minimum_required(VERSION 3.5)
project(structBioProj)

include_directories( /cs/bio3d/dina/libs/gamb++/
                    /cs/bio3d/dina/libs/DockingLib
                    /cs/bio3d/dina/libs/leda/
                    /cs/bio3d/dina/libs/leda/incl/
             )
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lz -std=c++11 ")
set (ALL_H_FILES


        )
set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")

set(GCC_COVERAGE_LINK_FLAGS  -L/cs/bio3d/dina/libs/gamb++/ -L/cs/bio3d/dina/libs/DockingLib -L/cs/bio3d/dina/libs/leda/ -ldockingLib -lgamb++ -lleda  -lstdc++ -lm  )

set( CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} ${GCC_COVERAGE_LINK_FLAGS}" )

set(SOURCE_FILES main.cpp )
add_executable(structBioProj ${SOURCE_FILES} )


