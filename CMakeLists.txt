cmake_minimum_required(VERSION 3.13)
project(mojave)

set(CMAKE_CXX_STANDARD 17)

ADD_SUBDIRECTORY(stemmer)

# stembundle and memory index library
ADD_LIBRARY(memIndex STATIC
        memIndex.cc )
ADD_LIBRARY(stembundle STATIC
        StemBundle.cc)
ADD_LIBRARY(mojaveStorage STATIC
        mojaveStorage.cc)

TARGET_LINK_LIBRARIES(stembundle
        memIndex
        hash
        porter)
TARGET_LINK_LIBRARIES(mojaveStorage
        hash
        porter)
# !- Main Part -!
# inverted index

ADD_EXECUTABLE(mojave
        mojave.cc)

TARGET_LINK_LIBRARIES(mojave stembundle mojaveStorage)
