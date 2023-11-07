#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <map>

using namespace std;

class Seat {
    string place;
    bool available;
    string price;

public:
    // Constructor
    Seat(const string& place, const string& price, bool available = true)
        : place(place), price(price), available(available) {}

    string getPlace() const {
        return this->place;
    }

    string getPrice() const {
        return this->price;
    }

    bool isAvailable() const {
        return this->available;
    }
};


class Airplane {

    string date;
    string flight_no;
    int seats;
    vector<Seat> availability;

public:

    // Parameterized constructor
    Airplane(const string& date, const string& flight_no, int seats, const vector<Seat>& availability)
        : date(date), flight_no(flight_no), seats(seats), availability(availability) {}

};

class ConfigReader {

    string filename;

public:
    ConfigReader(const string& filename) : filename(filename) {}

    vector<Airplane> readConfig() {

        vector<Airplane> airplanes;
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

                    string date = words[0]; // e.g. 11.12.22
                    string flight = words[1]; // e.g. FQ12
                    int seatsInRow = stoi(words[2]);

                    // Generate seat letters based on the number of seats in a row
                    std::vector<Seat> seats;
                    std::string seatLetters = "";
                    for (int i = 0; i < seatsInRow; i++) {
                        seatLetters += 'A' + i;
                    }

                    // Create a Seat object for each place
                    map<string, string> seatRangesAndPrices;
                    for (size_t i = 3; i < words.size(); i += 2) {
                        seatRangesAndPrices[words[i]] = words[i + 1];
                    }

                    for (const auto& seatRangeAndPrice : seatRangesAndPrices) {
                        string range = seatRangeAndPrice.first;
                        string price = seatRangeAndPrice.second;
                        int start = stoi(range.substr(0, range.find('-')));
                        int end = stoi(range.substr(range.find('-') + 1));

                        for (int row = start; row <= end; row++) {
                            for (char seat : seatLetters) {
                                string place = to_string(row) + seat;
                                Seat seat(place, price, true);
                                seats.push_back(seat);
                            }
                        }
                    }

                    // Create an Airplane object
                    Airplane airplane(date, flight, seats.size(), seats);
                    airplanes.push_back(airplane);
                }

            }
            file.close();
        }
        else {
            cout << "Unable to open file" << '\n';
        }

        return airplanes;
    }
};


int main() {
    
    ConfigReader reader("config-input.txt");
    vector<Airplane> airplanes = reader.readConfig();

    return 0;
}
