#pragma once

namespace detail
{

template<typename T>
struct has_member
{
    typedef char one;
    struct two { char _x[2]; };

#define test_one(name) template<typename C> static one test_##name( decltype (&C::name) );
#define test_two(name) template<typename C> static two test_##name(...);
#define test_value(name) struct name{ enum { value = sizeof( test_##name<T>(0) ) == sizeof(char) }; };
#define property(name) test_one(name) test_two(name) test_value(name)

    /* vertex members */
    property(position)
    property(color)
    property(uv)
    property(normal)
    property(world_position)
    property(model_position)

#undef test_one
#undef test_two
#undef test_value
#undef property
};

}
