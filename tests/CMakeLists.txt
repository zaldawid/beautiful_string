enable_testing()

add_executable(tests_dynamic_storage tests_dynamic_storage.cpp)
list( APPEND targets tests_dynamic_storage )

include(GoogleTest)

# ASAN things

option(USE_ASAN "Use address sanitizer if available" ON)
set(HAS_ASAN FALSE)

if (NOT CMAKE_CXX_COMPILER_ID MATCHES "MSVC")

    execute_process(COMMAND "${CMAKE_CXX_COMPILER}" -print-file-name=libasan.so
        OUTPUT_VARIABLE LIBASAN_PATH
        RESULT_VARIABLE ASAN_RESULT
        OUTPUT_STRIP_TRAILING_WHITESPACE)


    if (USE_ASAN AND (${ASAN_RESULT} EQUAL 0) AND (NOT ${LIBASAN_PATH} STREQUAL ""))
        message("libasan found @${LIBASAN_PATH}")
        message("Address Sanitizer enabled. To disable it set USE_ASAN to OFF.\n")
        set(HAS_ASAN TRUE)
    endif ()

else()
    message("Address Sanitizer disabled for MSVC builds")

endif (NOT CMAKE_CXX_COMPILER_ID MATCHES "MSVC")

foreach(target ${targets})
    target_compile_features(${target} PRIVATE cxx_std_20)
    set_target_properties(${target} PROPERTIES CXX_EXTENSIONS ON)

    if (${USE_ASAN} AND ${HAS_ASAN})
        target_compile_options(${target} PRIVATE -fsanitize=address)
        target_link_options(${target} PRIVATE -fsanitize=address)
    endif (${USE_ASAN} AND ${HAS_ASAN})

    target_link_libraries(${target} PRIVATE beautiful_text)

    target_link_libraries(${target} PRIVATE gtest_main gmock)

    gtest_discover_tests(${target})

endforeach()


