#pragma once

#include "Common.h"


class EventStream 
{
private:
    std::istream& is;
    std::deque<std::int8_t> pending;
    std::int32_t columnIdx;

private:
    std::int32_t nextDetail()
    {
        if (pending.empty())
        {
            return is.get();
        }
        std::int32_t ret = pending.front();
        pending.pop_front();
        return ret;
    }

public:
    EventStream(std::istream& is) : is(is), pending(), columnIdx(0){}

    std::int32_t getColumnIdx() const
    {
        return columnIdx;
    }

    bool isEof() const
    {
        return is.eof() || is.peek() == EOF;
    }

    std::int8_t next()
    {
        std::int32_t ch = nextDetail();
        if (ch == '\n')
        {
            columnIdx = 0;
        }
        else
        {
            ++columnIdx;
        }
        return ch;
    }

    std::int8_t peek(std::int32_t idx = 0)
    {
        while (pending.size() <= idx)
        {
            pending.emplace_back(is.get());
        }
        return pending[idx];
    }

    void skipWhiteSpaces()
    {
        while (true)
        {
            switch (peek()) 
            {
            case ' ':
            case '\n':
                next();
                break;

            default:
                return;
            }
        }
    }
};


//YAMLEvent aka inner parser
class YAMLEvent
{
private:
    EventStream& eventStream;
    std::vector<EVENT_TYPE::event> events;
    std::int32_t indent, simpleKeyColumn;
    std::vector<std::int32_t> indents;

private:
    bool isWhiteSpace(std::uint8_t byte)
    {
        if (byte == ' ' || byte == '\n')
        {
            return true;
        }
        return false;
    }

    EVENT_TYPE::YAML_PLAIN_TEXT peekPlainText()
    {
        auto ss = std::stringstream{};
        while (true) 
        {
            const auto& data = eventStream.peek();
            if (data == '\n' || data == EOF || (data == ':' && isWhiteSpace(eventStream.peek(1))))
            {
                break;
            }
            ss << static_cast<std::uint8_t>(eventStream.next());
        }
        return EVENT_TYPE::YAML_PLAIN_TEXT { ss.str() };
    }

    template <class eventType>
    void rollIndent(std::int32_t column)
    {
        if (indent < column) 
        {
            events.push_back(eventType{});
            indents.push_back(indent);
            indent = column;
        }
    }

    void unrollIndent(std::int32_t column)
    {
        while (indent > column) 
        {
            indent = indents.back();
            indents.pop_back();
            events.push_back(EVENT_TYPE::YAML_BLOCK_END{});
        }
    }

    void parseEvents()
    {
        while (true) 
        {
            eventStream.skipWhiteSpaces();
            unrollIndent(eventStream.getColumnIdx());
            std::int8_t byte = eventStream.peek();

            if (byte == EOF)
            {
                break;
            }
            else if (byte == '-')
            {
                if (!isWhiteSpace(eventStream.peek(1)))
                {
                    break;
                }
                rollIndent<EVENT_TYPE::YAML_BLOCK_START>(eventStream.getColumnIdx());
                eventStream.next();
                events.push_back(EVENT_TYPE::YAML_BLOCK_ENTRY{});
                continue;
            }
            else if (':')
            {
                if (!isWhiteSpace(eventStream.peek(1)))
                {
                    break;
                }
                auto key = events.back();
                events.pop_back();
                rollIndent<EVENT_TYPE::YAML_MAP_START>(simpleKeyColumn);
                eventStream.next();
                events.emplace_back(EVENT_TYPE::YAML_INNER_KEY{});
                events.emplace_back(key);
                events.emplace_back(EVENT_TYPE::YAML_INNER_VALUE{});
                continue;
            }
            simpleKeyColumn = eventStream.getColumnIdx();
            events.emplace_back(EVENT_TYPE::YAML_PLAIN_TEXT{});
        }

        unrollIndent(-1);
        events.emplace_back(EVENT_TYPE::YAML_EOF{});
    }

public:
    YAMLEvent(EventStream& eventStream) : eventStream(eventStream), indent(-1), simpleKeyColumn(-1)
    {
        parseEvents();
    }

    const std::vector<EVENT_TYPE::event>& get() const
    {
        return events;
    }
};
