# Tests

enable_testing()

add_subdirectory(domain)
add_subdirectory(simulation)
add_subdirectory(application)

# Test general
add_test(NAME AllTests COMMAND ctest)
