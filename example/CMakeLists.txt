#
# MIT License
# Copyright (c) 2017 Roman Orlov
# See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT
#

cmake_minimum_required(VERSION 2.8)

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++latest /W4")
elseif(MINGW)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++14 -pedantic -Wall -Wextra")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++1z -pedantic -Wall -Wextra")
endif()
add_executable(example example.cpp)

