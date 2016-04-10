#include <experimental/string_view>
#include <iostream>

#include "huffman.hpp"

int main()
{
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);
    Codecs::HuffmanCodec zipper;
    std::string tmp, sum, middle, output;
    std::vector<std::string> input;
    std::vector<std::experimental::string_view> input_view(input.size());
    while (!std::cin.eof())
    {
        std::getline(std::cin, tmp);
        if (!std::cin.eof())
            input.push_back(tmp);
    }
    for (const std::string &i : input)
        input_view.push_back(i);
    zipper.learn(input_view);
    for (const std::string &i : input)
    {
        zipper.encode(middle, i);
        zipper.decode(output, middle);
        std::cout << output << std::endl;
    }
    zipper.reset();
    return 0;
}
