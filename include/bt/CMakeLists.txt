cmake_minimum_required(VERSION 3.12)

project("beautiful_text" VERSION 1.0.0
        DESCRIPTION "A header only C++ for text manipulation."
        HOMEPAGE_URL "https://github.com/zaldawid/")

add_library(beautiful_text INTERFACE)

target_compile_features(beautiful_text INTERFACE cxx_std_20)

target_include_directories(
        beautiful_text
        INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)
