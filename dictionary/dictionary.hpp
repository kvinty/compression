#pragma once

#include <algorithm>
#include <array>
#include <cstdint>
#include <experimental/string_view>
#include <map>
#include <string>
#include <utility>
#include <vector>

#include "suffix_automaton.hpp"
#include "trie.hpp"

using byte = unsigned char;

namespace Codecs
{
class dictionary_codec
{
protected:
    struct good_string
    {
        std::string data;
        size_t count, index;
        bool useful;

        good_string(std::string data_init,
        size_t count_init, size_t index_init, bool useful_init) noexcept;
    };

    trie saved;
    std::array<std::string, 1 << 16> table;
    mutable std::string ready_save;

    void insert_escape(std::string &s) const noexcept;
    void delete_escape(std::string &s) const noexcept;

public:
    void learn(const std::vector<std::experimental::string_view> &v) noexcept;
    std::string save() const noexcept;
    void load(const std::experimental::string_view &s) noexcept;
    size_t sample_size(size_t records) const noexcept;
    void reset() noexcept;
    void encode(std::string &encoded,
    const std::experimental::string_view &raw) const noexcept;
    void decode(std::string &raw,
    const std::experimental::string_view &encoded) const noexcept;
};
}
