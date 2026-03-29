# Test: Integrators
add_executable(test_integrators test_integrators.cpp)
target_include_directories(test_integrators PRIVATE ${CMAKE_SOURCE_DIR}/src/simulation)
target_link_libraries(test_integrators PRIVATE tp_simulation tp_domain)
add_test(NAME TestIntegrators COMMAND test_integrators)

# Test: Physics
add_executable(test_physics test_physics.cpp)
target_include_directories(test_physics PRIVATE ${CMAKE_SOURCE_DIR}/src/simulation)
target_link_libraries(test_physics PRIVATE tp_simulation tp_domain)
add_test(NAME TestPhysics COMMAND test_physics)
