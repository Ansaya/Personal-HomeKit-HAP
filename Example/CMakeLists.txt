project(libHAPExample)
cmake_minimum_required(VERSION 3.7)

add_executable(hap_showcase "main.cpp")
target_link_libraries(hap_showcase hap)
set_target_properties(hap_showcase PROPERTIES OUTPUT_NAME showcase)

install(TARGETS hap_showcase DESTINATION ${PROJECT_SOURCE_DIR}/..)