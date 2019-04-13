cmake_minimum_required (VERSION 3.6)

project(UniqueEngine CXX)


aux_source_directory(. DIR_SRCS)

add_library(UniqueEngine STATIC ${DIR_SRCS})

