//
// MIT License
// Copyright (c) 2017 Roman Orlov
// See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT
//

#include "../include/typed_flags.hpp"
#include <cassert>

using namespace tfl;

class has_tail;
class eats_meat;
class eats_grass;
class builds_spaceships;

typedef typed_flags<eats_meat, eats_grass, has_tail> animal;
typedef typed_flags<eats_meat, eats_grass, builds_spaceships> human;

int main()
{    
    //typed_flags<eats_meat, eats_meat> ill_formed; // compilation error!
    
    typed_flags<> empty;
    assert( empty.size() == 0 );
    assert( sizeof(empty) == 1 );
    assert( empty.to_integral<int>() == 0 );
    assert( empty.none<>() );
    assert( !empty.any<>() );
    assert( empty.all<>() );
    empty = typed_flags<>(1);
    assert( empty.to_integral<int>() == 0 );
        
    assert( animal::index<eats_meat>() == 0 );
    assert( animal::index<eats_grass>() == 1 );
    assert( animal::index<has_tail>() == 2 );
    //animal::index<builds_spaceships>(); // compilation error!
    
    animal wolf;
    assert( wolf.size() == 3 );
    assert( sizeof(wolf) == 1 );
    assert( !wolf.test<eats_meat>() );
    assert( !wolf.test<eats_grass>() );
    assert( !wolf.test<has_tail>() );
    assert( wolf.to_string() == "000" );
    wolf.set<eats_meat>();
    assert( wolf.test<eats_meat>() );
    assert( wolf.to_string() == "001" );
    wolf.set<eats_grass>(true);
    assert( wolf.test<eats_grass>() );
    assert( wolf.to_string('-') == "-11" );
    wolf.set<eats_grass>(false);
    assert( !wolf.test<eats_grass>() );
    wolf.set<has_tail>();
    assert( wolf.test<has_tail>() );
    assert( wolf.to_integral<int>() == 5 );
    assert( wolf.to_string('-', '+') == "+-+" );
    
    animal rabbit;
    rabbit.set<eats_grass, has_tail>();
    assert( !rabbit.test<eats_meat>() );
    assert( rabbit.test<eats_grass>() );
    assert( rabbit.test<has_tail>() );
    assert( rabbit.to_integral<int>() == 6 );
    
    animal unknown;
    assert( unknown.to_integral<int>() == 0 );
    assert( unknown.none() );
    assert( !unknown.any() );
    assert( !unknown.all() );
    unknown.set();
    assert( unknown.to_integral<int>() == 7 );
    assert( !unknown.none() );
    assert( unknown.any() );
    assert( unknown.all() );
    animal tmp = unknown;
    tmp.reset<eats_meat, eats_grass, has_tail>();
    assert( tmp != unknown );
    unknown.reset();
    assert( tmp == unknown );
    assert( unknown.to_integral<int>() == 0 );
    assert( unknown.none() );
    assert( !unknown.any() );
    assert( !unknown.all() );
    assert( unknown != wolf );
    unknown = wolf;
    assert( unknown == wolf );
    assert( unknown.to_integral<int>() == wolf.to_integral<int>() );
    
    human engineer;
    assert( (engineer.none<eats_meat>()) );
    assert( (engineer.none<eats_meat, eats_grass>()) );
    assert( (engineer.none<eats_meat, eats_grass, builds_spaceships>()) );
    assert( (!engineer.any<eats_meat>()) );
    assert( (!engineer.any<eats_meat, eats_grass>()) );
    assert( (!engineer.any<eats_meat, eats_grass, builds_spaceships>()) );
    assert( (!engineer.all<eats_meat>()) );
    assert( (!engineer.all<eats_meat, eats_grass>()) );
    assert( (!engineer.all<eats_meat, eats_grass, builds_spaceships>()) );
    engineer.set<builds_spaceships>();
    assert( engineer.to_integral<int>() == 4 );
    assert( (engineer.none<eats_meat>()) );
    assert( (engineer.none<eats_meat, eats_grass>()) );
    assert( (!engineer.none<eats_meat, eats_grass, builds_spaceships>()) );
    assert( (!engineer.any<eats_meat>()) );
    assert( (!engineer.any<eats_meat, eats_grass>()) );
    assert( (engineer.any<eats_meat, eats_grass, builds_spaceships>()) );
    assert( (!engineer.all<eats_meat>()) );
    assert( (!engineer.all<eats_meat, eats_grass>()) );
    assert( (!engineer.all<eats_meat, eats_grass, builds_spaceships>()) );
    engineer.flip<eats_meat, eats_grass, builds_spaceships>();
    assert( (!engineer.none<eats_meat>()) );
    assert( (!engineer.none<eats_meat, eats_grass>()) );
    assert( (!engineer.none<eats_meat, eats_grass, builds_spaceships>()) );
    assert( (engineer.any<eats_meat>()) );
    assert( (engineer.any<eats_meat, eats_grass>()) );
    assert( (engineer.any<eats_meat, eats_grass, builds_spaceships>()) );
    assert( (engineer.all<eats_meat>()) );
    assert( (engineer.all<eats_meat, eats_grass>()) );
    assert( (!engineer.all<eats_meat, eats_grass, builds_spaceships>()) );
    engineer.flip();
    assert( !engineer.test<eats_meat>() );
    assert( !engineer.test<eats_grass>() );
    assert( engineer.test<builds_spaceships>() );
    assert( engineer.to_integral<int>() == 4 );
    
    //engineer = wolf; // compilation error!
    
    unknown = animal{65535};
    assert( unknown.to_integral<int>() == 7 );
    unknown = ~unknown;
    assert( unknown.to_integral<int>() == 0 );
    unknown |= animal{2};
    assert( unknown.to_integral<int>() == 2 );
    unknown ^= animal{7};
    assert( unknown.to_integral<int>() == 5 );
    unknown &= animal{4};
    assert( unknown.to_integral<int>() == 4 );
    
    constexpr flag<int> const_int_flag_0;
    constexpr flag<int> const_int_flag_1{1};
    flag<int> int_flag;
    assert( !int_flag && int_flag == const_int_flag_0 );
    int_flag = true;
    assert( int_flag && int_flag == const_int_flag_1 );
    int_flag = false;
    assert( !int_flag );
    
    unknown = animal{flag<has_tail>{1}, flag<eats_grass>{1}};
    assert( unknown == rabbit );
    unknown.set(flag<eats_meat>{1});
    assert( unknown != rabbit );
    unknown.set(flag<eats_meat>{0}, flag<eats_grass>{1});
    assert( unknown == rabbit );
    flag<eats_meat> f1;
    flag<eats_grass> f2;
    flag<has_tail> f3;
    unknown.get(f1, f2, f3);
    assert( f2 && f3 );
    assert( !f1 );
    
    auto a1 = animal{"111"} & animal{"010"};
    assert( a1.to_string() == "010" );
    auto a2 = animal{"11100"} | animal{"11001"};
    assert( a2.to_string() == "101" );
    auto a3 = animal{"10"} ^ animal{"11"};
    assert( a3.to_string() == "001" );
    try
    {
        animal{"X"};
        assert( false );
    }
    catch (std::invalid_argument const&)
    {
    }
    
    typed_flags<class f1, class f2, class f3, 
                class f4, class f5, class f6, 
                class f7, class f8>
                flags_8;
    assert( flags_8.size() == 8 );
    assert( sizeof(flags_8) == 1 );
    flags_8.set<class f8>();
    assert( flags_8.to_integral<int>() == 128 );
    assert( flags_8.to_string() == "10000000" );
    
    typed_flags<class f1, class f2, class f3, 
                class f4, class f5, class f6, 
                class f7, class f8, class f9>
                flags_9;
    assert( flags_9.size() == 9 );
    assert( sizeof(flags_9) == 2 );
    //flags_9.to_integral<uint8_t>(); // compilation error!
    flags_9.set<class f1, class f9>();
    assert( flags_9.to_integral<int>() == 257 );
    assert( flags_9.to_string() == "100000001" );
    
    return 0;
}
