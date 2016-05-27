#include "union.hpp"

namespace Codecs
{
void union_codec::learn(const std::vector<std::experimental::string_view> &v)
noexcept
{
    std::vector<std::experimental::string_view> dictionary_sample;
    std::experimental::sample(v.begin(), v.end(),
    std::back_inserter(dictionary_sample), dictionary.sample_size(v.size()),
    std::mt19937(std::random_device()()));

    dictionary.learn(dictionary_sample);

    std::vector<std::experimental::string_view> huffman_sample;
    std::experimental::sample(v.begin(), v.end(),
    std::back_inserter(huffman_sample), huffman.sample_size(v.size()),
    std::mt19937(std::random_device()()));
    size_t size = huffman_sample.size();

    std::vector<std::string> encoded_sample_vector(size);
    std::vector<std::experimental::string_view> encoded_sample(size);

    for (size_t i = 0; i != size; ++i)
    {
        dictionary.encode(encoded_sample_vector[i], huffman_sample[i]);
        encoded_sample[i] = encoded_sample_vector[i];
    }
    huffman.learn(encoded_sample);
}

std::string union_codec::save() const noexcept
{
    if (ready_save.empty())
    {
        std::string result;
        std::string dictionary_save = dictionary.save();
        size_t size = dictionary_save.size();
        for (size_t i = 0; i != 4; ++i)
            result.push_back(static_cast<byte>
            ((size >> (i * CHAR_BIT)) & ((1 << CHAR_BIT) - 1)));
        result += dictionary_save;
        result += huffman.save();
        ready_save = result;
        return result;
    }
    else
        return ready_save;
}

void union_codec::load(const std::experimental::string_view &s) noexcept
{
    size_t size = 0;
    for (size_t i = 0; i != 4; ++i)
        size += static_cast<byte>(s[i]) * (1 << (i * CHAR_BIT));
    std::string str = s.to_string();
    dictionary.load(str.substr(4, size));
    huffman.load(str.substr(4 + size));
}

size_t union_codec::sample_size(size_t records) const noexcept
{
    return records;
}

void union_codec::reset() noexcept
{
    dictionary.reset();
    huffman.reset();
    ready_save.clear();
}

void union_codec::encode(std::string &encoded,
const std::experimental::string_view &raw) const noexcept
{
    std::string middle;
    dictionary.encode(middle, raw);
    huffman.encode(encoded, middle);
}

void union_codec::decode(std::string &raw,
const std::experimental::string_view &encoded) const noexcept
{
    std::string middle;
    huffman.decode(middle, encoded);
    dictionary.decode(raw, middle);
}
}
