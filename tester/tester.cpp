#include <cstring>
#include <ctime>
#include <experimental/algorithm>
#include <fstream>
#include <iostream>

#include "union.hpp"

static void usage(std::ostream &out)
{
    out << "Usage: tester [--help] [--nl] [--le] <file>\n"
    << "Use should point not more than one of these options\n"
    << "nl: usual file with NL splitted lines\n"
    << "le: in each line: length (4 bytes in binary, little endian format)"
    << " of the string, then that string\n"
    << "The default mode is NL.\n";
}

int main(int argc, char *argv[])
{
    static constexpr uint8_t NL = 0;
    static constexpr uint8_t LE = 1;
    size_t read = 0, file_size = 0;
    uint8_t type = NL;
    uint32_t size;
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);
    if (argc != 2 && argc != 3)
    {
        usage(std::cerr);
        return 1;
    }
    if (argc == 2 && strcmp(argv[1], "--help") == 0)
    {
        usage(std::cout);
        return 0;
    }
    if (argc == 3)
    {
        if (strcmp(argv[1], "--help") == 0)
        {
            usage(std::cout);
            return 0;
        }
        else if (strcmp(argv[1], "--nl") == 0)
            type = NL;
        else if (strcmp(argv[1], "--le") == 0)
            type = LE;
        else
        {
            usage(std::cerr);
            return 1;
        }
    }
    Codecs::dictionary_codec zipper;
    std::string tmp,encoded, decoded;
    std::vector<std::string> input;
    std::vector<std::experimental::string_view> sample;
    std::ifstream in(argv[argc - 1], std::ios_base::binary);
    if (type == LE)
    {
        in.seekg(0,std::ios_base::end);
        file_size = static_cast<size_t>(in.tellg());
        in.seekg(0,std::ios_base::beg);
    }
    double start, finish;
    if (type == NL)
        while (!in.eof())
        {
            std::getline(in, tmp);
            if (!in.eof())
                input.push_back(tmp);
        }
    else
        while (read != file_size)
        {
            in.read(static_cast<char *>(static_cast<void *>(&size)), 4);
            tmp.resize(size);
            in.read(const_cast<char *>(tmp.data()), size);
            read += 4 + size;
            input.push_back(tmp);
        }
    start = clock();
    std::experimental::sample(input.begin(), input.end(),
    std::back_inserter(sample), zipper.sample_size(input.size()),
    std::mt19937(std::random_device()()));
    zipper.learn(sample);
    finish = clock();
    std::cout << "Train time: " << (finish - start) / CLOCKS_PER_SEC << '\n';
    std::string save = zipper.save();
    zipper.reset();
    zipper.load(save);
    start = clock();
    for (size_t i = 0; i != input.size(); ++i)
        zipper.encode(encoded, input[i]);
    finish = clock();
    std::cout << "Encode time: " << (finish - start) / CLOCKS_PER_SEC << '\n';
    start = clock();
    for (size_t i = 0; i != input.size(); ++i)
        zipper.decode(decoded, encoded);
    finish = clock();
    std::cout << "Decode time: " << (finish - start) / CLOCKS_PER_SEC << '\n';
    size_t raw_size = 0, encoded_size = 0;
    for (size_t i = 0; i != input.size(); ++i)
    {
        zipper.encode(encoded, input[i]);
        zipper.decode(decoded, encoded);
        raw_size += input[i].size();
        encoded_size += encoded.size();
        if (input[i] != decoded)
        {
            std::cout << "An error has happened :(\n";
            return 1;
        }
    }
    std::cout << "Raw text length: " << raw_size << std::endl <<
    "Encoded text length: " << encoded_size << std::endl << "Save size: "
    << save.size() << std::endl << "Compression ratio: "
    << 100 * (1 - (static_cast<double>(encoded_size) + save.size()) / raw_size)
    << "%" << std::endl;
    return 0;
}
