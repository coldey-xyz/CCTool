cmake_minimum_required(VERSION 3.20)
project(CCTool LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(wxWidgets CONFIG REQUIRED COMPONENTS core base)

add_executable(CCTool WIN32
src/main.cpp
src/MainFrame.cpp
src/ImageProcessor.cpp
)
target_include_directories(CCTool PRIVATE
third_party
)
target_link_libraries(CCTool PRIVATE
wx::core
wx::base
)