#pragma once

#include <cstddef>
#include <tuple>

namespace detail
{

template <typename T, typename Func, std::size_t... Is>
void iter_tuple_impl(const Func& func, const T& v0, const T& v1, const T& v2, T& result, std::index_sequence<Is...>)
{
    (func(std::get<Is>(v0), std::get<Is>(v1), std::get<Is>(v2), std::get<Is>(result)), ...);
}

template <typename T, typename Func, std::size_t Size = std::tuple_size_v<T>>
void iter_tuple(const Func& func, const T& v0, const T& v1, const T& v2, T& res)
{
    iter_tuple_impl(func, v0, v1, v2, res, std::make_index_sequence<Size>{});
}

}
