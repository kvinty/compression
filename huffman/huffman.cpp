#include "huffman.hpp"

using Codecs::HuffmanCodec;

void HuffmanCodec::generate_map(const HuffmanCodec::Node *v, const HuffmanCodec::representation &prefix) noexcept
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
            r_prefix.first[r_prefix.second >> LOG_CHARBIT] |=
            static_cast<uint8_t>(1 << (r_prefix.second & (CHARBIT - 1)));
            ++r_prefix.second;
            generate_map(v->right, r_prefix);
        }
    }
}

void HuffmanCodec::dfs(const HuffmanCodec::Node *ptr, std::string &str) const noexcept
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

inline void HuffmanCodec::encode_byte(uint8_t byte, uint8_t size, std::string &encoded, size_t &bits_written) const noexcept
{
    encoded.back() |= static_cast<uint8_t>(byte << bits_written);
    if (bits_written + size <= CHARBIT)
        bits_written += size;
    else
    {
        encoded.push_back(0);
        encoded.back() |= static_cast<uint8_t>(byte >> (CHARBIT - bits_written));
        bits_written = size - (CHARBIT - bits_written);
    }
}

void HuffmanCodec::learn(const std::vector<std::experimental::string_view> &vec) noexcept
{
    struct NodeComparator
    {
        bool operator()(const Node *l, const Node *r) const noexcept
        {
            return l->frequency > r->frequency;
        }
    };

    for (std::experimental::string_view str : vec)
        for (uint8_t i : str)
            ++frequencies[i];

    std::priority_queue<Node *, std::vector<Node *>, NodeComparator> trees;
    for (size_t i = 0; i != SYMBOLS; ++i)
        if (frequencies[i])
        {
            Node *tmp = new Node(frequencies[i], static_cast<uint8_t>(i));
            allocated_memory.push_back(tmp);
            trees.push(tmp);
        }

    if (trees.size() == 0)
        return;

    if (trees.size() == 1)
    {
        root = new Node(trees.top(), nullptr);
        allocated_memory.push_back(root);
    }
    else
    {
        while (trees.size() > 1)
        {
            Node *l_child = trees.top();
            trees.pop();
            Node *r_child = trees.top();
            trees.pop();
            Node *parent = new Node(l_child, r_child);
            allocated_memory.push_back(parent);
            trees.push(parent);
        }
        root = trees.top();
    }
    generate_map(root, representation());
}

std::string HuffmanCodec::save() const noexcept
{
    std::string res;
    if (root)
        dfs(root, res);
    return res;
}

void HuffmanCodec::load(const std::experimental::string_view &str) noexcept
{
    reset();
    root = new HuffmanCodec::Node(nullptr, nullptr);
    allocated_memory.push_back(root);
    std::stack<HuffmanCodec::Node *> nodes;
    nodes.push(root);
    size_t i = 0;
    HuffmanCodec::Node *vertex = root;
    while (i != str.size())
    {
        if (str[i] == LEAF)
        {
            ++i;
            vertex->letter = str[i];
            vertex->is_leaf = true;
        }
        else if (str[i] == LEFT)
        {
            HuffmanCodec::Node *v = new HuffmanCodec::Node(nullptr, nullptr);
            allocated_memory.push_back(v);
            vertex->left = v;
            nodes.push(vertex);
            vertex = v;
        }
        else if (str[i] == RIGHT)
        {
            HuffmanCodec::Node *v = new HuffmanCodec::Node(nullptr, nullptr);
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

size_t HuffmanCodec::sample_size(size_t records) const noexcept
{
    return records;
}

void HuffmanCodec::reset() noexcept
{
    for (HuffmanCodec::Node *i : allocated_memory)
        delete i;
    allocated_memory.clear();
}

void HuffmanCodec::encode(std::string &encoded, const std::experimental::string_view &raw) const noexcept
{
    encoded.assign(1, 0);
    size_t bits_written = 0;
    for (uint8_t i : raw)
    {
        representation code = mapping[i];
        size_t j;
        for (j = 0; j != static_cast<size_t>(code.second >> LOG_CHARBIT); ++j)
            encode_byte(code.first[j], CHARBIT, encoded, bits_written);
        encode_byte(code.first[j], code.second & (CHARBIT - 1), encoded, bits_written);
    }
    encoded.push_back(static_cast<uint8_t>(CHARBIT - bits_written));
}

void HuffmanCodec::decode(std::string &raw, const std::experimental::string_view &encoded) const noexcept
{
    raw.clear();
    size_t bits_read = 0;
    std::experimental::string_view::const_iterator i = std::begin(encoded);
    while (i < encoded.end() - 2 || (i == encoded.end() - 2 && bits_read + encoded.back() < CHARBIT))
    {
        HuffmanCodec::Node *ptr = root;
        while (true)
        {
            if (ptr->is_leaf)
            {
                raw.push_back(ptr->letter);
                break;
            }
            if (bits_read != CHARBIT)
            {
                bool c = (*i & (1 << bits_read));
                if (c)
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
