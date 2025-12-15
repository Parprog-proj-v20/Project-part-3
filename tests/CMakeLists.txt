enable_testing()

set(TEST_SOURCES
    deadlock_test
    distribution_test
    exchange_test
    initialize_test
    perfomance_test
    stress_test
    validation_test
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

add_test(
    NAME system_test
    COMMAND ${MPIEXEC_EXECUTABLE}
            ${MPIEXEC_NUMPROC_FLAG} 10
            $<TARGET_FILE:rabbits>
)

set_tests_properties(system_test
    PROPERTIES WILL_FAIL TRUE
)
