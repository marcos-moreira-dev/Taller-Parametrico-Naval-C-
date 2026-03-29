# Tests de Application

add_executable(test_application
    test_experiment_service.cpp
)

target_link_libraries(test_application
    PRIVATE
        tp_application
        tp_domain
        tp_simulation
)

add_test(NAME ApplicationTests COMMAND test_application)
