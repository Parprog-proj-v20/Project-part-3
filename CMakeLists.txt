cmake_minimum_required(VERSION 3.13)
project(proj-part-3 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(MPI REQUIRED)

enable_testing()

add_library(rabbits_lib src/Rabbits.cpp)
target_include_directories(rabbits_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(rabbits_lib MPI::MPI_CXX)

add_executable(rabbits src/main.cpp)
target_link_libraries(rabbits rabbits_lib MPI::MPI_CXX)

set(TEST_SOURCES
    boundary_test
    completion_test
    convergence_test
    deadlock_test
    distribution_test
    exchange_test
    initialize_test
    non-negativity_test
    performance_test
    statistics_test
    values_test
    zero_carrots_test
)

foreach(test ${TEST_SOURCES})
    add_executable(${test} tests/${test}.cpp)
    target_link_libraries(${test} rabbits_lib MPI::MPI_CXX)

    add_test(
        NAME ${test}
        COMMAND ${MPIEXEC_EXECUTABLE}
                ${MPIEXEC_NUMPROC_FLAG} 20
                $<TARGET_FILE:${test}>
    )
endforeach()

add_test(
    NAME system_test
    COMMAND ${MPIEXEC_EXECUTABLE}
            ${MPIEXEC_NUMPROC_FLAG} 10
            $<TARGET_FILE:rabbits>
)

set_tests_properties(system_test
    PROPERTIES WILL_FAIL TRUE
)


