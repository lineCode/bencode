//#include <bencode/detail/parser/input_adapter.hpp>
#include <bencode/detail/parser/push_parser.hpp>
#include <bencode/detail/events/format_json_to.hpp>
#include <bencode/detail/events/debug_to.hpp>
#include <bencode/detail/bvalue/events.hpp>
#include <catch2/catch.hpp>
#include <sstream>

#include "data.hpp"

using namespace std::string_view_literals;
using namespace bencode;

TEST_CASE("test push parser to json")
{
    auto parser = push_parser();
    std::string out{};
    auto json_consumer = bencode::events::format_json_to(std::back_inserter(out));
    auto consumer = bencode::events::debug_to(std::back_inserter(out));

    SECTION("compare json output") {
        const auto [data, expected] = GENERATE_COPY(table<std::string_view, std::string_view>({
                {example,        example_json_result},
                {sintel_torrent, sintel_json_result}
        }));

        bool success = parser.parse(json_consumer, data);
        CHECK(out == expected);
    }

    SECTION("error - recursion limit - list") {
        auto parser2 = bencode::push_parser({.recursion_limit=10});
        auto r = parser2.parse(consumer, recursion_limit_list);
        CHECK_FALSE(r);
        CHECK(parser2.error().errc() == parsing_errc::recursion_depth_exceeded);
    }


    SECTION("error - recursion limit - dict") {
        auto parser2 = bencode::push_parser({.recursion_limit=10});
        auto r = parser2.parse(consumer, recursion_limit_dict);
        CHECK_FALSE(r);
        CHECK(parser2.error().errc() == parsing_errc::recursion_depth_exceeded);
    }

    SECTION("error - value limit") {
        auto parser2 = bencode::push_parser({.value_limit=10});
        auto r = parser2.parse(consumer, sintel_torrent);
        CHECK_FALSE(r);
        CHECK(parser2.error().errc() == parsing_errc::value_limit_exceeded);
    }

    SECTION("error - integer parsing") {
        auto r = parser.parse(consumer, error_integer);
        REQUIRE_FALSE(r);
        CHECK(parser.error().errc() == parsing_errc::leading_zero);
    }
    SECTION("error - string parsing") {
        auto r = parser.parse(consumer, error_string);
        REQUIRE_FALSE(r);
        CHECK(parser.error().errc() == parsing_errc::unexpected_eof);
    }
    SECTION("error - dict key parsing") {
        auto r = parser.parse(consumer, error_dict_key);
        REQUIRE_FALSE(r);
        CHECK(parser.error().errc() == parsing_errc::expected_colon);
    }
    SECTION("error - missing end - list") {
        auto r = parser.parse(consumer, error_missing_end_list);
        REQUIRE_FALSE(r);
        CHECK(parser.error().errc() == parsing_errc::expected_list_value_or_end);
    }
    SECTION("error - missing end - dict") {
        auto r = parser.parse(consumer, error_missing_end_dict);
        REQUIRE_FALSE(r);
        CHECK(parser.error().errc() == parsing_errc::expected_dict_key_or_end);
    }
    SECTION("error - missing value") {
        auto r = parser.parse(consumer, error_missing_value);
        REQUIRE_FALSE(r);
        CHECK(parser.error().errc() == parsing_errc::expected_value);
    }
    SECTION("error - missing dict value") {
        auto r = parser.parse(consumer, error_missing_dict_value);
        REQUIRE_FALSE(r);
        CHECK(parser.error().errc() == parsing_errc::expected_dict_value);
    }
    SECTION("error - missing list value") {
        auto r = parser.parse(consumer, error_missing_list_value_or_end);
        REQUIRE_FALSE(r);
        CHECK(parser.error().errc() == parsing_errc::expected_list_value_or_end);
    }
    SECTION("error - error_missing_dict_key_or_end") {
        auto r = parser.parse(consumer, error_missing_dict_key_or_end);
        CHECK_FALSE(r);
        CHECK(parser.error().errc() == parsing_errc::expected_dict_key_or_end);
    }
}


