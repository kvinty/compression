#include "huffman.hpp"
#include <iostream>
namespace Codecs
{
huffman_codec::node::node(size_t init_freq, byte letter) noexcept
: frequency(init_freq)
, left(nullptr)
, right(nullptr)
, letter(letter)
, is_leaf(true)
{}

huffman_codec::node::node(node *left, node *right) noexcept
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

void huffman_codec::generate_map(const huffman_codec::node *v,
const huffman_codec::representation &prefix) noexcept
{
    if (v->is_leaf)
        mapping[v->letter] = prefix;
    else
    {
        if (v->left)
        {
            representation l_prefix = prefix;
            ++l_prefix.second;
            generate_map(v->left, l_prefix);
        }
        if (v->right)
        {
            representation r_prefix = prefix;
            r_prefix.first[r_prefix.second >> LOG_CHAR_BIT] |=
            static_cast<byte>(1 << (r_prefix.second & (CHAR_BIT - 1)));
            ++r_prefix.second;
            generate_map(v->right, r_prefix);
        }
    }
}

void huffman_codec::dfs(const huffman_codec::node *ptr,
std::string &str) const noexcept
{
    if (ptr->is_leaf)
    {
        str += LEAF;
        str += ptr->letter;
    }
    else
    {
        if (ptr->left)
        {
            str += LEFT;
            dfs(ptr->left, str);
            str += UP;
        }
        if (ptr->right)
        {
            str += RIGHT;
            dfs(ptr->right, str);
            str += UP;
        }
    }
}

inline void huffman_codec::encode_byte(byte c, byte size,
std::string &encoded, size_t &bits_written) const noexcept
{
    encoded.back() |= static_cast<byte>(c << bits_written);
    if (bits_written + size <= CHAR_BIT)
        bits_written += size;
    else
    {
        encoded.push_back(0);
        encoded.back() |= static_cast<byte>(c >> (CHAR_BIT - bits_written));
        bits_written = size - (CHAR_BIT - bits_written);
    }
}

bool huffman_codec::node_comparator::operator()(const node *l, const node *r)
const noexcept
{
    return l->frequency > r->frequency;
}

void huffman_codec::learn(const std::vector<std::experimental::string_view> &v)
noexcept
{
    for (const std::experimental::string_view &str : v)
        for (byte i : str)
            ++frequencies[i];

    std::priority_queue<node *, std::vector<node *>, node_comparator> trees;
    for (size_t i = 0; i != SYMBOLS; ++i)
    {
        node *tmp = new node(frequencies[i], static_cast<byte>(i));
        allocated_memory.push_back(tmp);
        trees.push(tmp);
    }

    if (trees.size() == 0)
        return;

    if (trees.size() == 1)
    {
        root = new node(trees.top(), nullptr);
        allocated_memory.push_back(root);
    }
    else
    {
        while (trees.size() > 1)
        {
            node *l_child = trees.top();
            trees.pop();
            node *r_child = trees.top();
            trees.pop();
            node *parent = new node(l_child, r_child);
            allocated_memory.push_back(parent);
            trees.push(parent);
        }
        root = trees.top();
    }
    generate_map(root, representation());
}

std::string huffman_codec::save() const noexcept
{
    if (ready_save.empty())
    {
        std::string result;
        if (root)
            dfs(root, result);
        ready_save = result;
        return result;
    }
    else
        return ready_save;
}

void huffman_codec::load(const std::experimental::string_view &s) noexcept
{
    reset();
    ready_save = s.to_string();
    root = new huffman_codec::node(nullptr, nullptr);
    allocated_memory.push_back(root);
    std::stack<huffman_codec::node *> nodes;
    nodes.push(root);
    size_t i = 0;
    huffman_codec::node *vertex = root;
    while (i != s.size())
    {
        if (s[i] == LEAF)
        {
            ++i;
            vertex->letter = s[i];
            vertex->is_leaf = true;
        }
        else if (s[i] == LEFT)
        {
            huffman_codec::node *v = new huffman_codec::node(nullptr, nullptr);
            allocated_memory.push_back(v);
            vertex->left = v;
            nodes.push(vertex);
            vertex = v;
        }
        else if (s[i] == RIGHT)
        {
            huffman_codec::node *v = new huffman_codec::node(nullptr, nullptr);
            allocated_memory.push_back(v);
            vertex->right = v;
            nodes.push(vertex);
            vertex = v;
        }
        else
        {
            vertex = nodes.top();
            nodes.pop();
        }
        ++i;
    }
    generate_map(root, representation());
}

size_t huffman_codec::sample_size(size_t records) const noexcept
{
    return std::min(records,
    std::max(static_cast<size_t>(1000), records / 10));
}

void huffman_codec::reset() noexcept
{
    for (huffman_codec::node *&i : allocated_memory)
        delete i;
    allocated_memory.clear();
    ready_save.clear();
}

void huffman_codec::encode(std::string &encoded,
const std::experimental::string_view &raw) const noexcept
{
    encoded.assign(1, 0);
    size_t bits_written = 0;
    for (byte i : raw)
    {
        representation code = mapping[i];
        size_t j;
        for (j = 0; j != static_cast<size_t>(code.second >> LOG_CHAR_BIT); ++j)
            encode_byte(code.first[j], CHAR_BIT, encoded, bits_written);
        encode_byte(code.first[j], static_cast<byte>
        (code.second & (CHAR_BIT - 1)), encoded, bits_written);
    }
    encoded.push_back(static_cast<byte>(CHAR_BIT - bits_written));
}

void huffman_codec::decode(std::string &raw,
const std::experimental::string_view &encoded) const noexcept
{
    raw.clear();
    size_t bits_read = 0;
    std::experimental::string_view::const_iterator i = encoded.begin();
    while (i < encoded.end() - 2 ||
    (i == encoded.end() - 2 && bits_read + encoded.back() < CHAR_BIT))
    {
        huffman_codec::node *ptr = root;
        while (true)
        {
            if (ptr->is_leaf)
            {
                raw.push_back(ptr->letter);
                break;
            }
            if (bits_read != CHAR_BIT)
            {
                if (*i & (1 << bits_read))
                    ptr = ptr->right;
                else
                    ptr = ptr->left;
                ++bits_read;
            }
            else
            {
                ++i;
                bits_read = 0;
            }
        }
    }
}

huffman_codec::~huffman_codec() noexcept
{
    reset();
}
}
