add_library(hash STATIC MurmurHash2.cpp)

target_include_directories(
    hash
    PUBLIC
    ${CMAKE_CURRENT_LIST_DIR})
