add_library(bi STATIC DB.cpp)

target_include_directories(
    bi
    PUBLIC
    ${CMAKE_SOURCE_DIR}/third_party/MurmurHash
    ${CMAKE_CURRENT_LIST_DIR})

target_sources(bi PRIVATE
                  Index.cpp
                  LRU.cpp
                  RowData.cpp
                  ThreadPool.cpp
)
