#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <string>

using namespace std;


class ConfigReader {

    string filename;

public:
    ConfigReader(const string& filename) : filename(filename) {}

    void readConfig() {
        ifstream file(filename);
        string line;
        int num_records = 0;

        if (file.is_open()) {
            if (getline(file, line)) {
                num_records = stoi(line);
            }

            for (int i = 0; i < num_records; i++) {
                if (getline(file, line)) {
                    istringstream iss(line);
                    vector<string> words;
                    string word;

                    while (iss >> word) {
                        words.push_back(word);
                    }

                    for (const auto& word : words) {
                        cout << word << endl;
                    }
                }
            }
            file.close();
        }
        else {
            cout << "Unable to open file" << '\n';
        }
    }
};


int main() {
    
    ConfigReader reader("config-input.txt");
    reader.readConfig();

    return 0;
}
