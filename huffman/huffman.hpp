#pragma once

#include <array>
#include <cstdint>
#include <experimental/string_view>
#include <queue>
#include <stack>
#include <string>
#include <utility>
#include <vector>

namespace Codecs
{
    class HuffmanCodec
    {
    protected:
        static constexpr size_t LOG_CHARBIT = 3;
        static constexpr size_t CHARBIT = 1 << LOG_CHARBIT;
        static constexpr size_t SYMBOLS = 1 << CHARBIT;
        static constexpr size_t BYTES = SYMBOLS / CHARBIT;

        static constexpr uint8_t LEAF = 0;
        static constexpr uint8_t LEFT = 1;
        static constexpr uint8_t RIGHT = 2;
        static constexpr uint8_t UP = 3;

        using representation = std::pair<std::array<uint8_t, BYTES>, uint8_t>;

        struct Node
        {
            size_t frequency;
            Node *left;
            Node *right;
            uint8_t letter;
            bool is_leaf;

            explicit Node(size_t init_freq, uint8_t letter) noexcept
            : frequency(init_freq)
            , left(nullptr)
            , right(nullptr)
            , letter(letter)
            , is_leaf(true)
            {}

            explicit Node(Node *left, Node *right) noexcept
            : frequency(0)
            , left(left)
            , right(right)
            , letter(0)
            , is_leaf(false)
            {
                if (left)
                    frequency += left->frequency;
                if (right)
                    frequency += right->frequency;
            }
        };

        Node *root = nullptr;
        std::array<size_t, SYMBOLS> frequencies = {{0}};
        std::array<representation, SYMBOLS> mapping;
        std::vector<Node *> allocated_memory;

        void generate_map(const Node *v, const representation &prefix) noexcept;
        void dfs(const Node *ptr, std::string &str) const noexcept;
        inline void encode_byte(uint8_t byte, uint8_t size, std::string &encoded, size_t &bits_written) const noexcept;

    public:
        void learn(const std::vector<std::experimental::string_view> &vec) noexcept;
        std::string save() const noexcept;
        void load(const std::experimental::string_view &str) noexcept;
        size_t sample_size(size_t records) const noexcept;
        void reset() noexcept;
        void encode(std::string &encoded, const std::experimental::string_view &raw) const noexcept;
        void decode(std::string &raw, const std::experimental::string_view &encoded) const noexcept;
    };
}
