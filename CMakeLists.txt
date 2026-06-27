cmake_minimum_required(VERSION 3.16)
project(TaylorE
    VERSION 1.0.0
    DESCRIPTION "Taylor series expansion visualizer for e^(-x)"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ── Основное приложение ────────────────────────────────────────────────
add_executable(TaylorE main.cpp)

# ── Установщик (только Windows) ────────────────────────────────────────
if(WIN32)
    add_executable(setup setup.cpp)
    target_link_libraries(setup PRIVATE shell32 ole32)
endif()
