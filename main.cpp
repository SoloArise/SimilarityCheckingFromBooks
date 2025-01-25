#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <cctype>
#include <cmath>
#include <tuple>

using namespace std;

string normalizeWord(const string &word) {
    string normalized;
    for (char c : word) {
        if (isalnum(c)) {
            normalized += toupper(c);
        }
    }
    return normalized;
}

void removeStopWords(unordered_map<string, double> &wordFreq) {
    vector<string> stopWords = {"A", "AND", "AN", "OF", "IN", "THE"};
    for (const string &word : stopWords) {
        wordFreq.erase(word);
    }
}

unordered_map<string, double> calculateWordFrequency(const string &filePath) {
    ifstream file(filePath);
    if (!file.is_open()) {
        cerr << "Error: Could not open file " << filePath << endl;
        return {};
    }
    unordered_map<string, double> wordFreq;
    string word;
    double totalWords = 0;

    while (file >> word) {
        string normalized = normalizeWord(word);
        if (!normalized.empty()) {
            wordFreq[normalized]++;
            totalWords++;
        }
    }

    removeStopWords(wordFreq);

    for (auto &pair : wordFreq) {
        pair.second /= totalWords;
    }

    vector<pair<string, double>> wordList(wordFreq.begin(), wordFreq.end());
    sort(wordList.begin(), wordList.end(), [](const auto &a, const auto &b) {
        return b.second < a.second;
    });

    unordered_map<string, double> topWords;
    for (size_t i = 0; i < min<size_t>(100, wordList.size()); ++i) {
        topWords[wordList[i].first] = wordList[i].second;
    }

    return topWords;
}

double calculateSimilarity(const unordered_map<string, double> &freq1, const unordered_map<string, double> &freq2) {
    double similarity = 0.0;
    for (const auto &pair : freq1) {
        if (freq2.find(pair.first) != freq2.end()) {
            similarity += min(pair.second, freq2.at(pair.first));
        }
    }
    return similarity;
}

int main() {
    const int numBooks = 64;
    vector<string> bookFiles(numBooks);

    for (int i = 0; i < numBooks; ++i) {
        cout << "Enter the path for Book-Txt" << i + 1 << ": ";
        getline(cin, bookFiles[i]);
    }

    vector<unordered_map<string, double>> wordFrequencies;
    for (const string &filePath : bookFiles) {
        wordFrequencies.push_back(calculateWordFrequency(filePath));
    }

    vector<vector<double>> similarityMatrix(numBooks, vector<double>(numBooks, 0.0));
    for (int i = 0; i < numBooks; ++i) {
        for (int j = i + 1; j < numBooks; ++j) {
            double similarity = calculateSimilarity(wordFrequencies[i], wordFrequencies[j]);
            similarityMatrix[i][j] = similarity;
            similarityMatrix[j][i] = similarity;
        }
    }

    vector<tuple<double, int, int>> similarities;
    for (int i = 0; i < numBooks; ++i) {
        for (int j = i + 1; j < numBooks; ++j) {
            similarities.emplace_back(similarityMatrix[i][j], i, j);
        }
    }

    sort(similarities.rbegin(), similarities.rend()); 

    cout << "Top 10 similar pairs of books:\n";
    for (int k = 0; k < 10 && k < similarities.size(); ++k) {
        double similarity = std::get<0>(similarities[k]);
        int i = std::get<1>(similarities[k]);
        int j = std::get<2>(similarities[k]);
        cout << "Book " << i + 1 << " and Book " << j + 1 << " have similarity index: " << similarity << endl;
    }

    return 0;
}
