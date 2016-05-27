#pragma once

#include <limits>
#include <map>
#include <string>
#include <utility>
#include <vector>

using byte = unsigned char;

class suffix_automaton
{
protected:
    struct state
    {
        size_t length, link, count;
        bool is_cloned;
        std::map<byte, size_t> next;

        state(size_t length_init, size_t link_init,
        size_t count_init, bool is_cloned_init,
        std::map<byte, size_t> next_init) noexcept;
    };

    static constexpr size_t infinity = std::numeric_limits<size_t>::max();
    size_t size = 1, last = 0;
    std::vector<state> states;

    void append(byte c) noexcept;

public:
    suffix_automaton(const std::string &s) noexcept;
    std::pair<size_t, size_t> count(const std::pair<std::string, size_t> &s)
    const noexcept;
};
