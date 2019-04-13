cmake_minimum_required (VERSION 3.6)

project(UniqueSample CXX)


aux_source_directory(. DIR_SRCS)

add_executable(UniqueSample ${DIR_SRCS})

