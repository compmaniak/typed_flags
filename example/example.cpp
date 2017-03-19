//
// MIT License
// Copyright (c) 2017 Roman Orlov
// See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT
//

#include "../include/typed_flags.hpp"
#include <cassert>

using namespace tfl;

// at first declare some types
class eats_meat;
class eats_grass;
class has_tail;
class build_spaceships;

// then bind types to flag identifiers
typedef typed_flags<eats_meat, eats_grass, has_tail> animal;
typedef typed_flags<eats_meat, eats_grass, build_spaceships> engineer;

int main()
{
    // unlike std::bitset typed_flags allocates less memory
    assert( sizeof(animal) == 1 );
    
    // create flags from scratch
    animal wolf;
    wolf.set<eats_grass>(false);
    wolf.set<eats_meat, has_tail>();
    wolf.set(flag<has_tail>{1}, flag<eats_meat>{1});
    
    // create flags with flexible human-readable constructor
    wolf = animal{flag<has_tail>{1}, flag<eats_meat>{1}, flag<eats_grass>{0}};
    
    // test each flag separately
    assert( (wolf.test<eats_meat>() && wolf.test<has_tail>()) );
    
    // test group off flags in one call
    assert( (wolf.all<eats_meat, has_tail>()) );
    assert( (wolf.any<eats_meat, has_tail, eats_grass>()) );
    assert( (wolf.none<eats_grass>()) );
    
    // extract flag values
    flag<eats_meat> f1;
    flag<eats_grass> f2;
    flag<has_tail> f3;
    wolf.get(f1, f2);
    wolf.get(f3);
    assert( f1 && !f2 && f3 );
    
    // like std::bitset create flags from integers or strings 
    // and convert vice versa
    auto a1 = animal{3};
    auto a2 = animal{"101"};
    assert( a1.to_integral<int>() == 3 );
    assert( a2.to_string() == "101" );
    
    // there are bitwise member and non-member functions
    auto a3 = wolf;
    a3 &= animal{"001"};
    assert( a3 == animal{flag<eats_meat>{1}} );
    assert( (a3 | animal{2}) == animal{3} );
    
    //wolf.set<build_spaceships>(); // compile error!
    //wolf = engineer{1}; // compile error!
}
