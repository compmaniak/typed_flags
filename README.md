# Typed flags
[![Build Status](https://travis-ci.org/compmaniak/typed_flags.svg?branch=master)](https://travis-ci.org/compmaniak/typed_flags)
[![Build status](https://ci.appveyor.com/api/projects/status/kyrm1lmn2ciaxbel?svg=true)](https://ci.appveyor.com/project/compmaniak/typed-flags)
[![Try online](https://img.shields.io/badge/Try-online-4DB6AC.svg)](http://melpon.org/wandbox/permlink/55g1czUjwSFO8LS1)

Type-safe and human-readable sets of bool flags.

## Quick start

Start by declaring some types
```cpp
class eats_meat;
class eats_grass;
class has_tail;
```
Then bind these types to flag identifiers
```cpp
typedef typed_flags<eats_meat, eats_grass, has_tail> animal;
```
Create flags from scratch
```cpp
animal wolf;
wolf.set<eats_grass>(false);
wolf.set<eats_meat, has_tail>();
wolf.set(flag<has_tail>{1}, flag<eats_meat>{1});
```
Create flags with a flexible human-readable constructor
```cpp
wolf = animal{flag<has_tail>{1}, flag<eats_meat>{1}, flag<eats_grass>{0}};
```
Test each flag separately
```cpp
assert( (wolf.test<eats_meat>() && wolf.test<has_tail>()) );
```
Test a group of flags in one call
```cpp
assert( (wolf.all<eats_meat, has_tail>()) );
assert( (wolf.any<eats_meat, has_tail, eats_grass>()) );
assert( (wolf.none<eats_grass>()) );
```
Extract flag values
```cpp
flag<eats_meat> f1;
flag<has_tail> f2;
wolf.get(f1, f2);
assert( f1 && f2 );
```
Create flags from integers or strings and convert back and forth - like std::bitset
```cpp
auto a1 = animal{3};
auto a2 = animal{"101"};
assert( a1.to_integral<int>() == 3 );
assert( a2.to_string() == "101" );
```

## Documentation

You can find more detailed info [here](https://compmaniak.github.io/typed_flags/classtyped__flags.html).

## Requirements

To build tests and examples you need
* cmake
* compiler supporting fold-expressions from C++17 (GCC 6, Clang 3.8+)

