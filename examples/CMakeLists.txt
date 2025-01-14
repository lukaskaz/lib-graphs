cmake_minimum_required(VERSION 3.10)

option(ADD_EXAMPLES "Creates example basic" OFF)

if(ADD_EXAMPLES)
    add_subdirectory(dygraph)
endif()
