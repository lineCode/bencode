#pragma once

#include <iostream>
#include <optional>
#include <stack>
#include <sstream>
#include <ranges>

#include <bencode/detail/bencode_type.hpp>
#include <bencode/detail/utils.hpp>
#include <bencode/detail/parser/parsing_error.hpp>
#include <bencode/detail/itoa.hpp>

namespace bencode::events {

using namespace std::string_view_literals;

template <typename OIter>
    requires std::output_iterator<OIter, char>
class format_json_to {
public:
    explicit format_json_to(OIter out, size_t indent=4)
            : out_(out)
            , indent_(indent)
            , current_indent_(0)
    {
        std::fill(line_buffer_.begin(), line_buffer_.end(), ' ');
    };

    explicit format_json_to(std::ostream& out, size_t indent=4)
        : format_json_to(std::ostreambuf_iterator{out}, indent)
    {};

    format_json_to(const format_json_to&) = delete;
    format_json_to(format_json_to&&) = delete;

    void integer(std::int64_t value)
    {
        next();
        auto size = itoa::to_buffer(int_buffer_.data(), value);
        std::copy_n(int_buffer_.data(), size, out_);
    }

    void string(std::string_view value)
    {
        next();
        *out_++ = '"';
        std::copy_n(value.data(), value.size(), out_);
        *out_++ = '"';
    }

    void begin_list([[maybe_unused]] std::optional<std::size_t> size = std::nullopt)
    {
        next();
        *out_++ = '[';
        current_indent_ += indent_;
        first_ = true;
    };

    void list_item()
    {
        first_ = false;
    };

    void end_list([[maybe_unused]] std::optional<std::size_t> size = std::nullopt)
    {
        current_indent_ -= indent_;
        if (!first_) next_line();
        *out_++ = ']';
    };

    void begin_dict([[maybe_unused]] std::optional<std::size_t> size = std::nullopt)
    {
        next();
        *out_++ = '{';
        current_indent_ += indent_;
        first_ = true;
    };

    void end_dict([[maybe_unused]] std::optional<std::size_t> size = std::nullopt)
    {
        current_indent_ -= indent_;
        if (!first_) next_line();
        *out_++ = '}';
    };

    void dict_key()
    {
        std::copy_n(": ", 2, out_);
        first_ = true;
        after_key_ = true;
    }

    void dict_value() noexcept
    {
        first_ = false;
    }

    static void error(const bencode::parsing_error& e) {
        throw e;
    }

private:
    void next_line()
    {
        *out_++ = '\n';
        for (std::size_t len = current_indent_; len != 0; ) {
            const auto chunk_size = std::min(len, line_buffer_.size());
            std::copy_n(line_buffer_.data(), chunk_size, out_);
            len -= chunk_size;
        }
    }

    void next()
    {
        if (!first_)
            *out_++ = ',';

        if (after_key_)
            after_key_ = false;
        else
            next_line();
    }

    OIter out_;
    std::size_t indent_;
    std::size_t current_indent_;
    bool first_ = true;
    bool after_key_ = true;

    std::array<char, 24> line_buffer_ {};
    std::array<char, 20> int_buffer_ {};
    // buffer for integer to string conversion
};

template <typename OutputIterator>
format_json_to(OutputIterator out) -> format_json_to<OutputIterator>;

format_json_to(std::basic_ostream<char>& out) -> format_json_to<std::ostreambuf_iterator<char>>;

static_assert(event_consumer<format_json_to<char*>>, "internal error");

} // namespace bencode::events::consumer

