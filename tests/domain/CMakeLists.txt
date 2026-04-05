# Test: Scenario
add_executable(test_scenario test_scenario.cpp)
target_link_libraries(test_scenario PRIVATE tp_domain)
add_test(NAME TestScenario COMMAND test_scenario)

# Test: VectorField  
add_executable(test_vector_field test_vector_field.cpp)
target_link_libraries(test_vector_field PRIVATE tp_domain)
add_test(NAME TestVectorField COMMAND test_vector_field)

# Test: Boat
add_executable(test_boat test_boat.cpp)
target_link_libraries(test_boat PRIVATE tp_domain)
add_test(NAME TestBoat COMMAND test_boat)

# Test: Motor
add_executable(test_motor test_motor.cpp)
target_link_libraries(test_motor PRIVATE tp_domain)
add_test(NAME TestMotor COMMAND test_motor)

# Test: Boat Extended (conservación de energía, límites, torque)
add_executable(test_boat_extended test_boat_extended.cpp)
target_link_libraries(test_boat_extended PRIVATE tp_domain tp_shared)
add_test(NAME TestBoatExtended COMMAND test_boat_extended)
