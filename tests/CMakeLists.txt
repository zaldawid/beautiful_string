enable_testing()

add_executable(tests_dynamic_storage tests_dynamic_storage.cpp)

list( APPEND targets tests_dynamic_storage )

include(GoogleTest)

foreach(target ${targets})
    target_compile_features(${target} PRIVATE cxx_std_20)
    set_target_properties(${target} PROPERTIES CXX_EXTENSIONS ON)

    target_include_directories(${target} PRIVATE ../include/bs)

    target_link_libraries(${target} gtest_main gmock)
    gtest_discover_tests(${target})

endforeach()


