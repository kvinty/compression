#include "trie.hpp"

trie::node::node() noexcept
: is_terminal(false)
, escape(0)
{}

// Returns length of the longest found string and its number
std::pair<size_t, uint16_t> trie::find(const std::string &s, size_t start)
const noexcept
{
    size_t current = 0, longest = 0, steps = 0;
    uint16_t escape = 0;
    for (; start != s.size(); ++start)
    {
        std::map<byte, size_t>::const_iterator add =
        data[current].next.find(static_cast<byte>(s[start]));
        if (add != data[current].next.end())
        {
            current = add->second;
            ++steps;
            if (data[current].is_terminal)
            {
                longest = steps;
                escape = data[current].escape;
            }
        }
        else
            break;
    }
    return {longest, escape};
}

void trie::insert(const std::pair<std::string, uint16_t> &s) noexcept
{
    size_t current = 0;
    for (byte c : s.first)
    {
        std::map<byte, size_t>::const_iterator add =
        data[current].next.find(c);
        if (add != data[current].next.end())
            current = add->second;
        else
        {
            data.push_back(node());
            size_t last = data.end() - data.begin() - 1;
            data[current].next.insert({c, last});
            current = last;
        }
    }
    data[current].is_terminal = true;
    data[current].escape = s.second;
}

trie::trie() noexcept
{
    data.resize(1);
}

std::string trie::encode(const std::string &s) const noexcept
{
    std::string res;
    size_t n = s.size(), current = 0;
    while (current != n)
    {
        std::pair<size_t, uint16_t> search = find(s, current);
        if (search.first != 0 && current + search.first <= n)
        {
            res.push_back(ESCAPE);
            res.push_back(static_cast<byte>(search.second & ((1 << 8) - 1)));
            res.push_back(static_cast<byte>(search.second >> 8));
            current += search.first;
        }
        else
            res.push_back(static_cast<byte>(s[current++]));
    }
    return res;
}

void trie::reset() noexcept
{
    data.clear();
    data.resize(1);
}
