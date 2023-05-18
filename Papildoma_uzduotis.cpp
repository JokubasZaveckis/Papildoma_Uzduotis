#include "Header.h"

// Panaikina punktuacija, skaicius ir t.t.
wstring removePunctuation(const wstring& word) {
    wstring result;
    copy_if(word.begin(), word.end(), back_inserter(result), [](wchar_t c) {
        return iswalnum(c);
        });
    return result;
}

int main() {
    wifstream inputFile("duomenys.txt");
    if (!inputFile) {
        cerr << "Nepavyko atidaryti ivesties failo." << endl;
        return 1;
    }
    inputFile.imbue(locale(inputFile.getloc(), new codecvt_utf8<wchar_t>));

    unordered_map<wstring, int> wordCounts;
    unordered_map<wstring, vector<int>> wordLocations; // Lentele zodziu pasikartojimo vietoms saugoti
    wstring line;
    wregex urlRegex(LR"(\b((https?|ftp|file):\/\/[-A-Za-z0-9+&@#\/%?=~_|!:,.;]*[-A-Za-z0-9+&@#\/%=~_|])|\b(www\.[^\s]+))");

    int lineNumber = 1; // Seka teksto eiluciu numerius

    // Iteruojama per kiekviena eilute ivesties faile
    while (getline(inputFile, line)) {
        wsmatch match;
        wstring::const_iterator searchStart(line.cbegin());

        // Surandami ir spausdinami visi URL'ai eiluteje
        while (regex_search(searchStart, line.cend(), match, urlRegex)) {
            wcout << L"URL: " << match[0] << endl;
            searchStart = match.suffix().first;
        }

        wistringstream iss(line);
        wstring word;

        // Apskaiciuojamas kiekvieno zodzio pasikartojimu skaicius
        while (iss >> word) {
            wstring cleanWord = removePunctuation(word);
            if (!cleanWord.empty()) {
                wordCounts[cleanWord]++;
                wordLocations[cleanWord].push_back(lineNumber); // sšsaugomos zodzio pasikartojimo vietos
            }
        }

        lineNumber++;
    }

    inputFile.close();

    // Spausdinami zodziai, pasikartojantys daugiau nei viena karta ir ju pasikartojimo vietos
    wofstream outputFile("output.txt");
    if (!outputFile) {
        cerr << "Nepavyko atidaryti isvesties failo." << endl;
        return 1;
    }
    outputFile.imbue(locale(outputFile.getloc(), new codecvt_utf8<wchar_t>));

    for (const auto& pair : wordCounts) {
        if (pair.second > 1) {
            outputFile << pair.first << L": " << pair.second << L" (eilutese: ";
            const vector<int>& locations = wordLocations[pair.first];
            for (size_t i = 0; i < locations.size(); i++) {
                outputFile << locations[i];
                if (i < locations.size() - 1) {
                    outputFile << L", ";
                }
            }
            outputFile << L")" << endl;
        }
    }

    outputFile.close();

    return 0;
}
