#include "suffix_automaton.hpp"

suffix_automaton::state::state(size_t length_init = 0, size_t link_init = 0,
size_t count_init = 0, bool is_cloned_init = false,
std::map<byte, size_t> next_init = std::map<byte, size_t>()) noexcept
: length(length_init)
, link(link_init)
, count(count_init)
, is_cloned(is_cloned_init)
, next(next_init)
{}

void suffix_automaton::append(byte c) noexcept
{
    size_t current = size++, p = last;
    states[current].length = states[last].length + 1;
    states[current].is_cloned = false;
    while (p != infinity && !states[p].next.count(c))
    {
        states[p].next[c] = current;
        p = states[p].link;
    }
    if (p == infinity)
        states[current].link = 0;
    else
    {
        size_t q = states[p].next[c];
        if (states[p].length + 1 == states[q].length)
            states[current].link = q;
        else
        {
            size_t clone = size++;
            states[clone] = state(states[p].length + 1, states[q].link, 0,
            true, states[q].next);
            while (p != infinity && states[p].next[c] == q)
            {
                states[p].next[c] = clone;
                p = states[p].link;
            }
            states[q].link = states[current].link = clone;
        }
    }
    last = current;
}

suffix_automaton::suffix_automaton(const std::string &s) noexcept
{
    states.resize(2 * s.size() + 1);
    states[0].length = 0;
    states[0].link = infinity;
    states[0].is_cloned = true;  // Required, not a bug
    for (byte i : s)
        append(i);
    for (state &i : states)
        i.count = static_cast<size_t>(!i.is_cloned);
    std::vector<std::vector<size_t>> inverse(s.size() + 1);
    for (size_t i = 0; i != states.size(); ++i)
        inverse[states[i].length].push_back(i);
    for (size_t i = s.size(); i != 0; --i)
        for (size_t j : inverse[i])
            states[states[j].link].count += states[j].count;
}

// Gets the string and index in automaton of it without last symbol
// Returns the number of occurrences and index of the whole string in automaton
std::pair<size_t, size_t> suffix_automaton::count
(const std::pair<std::string, size_t> &s) const noexcept
{
    size_t index = s.second;
    std::map<byte, size_t>::const_iterator next_state =
    states[index].next.find(static_cast<byte>(s.first.back()));
    if (next_state != states[index].next.end())
        index = next_state->second;
    else
        return {0, 0};
    return {states[index].count, index};
}
