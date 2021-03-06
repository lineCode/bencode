#pragma once

#include <iostream>
#include <optional>
#include <string_view>
#include <sstream>
#include <stack>
#include <algorithm>
#include <concepts>

#include "bencode/detail/itoa.hpp"
#include "bencode/detail/symbol.hpp"
#include "bencode/detail/bencode_type.hpp"
#include "bencode/detail/utils.hpp"
#include "bencode/detail/events/concepts.hpp"
#include "bencode/detail/parser/parsing_error.hpp"

namespace bencode::events {

template <typename OIter>
    requires std::output_iterator<OIter, char>
class encode_to
{
public:
    explicit constexpr encode_to(OIter out) noexcept
            : out_(out) {}

    encode_to(const encode_to&) = delete;
    encode_to(encode_to&&) = delete;
    encode_to& operator=(const encode_to&) = delete;
    encode_to& operator=(encode_to&&) = delete;

    constexpr void integer(std::int64_t value) noexcept
    {
        *out_++ = symbol::begin_integer;
        const auto n = itoa::to_buffer(buffer_.data(), value);
        out_ = std::copy_n(buffer_.data(), n, out_);
        *out_++ = symbol::end;
        size_ += (2 + n);
    }

    constexpr void string(std::string_view value) noexcept
    {
        std::size_t n = itoa::to_buffer(buffer_.data(), value.size());
        out_ = std::copy_n(buffer_.data(), n, out_);
        *out_++ = symbol::colon;
        out_ = std::copy_n(value.data(), value.size(), out_);
        size_ += (n + 1 + value.size());
    }

    constexpr void begin_list([[maybe_unused]] std::optional<std::size_t> size = std::nullopt) noexcept
    {
        *out_++ = symbol::begin_list;
        ++size_;
    }

    constexpr void list_item() noexcept { };

    constexpr void end_list([[maybe_unused]] std::optional<std::size_t> size = std::nullopt) noexcept
    {
        *out_++ = symbol::end;
        ++size_;
    }

    constexpr void begin_dict([[maybe_unused]] std::optional<std::size_t> size = std::nullopt) noexcept
    {
        *out_++ = symbol::begin_dict;
        ++size_;
    }

    constexpr void end_dict([[maybe_unused]] std::optional<std::size_t> size = std::nullopt) noexcept
    {
        *out_++ = symbol::end;
        ++size_;
    }

    constexpr void dict_key() noexcept { };

    constexpr void dict_value() noexcept { };

    constexpr std::size_t count() noexcept
    { return size_; }

private:
    OIter out_;
    // keep track of how many bytes are written
    std::size_t size_ {};
    // buffer for integer to string conversion
    std::array<char, 20> buffer_ {};
};

template <typename OutputIterator>
encode_to(OutputIterator out) -> encode_to<OutputIterator>;

encode_to(std::basic_ostream<char>& os) -> encode_to<std::ostreambuf_iterator<char>>;

static_assert(event_consumer<encode_to<char*>>);


} // namespace bencode::events::consumer