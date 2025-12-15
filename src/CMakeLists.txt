add_library(rabbits_lib
    Rabbits.cpp
)

target_include_directories(rabbits_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(rabbits_lib MPI::MPI_CXX)

add_executable(rabbits main.cpp)
target_link_libraries(rabbits rabbits_lib MPI::MPI_CXX)