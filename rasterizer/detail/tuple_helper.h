#pragma once

#include <array>
#include <algorithm>
#include <cstddef>
#include <tuple>
#include <type_traits>

namespace detail
{

template <typename T, typename Func, std::size_t... Is>
void tuple_iter_impl(const Func& func, const T& v0, const T& v1, const T& v2, T& result, std::index_sequence<Is...>)
{
    (func(std::get<Is>(v0), std::get<Is>(v1), std::get<Is>(v2), std::get<Is>(result)), ...);
}

template <typename T, typename Func, std::size_t Size = std::tuple_size_v<T>>
void tuple_iter(const Func& func, const T& v0, const T& v1, const T& v2, T& res)
{
    tuple_iter_impl(func, v0, v1, v2, res, std::make_index_sequence<Size>{});
}


template <typename T, typename Func, std::size_t... Is>
void tuple_iter_impl(const Func& func, const T& v0, const T& v1, T& result, std::index_sequence<Is...>)
{
    (func(std::get<Is>(v0), std::get<Is>(v1), std::get<Is>(result)), ...);
}

template <typename T, typename Func, std::size_t Size = std::tuple_size_v<T>>
void tuple_iter(const Func& func, const T& v0, const T& v1, T& res)
{
    tuple_iter_impl(func, v0, v1, res, std::make_index_sequence<Size>{});
}


template <typename T, typename Func, std::size_t... Is>
void tuple_iter_impl(const Func& func, T& value, std::index_sequence<Is...>)
{
    (func(std::get<Is>(value)), ...);
}

template <typename T, typename Func, std::size_t Size = std::tuple_size_v<T>>
void tuple_iter(const Func& func, T& value)
{
    tuple_iter_impl(func, value, std::make_index_sequence<Size>{});
}


template <typename T, typename Tuple>
struct tuple_has_type;

template <typename T, typename... Us>
struct tuple_has_type<T, std::tuple<Us...>> : std::disjunction<std::is_same<T, Us>...> {};



template <class T, class Tuple>
struct tuple_index;

template <class T, typename... Ts>
struct tuple_index<T, std::tuple<Ts...>>
{
    static constexpr std::size_t index = []()
    {
        static_assert(tuple_has_type<T, std::tuple<Ts...>>::value, "Couldn't find searched type in tuple!" );

        constexpr std::array<bool, sizeof...(Ts)> a{{ std::is_same<T, Ts>::value... }};
        const auto it = std::find(a.begin(), a.end(), true);
        return std::distance(a.begin(), it);
    }();
};

}
