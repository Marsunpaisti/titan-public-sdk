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

add_executable(rune_pouch_test rune_pouch_test.cpp)
target_link_libraries(rune_pouch_test PRIVATE titan_sdk)
add_test(NAME rune_pouch COMMAND rune_pouch_test)

add_executable(runescape_bank_test runescape_bank_test.cpp)
target_link_libraries(runescape_bank_test PRIVATE titan_sdk)
add_test(NAME runescape_bank COMMAND runescape_bank_test)
