cmake_minimum_required(VERSION 3.14)

add_executable(bencode-tests)
target_sources(bencode-tests
    PUBLIC
        main.cpp
        basic_bvalue/test_accessors.cpp
        basic_bvalue/test_conversion.cpp
        basic_bvalue/test_comparison.cpp
        basic_bvalue/test_constructor.cpp
        basic_bvalue/test_basic_bvalue.cpp
        basic_bvalue/test_events.cpp

        bview/test_accessors.cpp
        bview/test_conversion.cpp
        bview/test_integer_bview.cpp
        bview/test_string_bview.cpp
        bview/test_list_bview.cpp
        bview/test_dict_bview.cpp

        parser/test_common.cpp
        parser/push_parser.cpp
        parser/descriptor_parser.cpp

        test_concepts.cpp
        test_encoder.cpp
        test_itoa.cpp
        test_bencode_type.cpp
        test_connect.cpp
)

#include_directories("../include/")
#include(../external/CMakeLists.txt)
target_link_libraries(bencode-tests
        Catch2::Catch2
        bencode
)

if (BENCODE_BUILD_TESTS_COVERAGE)
    message(STATUS "Building with coverage enabled")
    set(CMAKE_CXX_FLAGS --coverage)
    set(CMAKE_C_FLAGS --coverage)
endif()

target_compile_options(bencode-tests PRIVATE -fconcepts-diagnostics-depth=4)

target_compile_definitions(bencode-tests PRIVATE RESOURCES_DIR=\"${CMAKE_CURRENT_SOURCE_DIR}/resources\")

target_compile_options(bencode-tests PRIVATE)

include(Catch)
catch_discover_tests(bencode-tests)