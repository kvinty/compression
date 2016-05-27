#pragma once

#include <algorithm>
#include <array>
#include <climits>
#include <experimental/string_view>
#include <queue>
#include <stack>
#include <string>
#include <utility>
#include <vector>

using byte = unsigned char;

namespace Codecs
{
class huffman_codec
{
protected:
    static constexpr size_t LOG_CHAR_BIT = 3;
    static constexpr size_t SYMBOLS = static_cast<size_t>(1 << CHAR_BIT);
    static constexpr size_t BYTES = SYMBOLS / CHAR_BIT;

    static constexpr byte LEAF = 0;
    static constexpr byte LEFT = 1;
    static constexpr byte RIGHT = 2;
    static constexpr byte UP = 3;

    using representation = std::pair<std::array<byte, BYTES>, byte>;

    struct node
    {
        size_t frequency;
        node *left;
        node *right;
        byte letter;
        bool is_leaf;

        node(size_t init_freq, byte letter) noexcept;
        node(node *left, node *right) noexcept;
    };

    struct node_comparator
    {
        bool operator()(const node *l, const node *r) const noexcept;
    };

    node *root = nullptr;
    std::array<size_t, SYMBOLS> frequencies = {{0}};
    std::array<representation, SYMBOLS> mapping;
    std::vector<node *> allocated_memory;
    mutable std::string ready_save;

    void generate_map(const node *v, const representation &prefix) noexcept;
    void dfs(const node *ptr, std::string &str) const noexcept;
    inline void encode_byte(byte c, byte size,
    std::string &encoded, size_t &bits_written) const noexcept;

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
    ~huffman_codec() noexcept;
};
}
