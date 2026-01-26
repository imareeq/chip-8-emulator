cmake_minimum_required(VERSION 4.2.1)
project(CHIP8_EMULATOR VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_CXX_EXTENSIONS NO)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(MSVC)
    add_compile_options(
        /W4              
        /WX
        /permissive-
        /fp:strict
        /GS
    )
else()
    add_compile_options(
        -Wall
        -Wextra
        -Werror
        -pedantic-errors
        -Wconversion
        -Wsign-conversion
        -Wdouble-promotion
        -Wcast-align
        -Wformat=2
        -Wuninitialized
        -Wnull-dereference
        -Wnon-virtual-dtor
        -Woverloaded-virtual
        -Wdeprecated-copy-dtor
        -Wold-style-cast
        -Wzero-as-null-pointer-constant
        -Wsuggest-override
        -fstack-protector-strong
    )
endif()

set(CMAKE_CXX_FLAGS_DEBUG_INIT "-fsanitize=address,undefined")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO_INIT "-fsanitize=address,undefined")
set(CMAKE_CXX_EXE_LINKER_FLAGS_DEBUG_INIT "-fsanitize=address,undefined")
set(CMAKE_CXX_EXE_LINKER_FLAGS_RELWITHDEBINFO_INIT "-fsanitize=address,undefined")

include(FetchContent)
FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.12.0
)
FetchContent_MakeAvailable(Catch2)

enable_testing()

add_library(chip_8 src/chip_8.h src/chip_8.cpp)
target_include_directories(chip_8 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

add_executable(chip8_tests src/chip_8.test.cpp)
target_link_libraries(chip8_tests PRIVATE chip_8 Catch2::Catch2WithMain)
