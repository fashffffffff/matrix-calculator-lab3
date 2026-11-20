cmake_minimum_required(VERSION 3.20)
project(MatrixCalculator VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

function(matrix_enable_warnings target)
    if(MSVC)
        target_compile_options(${target} PRIVATE /W4 /WX)
    else()
        target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic -Werror)
    endif()
endfunction()

add_library(matrix STATIC src/matrix.cpp)
target_include_directories(matrix PUBLIC include)
matrix_enable_warnings(matrix)

add_executable(matrix_calculator src/main.cpp)
target_link_libraries(matrix_calculator PRIVATE matrix)
matrix_enable_warnings(matrix_calculator)

include(CTest)
if(BUILD_TESTING)
    add_subdirectory(tests)
endif()
