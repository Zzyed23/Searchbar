#include <iostream>
#include <fstream>
#include <unordered_map>
#include <string>
#include <vector>
#include <algorithm>
#include <unistd.h>

using namespace std;
int backtrackLevel = 0;
class TrieNode {
public:
    std::unordered_map<char, TrieNode*> children;
    bool is_end_of_word;

    TrieNode() : is_end_of_word(false) {}
};

class Trie {
public:
    Trie() {
        root = new TrieNode();
    }

    void insert(const std::string& word) {
        TrieNode* node = root;
        for (char ch : word) {
            if (node->children.find(ch) == node->children.end()) {
                node->children[ch] = new TrieNode();
            }
            node = node->children[ch];
        }
        node->is_end_of_word = true;
    }

    std::vector<std::string> findAutoCompleteOptions(const std::string& prefix) {
        std::vector<std::string> options;
        TrieNode* node = root;

        for (char ch : prefix) {
            if (node->children.find(ch) == node->children.end()) {
                backtrackAndCollectOptions(node, prefix, prefix, options, 0);
                break;            }
            node = node->children[ch];
        }

        traverseAndCollectOptions(node, prefix, options);

        if (options.size() > 3) {
            int n = 3;

            std::vector<std::string> x(options.end() - n, options.end());
            options = x;
            //options.resize(3);
        }

        return options;
    }

private:
    TrieNode* root;

    void traverseAndCollectOptions(TrieNode* node, std::string current, std::vector<std::string>& options) {

        if (node->is_end_of_word) {
            options.push_back(current);
        }

        for (const auto& pair : node->children) {
            traverseAndCollectOptions(pair.second, current + pair.first, options);
        }
    }
    void backtrackAndCollectOptions(TrieNode* node, std::string current, std::string prefix, std::vector<std::string>& options, int backtrackLevel) {

        if (node->is_end_of_word) {
            options.push_back(current);
        }

        for (const auto& pair : node->children) {
            std::string newCurrent = current + pair.first;
            backtrackAndCollectOptions(pair.second, newCurrent, prefix, options, backtrackLevel);
            backtrackLevel++;
        }

        if (backtrackLevel > 0) {
            string beginning = prefix.substr(0, prefix.length()-backtrackLevel);
            string end = current.substr(prefix.length());
            current = beginning + end;

        }
    }
};

Trie constructTrieFromFile(const std::string& file_name) {
    Trie trie;
    fstream new_file;
    char buffer[PATH_MAX];
    getcwd(buffer, sizeof(buffer));
    std::cout << "Current working directory: " << buffer << std::endl;
    new_file.open(  file_name, ios::in);

    if (new_file.is_open()) {
        string sa;
        while (getline(new_file, sa)) {
            //std::transform(sa.begin(), sa.end(), sa.begin(), ::tolower);
            const char* WhiteSpace = " \t\v\r\n";
            std::size_t start = sa.find_first_not_of(WhiteSpace);
            sa = sa.substr(start);
            trie.insert(sa);
        }
        new_file.close();
    }
    return trie;
}

int main() {
    std::string dictionary_file = "dictionary.txt";
    Trie trie = constructTrieFromFile(dictionary_file);

    std::string query;

    while (true) {
        std::cout << "Please type search queries:" << std::endl;
        std::cout << "> ";
        std::getline(std::cin, query);

        std::transform(query.begin(), query.end(), query.begin(), ::tolower);

        std::vector<std::string> options = trie.findAutoCompleteOptions(query);
        std::cout << "Your options are:" << std::endl;
        std::for_each(options.begin(), options.end(), [](const std::string& str) {
            std::cout << str << std::endl;
        });

    }

    return 0;
}
