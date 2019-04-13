cmake_minimum_required (VERSION 3.6)

project(UniqueEngine CXX)


file(GLOB SOURCES "*.cpp")

add_library(UniqueEngine STATIC ${SOURCE})

