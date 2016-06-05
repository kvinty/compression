#include "trie.hpp"

trie::node::node() noexcept
: is_terminal(false)
, escape(0)
{
    next.fill(INF);
}

// Returns length of the longest found string and its number
std::pair<size_t, uint16_t> trie::find(const std::string &s, size_t start)
const noexcept
{
    size_t current = 0, longest = 0, steps = 0;
    uint16_t escape = 0;
    for (; start != s.size(); ++start)
    {
        size_t add = data[current].next[static_cast<byte>(s[start])];
        if (add != INF)
        {
            current = add;
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
        size_t add = data[current].next[c];
        if (add != INF)
            current = add;
        else
        {
            data.push_back(node());
            size_t last = data.end() - data.begin() - 1;
            data[current].next[c] = last;
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

void trie::encode(const std::string &s, std::string &res) const noexcept
{
    size_t n = s.size(), current = 0;
    res.clear();
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
}

void trie::reset() noexcept
{
    data.clear();
    data.resize(1);
}
