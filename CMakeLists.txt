cmake_minimum_required(VERSION 3.16)

project(WarehouseApp VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(SQLite3 REQUIRED)

add_executable(warehouse_app
    src/FileIO.cpp
    src/Menu.cpp
    src/Product.cpp
    src/SqliteStorage.cpp
    src/Warehouse.cpp
    src/main.cpp
)

target_include_directories(warehouse_app PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(warehouse_app PRIVATE SQLite3::SQLite3)

if(MSVC)
    target_compile_options(warehouse_app PRIVATE /W4)
else()
    target_compile_options(warehouse_app PRIVATE -Wall -Wextra -pedantic)
endif()
