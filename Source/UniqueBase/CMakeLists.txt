cmake_minimum_required (VERSION 3.6)

project(UniqueBase CXX)


file(GLOB SOURCES "Core/*.cpp")

add_library(UniqueBase STATIC ${SOURCE})

