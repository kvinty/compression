#pragma once

#include <cstdint>
#include <map>
#include <string>
#include <utility>
#include <vector>

using byte = unsigned char;

/* ESCAPE is an escape symbol that is used for inserting a substring.
 * There should be no ESCAPE bytes in the raw string when dictionary_codec
 * starts to encode it. Therefore, we should replace it. We use
 * ESCAPE_H1 + ESCAPE_H1 to replace ESCAPE and
 * ESCAPE_H1 + ESCAPE_H2 instead of ESCAPE_H1.
 * After decoding, we restore these bytes.
 * For simplicity, these bytes should meet the condition:
 * ESCAPE_H2 = ESCAPE_H1 + 1 = ESCAPE + 2
 */
static constexpr byte ESCAPE = 1;
static constexpr byte ESCAPE_H1 = 2;
static constexpr byte ESCAPE_H2 = 3;

class trie
{
protected:
    struct node
    {
        bool is_terminal;
        uint16_t escape;
        std::map<byte, size_t> next;

        node() noexcept;
    };

    std::vector<node> data;

    std::pair<size_t, uint16_t> find(const std::string &s, size_t start)
    const noexcept;

public:
    void insert(const std::pair<std::string, uint16_t> &s) noexcept;
    trie() noexcept;
    std::string encode(const std::string &s) const noexcept;
    void reset() noexcept;
};
