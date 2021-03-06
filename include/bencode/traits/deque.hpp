#pragma once

#include <deque>
#include "bencode/detail/serialization_traits.hpp"

namespace bencode {

template <typename T, typename Alloc>
struct serialization_traits<std::deque<T, Alloc>> : serializes_to_list {};

}