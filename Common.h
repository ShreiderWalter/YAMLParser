#pragma once

#include <algorithm>
#include <cassert>
#include <deque>
#include <memory>
#include <optional>
#include <sstream>
#include <unordered_map>
#include <variant>
#include <vector>
#include <cstdint>


template <class... Ts>
struct VariantsPool : Ts...
{
    using Ts::operator()...;
};

template <class... Ts>
VariantsPool(Ts...)->VariantsPool<Ts...>;

template <class Variant, class... Visitor>
auto matchWith(Variant&& var, Visitor&& ... visitors)
{
    return std::visit(VariantsPool{ visitors... }, var);
}

//Parsing event types written below
namespace EVENT_TYPE
{
    struct YAML_EOF {};
    struct YAML_INNER_KEY {};
    struct YAML_INNER_VALUE {};
    struct YAML_MAP_START {};
    struct YAML_BLOCK_START {};
    struct YAML_BLOCK_END {};
    struct YAML_BLOCK_ENTRY {};
    struct YAML_PLAIN_TEXT 
    {
        std::string text;
    };

    using event = std::variant<YAML_EOF, YAML_INNER_KEY, YAML_INNER_VALUE, YAML_MAP_START, YAML_BLOCK_START, YAML_BLOCK_END, YAML_BLOCK_ENTRY, YAML_PLAIN_TEXT>;
};