# Tests de Integración
# Tests end-to-end que verifican flujos completos

add_executable(test_preset_scenarios
    test_preset_scenarios.cpp
)

target_link_libraries(test_preset_scenarios
    PRIVATE
        tp_application
        tp_domain
        tp_simulation
        tp_shared
)

add_test(NAME PresetScenariosTests COMMAND test_preset_scenarios)