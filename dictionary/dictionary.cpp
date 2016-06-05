#include "dictionary.hpp"

namespace Codecs
{
dictionary_codec::good_string::good_string(std::string data_init,
size_t count_init, size_t index_init, bool useful_init) noexcept
: data(data_init)
, count(count_init)
, index(index_init)
, useful(useful_init)
{}

void dictionary_codec::insert_escape(std::string &s) const noexcept
{
    std::string tmp;
    for (byte c : s)
    {
        if (c == ESCAPE || c == ESCAPE_H1)
        {
            tmp.push_back(ESCAPE_H1);
            tmp.push_back(static_cast<byte>(c + 1));
        }
        else
            tmp.push_back(c);
    }
    s = tmp;
}

void dictionary_codec::delete_escape(std::string &s) const noexcept
{
    std::string tmp;
    for (size_t i = 0; i != s.size(); ++i)
    {
        if (s[i] == ESCAPE_H1)
        {
            ++i;
            tmp.push_back(static_cast<byte>(s[i] - 1));
        }
        else
            tmp.push_back(s[i]);
    }
    s = tmp;
}

void dictionary_codec::learn
(const std::vector<std::experimental::string_view> &v) noexcept
{
    std::string concat = v[0].to_string();
    for (size_t i = 1; i != v.size(); ++i)
    {
        concat.push_back(ESCAPE_H1);
        concat += v[i].to_string();
    }
    insert_escape(concat);

    suffix_automaton sa(concat);

    constexpr size_t maximum = 1000;
    std::array<std::vector<good_string>, maximum + 1> strings;
    strings[0].push_back(good_string(std::string(), maximum + 1, 0, false));
    for (size_t i = 1; i <= maximum; ++i)
        for (good_string &s : strings[i - 1])
            for (unsigned c = 0; c != 256; ++c)
            {
                std::string appended = s.data;
                appended.push_back(static_cast<byte>(c));
                std::pair<size_t, size_t> info = sa.count({appended, s.index});
                if (info.first >= 8)
                {
                    strings[i].push_back(good_string(appended, info.first,
                    info.second, true));
                    if (info.first >= (9 * s.count / 10))
                        s.useful = false;
                    if (info.first <= (1 * s.count / 10))
                        strings[i].back().useful = false;
                }
            }

    size_t counter = 0;
    for (const std::vector<good_string> &i : strings)
        for (std::vector<good_string>::const_iterator j = i.begin();
        j != i.end(); ++j)
            if (j->data.size() >= 4 && j->useful && counter < (1 << 16))
            {
                saved.insert({j->data, counter});
                table[counter] = j->data;
                ++counter;
            }
}

std::string dictionary_codec::save() const noexcept
{
    if (ready_save.empty())
    {
        std::string result;
        for (const std::string &s : table)
            if (!s.empty())
            {
                size_t len = s.size();
                result += static_cast<byte>(len);
                result += static_cast<byte>(len >> 8);
                result += s;
            }
        ready_save = result;
        return result;
    }
    else
        return ready_save;
}

void dictionary_codec::load(const std::experimental::string_view &s) noexcept
{
    reset();
    ready_save = s.to_string();
    size_t pos = 0, i = 0;
    while (pos != s.size())
    {
        size_t len = static_cast<byte>(s[pos]) +
                    (static_cast<byte>(s[pos + 1]) << 8);
        pos += 2;
        table[i] = std::string(s.data() + pos, len);
        pos += len;
        ++i;
    }
    for (std::array<std::string, 1 << 16>::const_iterator t = table.begin();
    t != table.end(); ++t)
        if (!t->empty())
            saved.insert({*t, t - table.begin()});
}

size_t dictionary_codec::sample_size(size_t records) const noexcept
{
    return std::min(records,
    std::max(static_cast<size_t>(500), records / 1000));
}

void dictionary_codec::reset() noexcept
{
    saved.reset();
    for (std::string &s : table)
    {
        s.clear();
        s.shrink_to_fit();
    }
    ready_save.clear();
}

void dictionary_codec::encode(std::string &encoded,
const std::experimental::string_view &raw) const noexcept
{
    std::string tmp = raw.to_string();
    insert_escape(tmp);
    saved.encode(tmp, encoded);
}

void dictionary_codec::decode(std::string &raw,
const std::experimental::string_view &encoded) const noexcept
{
    raw.clear();
    size_t current = 0;
    while (current != encoded.size())
    {
        if (encoded[current] != ESCAPE)
        {
            raw.push_back(static_cast<byte>(encoded[current]));
            ++current;
        }
        else
        {
            raw += table[
            static_cast<uint16_t>(static_cast<byte>(encoded[current + 1])) +
            static_cast<uint16_t>((static_cast<byte>(
            encoded[current + 2])) << 8)];
            current += 3;
        }
    }
    delete_escape(raw);
}
}
