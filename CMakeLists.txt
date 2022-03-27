cmake_minimum_required(VERSION 3.21)
project(beautiful_string)

add_subdirectory(./include/bt)

set(target beautiful_string)
add_executable(${target})
#target_include_directories(${target} PRIVATE include/bt)

target_sources(${target}
        PRIVATE
        main.cpp)

target_link_libraries(${target} PRIVATE beautiful_text)

target_compile_features(${target} PRIVATE cxx_std_20)
set_target_properties(${target} PROPERTIES CXX_EXTENSIONS OFF)


# Set options for gnu-like
target_compile_options(${target} PRIVATE
        # set warnings for all build types
        $<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-Wall -Wextra -Wpedantic -Werror -Wconversion -Wfloat-equal
        -Wformat-nonliteral -Wformat-security -Wformat-y2k -Wformat=2 -Wlogical-not-parentheses
        -Wmissing-declarations -Wmissing-field-initializers -Wmissing-format-attribute -Walloca -Wmissing-include-dirs -Wmissing-noreturn -Wpacked -Wpointer-arith -Wundef
        -Wredundant-decls -Wshadow -Wstack-protector -Wswitch-enum -Wwrite-strings>
        # debug optimisation in Debug
        $<$<AND:$<CXX_COMPILER_ID:GNU,Clang,AppleClang>,$<CONFIG:Release>>:-O3>
        # O3 optimisation Release
        $<$<AND:$<CXX_COMPILER_ID:GNU,Clang,AppleClang>,$<CONFIG:Debug>>:-Og -g -fno-omit-frame-pointer>
        )

# Set options for MSVC
target_compile_options(${target} PRIVATE
        # set warnings for all build types
        $<$<CXX_COMPILER_ID:MSVC>:/Wall>
        # debug optimisation in Debug
        $<$<AND:$<CXX_COMPILER_ID:MSVC>,$<CONFIG:Release>>:/O2>
        # O3 optimisation in Release
        $<$<AND:$<CXX_COMPILER_ID:MSVC>,$<CONFIG:Debug>>:/RTC1 /Od /Zi>
        )
# ASAN things

option(USE_ASAN "Use address sanitizer if available" ON)

execute_process(COMMAND "${CMAKE_CXX_COMPILER}" -print-file-name=libasan.so
        OUTPUT_VARIABLE LIBASAN_PATH
        RESULT_VARIABLE ASAN_RESULT
        OUTPUT_STRIP_TRAILING_WHITESPACE)

set(HAS_ASAN FALSE)

if (USE_ASAN AND (${ASAN_RESULT} EQUAL 0) AND (NOT ${LIBASAN_PATH} STREQUAL ""))
    message("libasan found @${LIBASAN_PATH}")
    message("To disable the address sanitizer set USE_ASAN to OFF.\n")
    set(HAS_ASAN TRUE)
endif ()

if (HAS_ASAN AND USE_ASAN)
    target_compile_options(${target} PRIVATE -fsanitize=address)
    target_link_options(${target} PRIVATE -fsanitize=address)
endif ()

# download and populate Google Test from github
include(FetchContent)
FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG        release-1.11.0
)

# for Windows: prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)

add_subdirectory(tests)