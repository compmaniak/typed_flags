#
# MIT License
# Copyright (c) 2017 Roman Orlov
# See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT
#

cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++1z -pedantic -Wall -Wextra")
add_executable(example example.cpp)

