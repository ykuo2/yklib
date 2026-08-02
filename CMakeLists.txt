cmake_minimum_required(VERSION 3.23)

project(yklib LANGUAGES C)

add_library(yklib STATIC)

target_sources(yklib
    PRIVATE
        src/list.c

    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS include
        FILES
            include/yklib/list.h
)

target_include_directories(yklib
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
)

# C11
target_compile_features(yklib
    PUBLIC
        c_std_11
)
