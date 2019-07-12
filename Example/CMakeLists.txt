project(libHAPExample)
cmake_minimum_required(VERSION 3.7)

add_executable(hap_showcase "main.cpp")
target_link_libraries(hap_showcase 
    PUBLIC 
    HAP::HAP )

install(TARGETS hap_showcase DESTINATION ${PROJECT_SOURCE_DIR}/..)