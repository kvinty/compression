# About
This project is a compression library. Its main feature is random-access
to encoded data, therefore it is aimed to compete with femtozip.

# Algorithms
* huffman  
The optimal prefix coding algorithm.
* dictionary  
Developed algorithm based on using suffix automaton.
It finds substrings (starting from an empty string and increasing its length)
that occur frequently (using heuristic) and saves them in dictionary.
While encoding, these thrings are replaced by three bytes 0LH
where LH is the index of saved substring (maximum possible quantity is 2 ** 16).
* union  
A simple wrapper for two previous codecs allowing to receive sligtly better
compression ratio (but works longer).
* tester  
A simple program demonstrating how these libraries work

# How to use
1. Download test data from https://yadi.sk/d/AXBvdA6MnabDe and move it
to the directory containing this file.
2. Run `./test <file>`.
Optionally, you can explicitly specify the compiler:
`CXX=g++ ./test <file>`. GCC 5.3 and Clang 3.8 are known to work.

# Important!
Tester uses dictionary_codec by default. If you want to use
union_codec instead, modify line 55 of tester/tester.cpp accordingly.

# Going deeper
You can read more about suffix automata here:
* http://codeforces.com/blog/entry/20861
* http://codeforces.com/blog/entry/22420
* http://e-maxx.ru/algo/suffix_automata

About Huffman coding:
* https://en.wikipedia.org/wiki/Huffman_coding
* https://fastcompression.blogspot.ru/2015/07/huffman-revisited-part-1.html

# Copyright information
All source files are under 2-clause BSD (see LICENSE for more details).
