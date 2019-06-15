#pragma once

#include "Common.h"


class YAMLEncoding
{
public:
    using dataList = std::vector<YAMLEncoding>;
    using dataMap = std::unordered_map<std::string, YAMLEncoding>;
    using encodingType = std::variant<std::int32_t, std::string, std::shared_ptr<dataList>, std::shared_ptr<dataMap>>;

private:
    encodingType value;

public:
    YAMLEncoding() : value() {}
    YAMLEncoding(std::int32_t value) : value(value) {}
    YAMLEncoding(const std::string& value) : value(std::move(value)) {}
    YAMLEncoding(const dataList& value) : value(std::make_shared<dataList>(value)) {}
    YAMLEncoding(const dataMap& value) : value(std::make_shared<dataMap>(value)) {}

public:
    template<class T>
    const T& get() const
    {
        return std::get<T>(value);
    }

    const YAMLEncoding& operator[] (std::int32_t idx) const;
    const YAMLEncoding& operator[] (const std::string& key) const;

    template<class... Visitors>
    auto visit(Visitors&& ... visitors)
    {
        auto variantMatched = VariantsPool{ visitors... };
        return ::matchWith(
            value,
            [&](int value) { return variantMatched(value); },
            [&](const std::string& value) { return variantMatched(value); },
            [&](const std::shared_ptr<dataList>& value) {return variantMatched(*value); },
            [&](const std::shared_ptr<dataMap>& value) {return variantMatched(*value); }
        );
    }
};

template<>
const YAMLEncoding::dataList& YAMLEncoding::get() const
{
    return *std::get<std::shared_ptr<YAMLEncoding::dataList>>(value);
}

template<>
const YAMLEncoding::dataMap& YAMLEncoding::get() const
{
    return *std::get<std::shared_ptr<YAMLEncoding::dataMap>>(value);
}

const YAMLEncoding& YAMLEncoding::operator[] (std::int32_t idx) const
{
    return get<YAMLEncoding::dataList>().at(idx);
}

const YAMLEncoding& YAMLEncoding::operator[] (const std::string& key) const
{
    return get<YAMLEncoding::dataMap>().at(key);
}