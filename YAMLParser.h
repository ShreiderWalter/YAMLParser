#pragma once


#include "YAMLEncoding.h"
#include "YAMLEvent.h"

class YAMLParser 
{
private:
    std::vector<EVENT_TYPE::event>::iterator eventsIter;
    YAMLEncoding value;

private:
    template <class eventType>
    std::optional<eventType> popIf()
    {
        auto ptk = std::get_if<eventType>(&*eventsIter);
        if (!ptk)
        {
            return std::nullopt;
        }
        ++eventsIter;
        return *ptk;
    }

    template <class eventType>
    eventType expect()
    {
        if (auto tk = popIf<eventType>())
        {
            return *tk;
        }
    }

    template <class eventType>
    std::optional<eventType> match()
    {
        auto ptk = std::get_if<eventType>(&*eventsIter);
        if (!ptk)
        {
            return std::nullopt;
        }
        return *ptk;
    }

    YAMLEncoding parseBlock()
    {
        if (popIf<EVENT_TYPE::YAML_BLOCK_START>()) 
        {
            std::vector<YAMLEncoding> src;
            while (popIf<EVENT_TYPE::YAML_BLOCK_ENTRY>())
            {
                src.push_back(parseBlockContent());
            }
            expect<EVENT_TYPE::YAML_BLOCK_END>();
            return YAMLEncoding(src);
        }

        if (popIf<EVENT_TYPE::YAML_MAP_START>()) 
        {
            std::unordered_map<std::string, YAMLEncoding> src;
            while (popIf<EVENT_TYPE::YAML_INNER_KEY>())
            {
                auto key = expect<EVENT_TYPE::YAML_PLAIN_TEXT>().text;
                expect<EVENT_TYPE::YAML_INNER_VALUE>();
                if (match<EVENT_TYPE::YAML_BLOCK_ENTRY>()) 
                {
                    std::vector<YAMLEncoding> lst;
                    while (popIf<EVENT_TYPE::YAML_BLOCK_ENTRY>())
                    {
                        lst.emplace_back(parseBlockContent());
                    }
                    src.emplace(key, lst);
                }
                else
                {
                    auto val = parseBlockContent();
                    src.emplace(key, val);
                }
            }
            expect<EVENT_TYPE::YAML_BLOCK_END>();
            return YAMLEncoding(src);
        }
    }

    YAMLEncoding parseBlockContent()
    {
        if (auto tk = popIf<EVENT_TYPE::YAML_PLAIN_TEXT>())
        {
            return YAMLEncoding(tk->text);
        }
        return parseBlock();
    }

    void parse()
    {
        value = parseBlockContent();
    }

public:
    YAMLParser(std::vector<EVENT_TYPE::event>::iterator eventsIter) : eventsIter(eventsIter)
    {
        parse();
    }

    const YAMLEncoding& get()
    {
        return value;
    }
};