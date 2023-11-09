#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <map>
#include <algorithm>

using namespace std;

class Seat {
    string place;
    bool available;
    string price;

public:

    Seat(){}

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

    void changeAvailability() {
        available = !available;
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

    vector<Seat> getAvailableSeats() const {
        vector<Seat> availableSeats;
        for (const auto& seat : this->availability) {
            if (seat.isAvailable()) {
                availableSeats.push_back(seat);
            }
        }
        return availableSeats;
    }

    string getDate() const {
        return this->date;
    }

    string getFlightNo() const {
        return this->flight_no;
    }
};

class Ticket {
    int id;
    string date;
    string flight_no;
    string passenger;
    Seat seat;

public:
    // Default constructor
    Ticket() {}

    // Parameterized constructor
    Ticket(const string& date, const string& flight_no, const string& passenger, const Seat& seat, const int& id)
        : date(date), flight_no(flight_no), passenger(passenger), seat(seat), id(id) {
        cout << "Ticket created, ticket.no: " << id << endl;
    }

    int getId() const {
        return this->id;
    }

    string getFlight() const {
        return this->flight_no;
    }

    string getDate() const {
        return this->date;
    }

    string getPassenger() const {
        return this->passenger;
    }

    Seat getSeat() const {
        return this->seat;
    }
};

class Passenger {
    string name;
    vector<Ticket> tickets;

public:
    Passenger(const string& name): name(name){}

    void addTicket(const Ticket& ticket) {
        this->tickets.push_back(ticket);
    }

    string getName() const {
        return this->name;
    }

    vector<Ticket> getTickets() const {
        return this->tickets;
    }
};

class ConfigReader {

public:
    ConfigReader(){}

    vector<Airplane> readConfig(const string& filename) {

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

class CommandExecutor {

    string filename = "config-input.txt";
    ConfigReader reader;
    vector<Airplane> airplanes = reader.readConfig(filename);
    vector<Ticket> tickets;
    vector<Passenger> passengers;
    int id = 1234;

public:

    vector<Seat> findAirplane(const string& date, const string& flight_no) {
        for (const auto& airplane : this->airplanes) {
            if (airplane.getDate() == date && airplane.getFlightNo() == flight_no) {
                return airplane.getAvailableSeats();
            }
        }

        // Return an empty vector if no matching airplane is found
        return vector<Seat>();
    };

    void bookSeat(const string& date, const string& flight_no, 
        const string& place, const string& passengerName) {

        bool ticketBooked = false;

        for (const auto& airplane : this->airplanes) {
            if (airplane.getDate() == date && airplane.getFlightNo() == flight_no) {
                for (auto& seat : airplane.getAvailableSeats()) {
                    if (seat.getPlace() == place) {
                        Ticket ticket(date, flight_no, passengerName, seat, id);
                        id++;
                        seat.changeAvailability();
                        ticketBooked = true;
                        tickets.push_back(ticket);

                        bool found = false;
                        for (auto& passenger : passengers) {
                            if (passenger.getName() == passengerName) {
                                // If the passenger already exists, add the ticket to their tickets
                                passenger.addTicket(ticket);
                                found = true;
                                break;
                            }
                        }

                        if (!found) {
                            // If the passenger does not exist, create a new passenger and add the ticket to their tickets
                            Passenger newPassenger(passengerName);
                            newPassenger.addTicket(ticket);
                            passengers.push_back(newPassenger);
                        }
                        break;

                        
                    }
                }
                if (ticketBooked) {
                    break;
                }
                else {
                    cout << "The seat " << place << " is already booked." << endl;
                }
            }
        }

        if (!ticketBooked) {
            cout << "You entered a non-existing flight" << endl;
        }

        // cout << tickets.size() << endl;
        // cout << passengers.size() << endl;
    }

    void viewTicket(const int& id_number) {

        int ticketIndex = findTicket(id_number);
        if (ticketIndex != - 1) {
            Ticket ticket = tickets[ticketIndex];

            cout << "Flight: " << ticket.getFlight() << endl
                << "Date: " << ticket.getDate() << endl
                << "Place: " << ticket.getSeat().getPlace() << endl
                << "Price: " << ticket.getSeat().getPrice() << endl
                << "Passenger name: " << ticket.getPassenger() << endl;
        }
    }

    int findTicket(const int& id) {
        for (size_t i = 0; i < tickets.size(); ++i) {
            if (tickets[i].getId() == id) {
                return i;  // Return the index of the found object
            }
        }
        return -1;  // Return -1 if no object was found
    }


    void showForUser(const string& psngName) {

        bool userFound = false;

        for (size_t i = 0; i < passengers.size(); ++i) {
            if (passengers[i].getName() == psngName) {
                userFound = true;
                vector<Ticket> pasTickets = passengers[i].getTickets();
                for (size_t j = 0; j < pasTickets.size(); ++j) {
                    int id = pasTickets[j].getId();
                    cout << j + 1<< ". ";
                    viewTicket(id);
                }
                if (userFound) {
                    break;
                }
            }
        }
        if(!userFound) {
            cout << "There is no user with entered username" << endl;
        }
    }

};

class InputReader {

    CommandExecutor executor;

public:

    void ProcessInput() {

        while (true){
            string inputline;
            cout << "> ";
            getline(cin, inputline);

            istringstream iss(inputline);
            string word;
            vector<std::string> inputParams;
            while (iss >> word) {
                inputParams.push_back(word);
            }

            string command = inputParams[0];

            if (command == "check") {
                string date = inputParams[1];
                string flight = inputParams[2];
                vector<Seat> availability = executor.findAirplane(date, flight);
                // cout << availability.size() << endl;

                cout << "Available places:" << endl;
                for (const auto& seat : availability) {
                    cout << seat.getPlace() << " - " << seat.getPrice() << endl;
                }
            }

            else if (command == "book") {
                // booking
                string date = inputParams[1];
                string flight = inputParams[2];
                string place = inputParams[3];
                string passenger = inputParams[4];

                executor.bookSeat(date, flight, place, passenger);
            }

            else if (command == "return") {
                // returning the ticket
            }

            else if (command == "view") {
                // viewing by id/username

                if (inputParams[1] == "username") {
                    // viewing by username 
                    string passenger = inputParams[2];
                    executor.showForUser(passenger);
                }

                else {
                    // view by ticket id
                    int id = stoi(inputParams[1]);
                    executor.viewTicket(id);
                }
            }

            else {
                cout << "invalid command" << endl;
            }
        }
        
    }

};


int main() {
    
    InputReader inpReader;
    inpReader.ProcessInput();

    return 0;
}
