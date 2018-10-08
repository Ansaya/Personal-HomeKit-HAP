project(libHAP)
cmake_minimum_required(VERSION 3.7)

set(LIB_NAME "hap")

file(GLOB_RECURSE LIB_SRCS "srcs/*.cpp")
file(GLOB LIB_HEADERS "include/*.h")
file(GLOB LIB_NET_HEADERS "include/net/*.h")

file(GLOB_RECURSE CHACHA20_SRCS "Chacha20/*.c")
file(GLOB_RECURSE CURVE25519_SRCS "curve25519/*.c")
file(GLOB_RECURSE CURVE25519_SRCS_DONNA_SRCS "ed25519-donna/*.c")
file(GLOB_RECURSE POLY1305_OPT_SRCS "poly1305-opt-master/*.c")
file(GLOB_RECURSE RFC6234_SRCS "rfc6234-master/*.c")
file(GLOB_RECURSE SRP_SRCS "srp/*.c")

add_compile_options(
	"-Wall"
	"-Wno-deprecated-declarations"
	"-Wno-format"
	)

set( CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -O0 -g2 -gdwarf-2" )
set( CMAKE_C_FLGAS_RELEASE "${CMAKE_C_FLAGS_RELAEASE} -O3" )

set ( CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -g2 -gdwarf-2 -std=c++11" )
set ( CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -std=c++11" )

include_directories(
	"include" 
	"Chacha20" 
	"curve25519" 
	"ed25519-donna" 
	"poly1305-opt-master" 
	"rfc6234-master" 
	"srp"
	)
	
link_libraries(ssl crypto dl pthread dns_sd)

add_library(${LIB_NAME}_static STATIC "Helpers.cpp" ${LIB_SRCS} ${CHACHA20_SRCS} ${CURVE25519_SRCS} ${CURVE25519_SRCS_DONNA_SRCS} ${POLY1305_OPT_SRCS} ${RFC6234_SRCS} ${SRP_SRCS} )
set_target_properties(${LIB_NAME}_static PROPERTIES OUTPUT_NAME ${LIB_NAME})

add_library(${LIB_NAME} SHARED "Helpers.cpp" ${LIB_SRCS} ${CHACHA20_SRCS} ${CURVE25519_SRCS} ${CURVE25519_SRCS_DONNA_SRCS} ${POLY1305_OPT_SRCS} ${RFC6234_SRCS} ${SRP_SRCS} )

### Installation
install(TARGETS ${LIB_NAME}
        LIBRARY DESTINATION /usr/local/lib
)

install(TARGETS ${LIB_NAME}_static
        ARCHIVE DESTINATION /usr/local/lib
)

install(FILES ${LIB_HEADERS}
        DESTINATION /usr/local/include/${LIB_NAME}
)

install(FILES ${LIB_NET_HEADERS}
        DESTINATION /usr/local/include/${LIB_NAME}/net
)

### Showcase application
add_subdirectory(Example)