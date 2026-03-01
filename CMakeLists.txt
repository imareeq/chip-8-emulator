cmake_minimum_required(VERSION 4.2.1)
project(CHIP8_EMULATOR VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_CXX_EXTENSIONS NO)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(MSVC)
    set(COMPILE_OPTS 
        /W4 
        /WX 
        /permissive- 
        /fp:strict 
        /GS
    )
else()
    set(COMPILE_OPTS         
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

include(FetchContent)

FetchContent_Declare(
    SDL3
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG origin/main
)

FetchContent_MakeAvailable(SDL3)

set(CMAKE_CXX_FLAGS_DEBUG_INIT "-fsanitize=address,undefined")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO_INIT "-fsanitize=address,undefined")
set(CMAKE_CXX_EXE_LINKER_FLAGS_DEBUG_INIT "-fsanitize=address,undefined")
set(CMAKE_CXX_EXE_LINKER_FLAGS_RELWITHDEBINFO_INIT "-fsanitize=address,undefined")

add_library(chip_8 src/chip_8.hpp src/chip_8.cpp src/platform.hpp src/platform.cpp)
target_link_libraries(chip_8 PUBLIC SDL3::SDL3)
target_include_directories(chip_8 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

add_executable(chip8_emulator src/main.cpp)

target_compile_options(chip_8 PRIVATE ${COMPILE_OPTS})
target_compile_options(chip8_emulator PRIVATE ${COMPILE_OPTS})

target_link_libraries(chip8_emulator PRIVATE chip_8)

if(MSVC OR WIN32)
    add_custom_command(TARGET chip8_emulator POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:SDL3::SDL3>
        $<TARGET_FILE_DIR:chip8_emulator>
    )
endif()
