cmake_minimum_required(VERSION 3.24)
project(titan_sdk_tests LANGUAGES CXX)

enable_testing()
add_subdirectory(.. titan_sdk)

add_executable(rune_element_test rune_element_test.cpp)
target_link_libraries(rune_element_test PRIVATE titan_sdk)
add_test(NAME rune_element COMMAND rune_element_test)

add_executable(rune_element_equipment_test rune_element_equipment_test.cpp)
target_link_libraries(rune_element_equipment_test PRIVATE titan_sdk)
add_test(NAME rune_element_equipment COMMAND rune_element_equipment_test)

add_executable(rune_amounts_test rune_amounts_test.cpp)
target_link_libraries(rune_amounts_test PRIVATE titan_sdk)
add_test(NAME rune_amounts COMMAND rune_amounts_test)
