# EXCERPT — source: CMakeLists.txt
# EVIDENCE: E1 | symbol: FD_SETSIZE | lines: 9-19

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
idf_build_set_property(MINIMAL_BUILD ON)

# LWIP_MAX_SOCKETS=20 requires FD_SETSIZE >= MAX_SOCKETS + offset.
# Use 30 for safety (stdin/stdout/stderr + sockets).
add_compile_definitions(FD_SETSIZE=30)

project(KC868_A16_EnIP)
