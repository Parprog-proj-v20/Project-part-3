enable_testing()

set(TEST_SOURCES

)

foreach(test ${TEST_SOURCES})
    add_executable(${test} ${test}.cpp)
    target_link_libraries(${test} rabbits_lib MPI::MPI_CXX)

    add_test(
        NAME ${test}
        COMMAND ${MPIEXEC_EXECUTABLE}
                ${MPIEXEC_NUMPROC_FLAG} 20
                $<TARGET_FILE:${test}>
    )
endforeach()
