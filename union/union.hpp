#pragma once

#include <algorithm>
#include <climits>
#include <experimental/algorithm>
#include <random>

#include "dictionary.hpp"
#include "huffman.hpp"

namespace Codecs
{
class union_codec
{
protected:
    dictionary_codec dictionary;
    huffman_codec huffman;
    mutable std::string ready_save;

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
