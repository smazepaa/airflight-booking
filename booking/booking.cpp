#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <cstdlib>

using namespace std;

class Seat {
    string place;
    bool available;
    string price;

public:

    Seat(){}

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

    Airplane(const string& date, const string& flight_no, int seats, const vector<Seat>& availability)
        : date(date), flight_no(flight_no), seats(seats), availability(availability) {}

    vector<Seat>& getSeats() {
        return this->availability;
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
    Ticket() {}

    Ticket(const string& date, const string& flight_no, const string& passenger, const Seat& seat, const int& id)
        : date(date), flight_no(flight_no), passenger(passenger), seat(seat), id(id) {
        cout << "Ticket booked, ticket.no: " << id << endl;
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

    void removeTicketById(const int& id) {
        this->tickets.erase(remove_if(this->tickets.begin(), this->tickets.end(),
            [id](const Ticket& ticket) { return ticket.getId() == id; }), tickets.end());
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

        if (!file.is_open()) {
            cout << "Unable to open file" << '\n';
            return airplanes;
        }

        if (!getline(file, line)) {
            return airplanes;
        }

        num_records = stoi(line);

        for (int i = 0; i < num_records; i++) {
            if (!getline(file, line)) {
                continue;
            }

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
            vector<Seat> seats;
            string seatLetters = "";
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

        file.close();

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

    vector<Seat>& findAirplane(const string& date, const string& flight_no) {
        for (auto& airplane : this->airplanes) {
            if (airplane.getDate() == date && airplane.getFlightNo() == flight_no) {
                return airplane.getSeats();
            }
        }

        static vector<Seat> emptyVector;
        return emptyVector;
    }


    void bookSeat(const string& date, const string& flight_no,
        const string& place, const string& passengerName) {

        bool ticketBooked = false;
        Airplane* targetAirplane = nullptr;

        for (Airplane& airplane : this->airplanes) {
            if (airplane.getDate() == date && airplane.getFlightNo() == flight_no) {
                targetAirplane = &airplane;
                break;
            }
        }

        if (targetAirplane == nullptr) {
            cout << "You entered a non-existing flight" << endl;
            return;
        }

        vector<Seat>& seats = targetAirplane->getSeats();
        Seat* targetSeat = nullptr;

        for (Seat& seat : seats) {
            if (seat.getPlace() == place && seat.isAvailable()) {
                targetSeat = &seat;
                break;
            }
        }

        if (targetSeat == nullptr) {
            cout << "The seat " << place << " is already booked." << endl;
            return;
        }

        targetSeat->changeAvailability();
        Ticket ticket(date, flight_no, passengerName, *targetSeat, id);
        id++;
        ticketBooked = true;
        tickets.push_back(ticket);

        bool passengerExists = false;
        for (auto& passenger : passengers) {
            if (passenger.getName() == passengerName) {
                passenger.addTicket(ticket);
                passengerExists = true;
                break;
            }
        }

        if (!passengerExists) {
            Passenger newPassenger(passengerName);
            newPassenger.addTicket(ticket);
            passengers.push_back(newPassenger);
        }
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
                return i;
            }
        }
        return -1;
    }

    void showForUser(const string& psngName) {

        bool userFound = false;

        for (size_t i = 0; i < passengers.size(); ++i) {
            if (passengers[i].getName() == psngName) {
                userFound = true;
                vector<Ticket> pasTickets = passengers[i].getTickets();
                if (pasTickets.size() == 0) {
                    cout << "There is no ticket for this user" << endl;
                    return;
                }
                else {
                    for (size_t j = 0; j < pasTickets.size(); ++j) {
                        int id = pasTickets[j].getId();
                        cout << j + 1 << ". ";
                        viewTicket(id);
                    }
                    if (userFound) {
                        break;
                    }
                }
                
            }
        }
        if(!userFound) {
            cout << "There is no user with entered username" << endl;
        }
    }

    void returnTicket(const int& id) {
        int ticketIndex = findTicket(id);
        if (ticketIndex == -1) {
            cout << "Error: Ticket with ID " << id << " not found." << endl;
            return;
        }

        Ticket ticket = tickets[ticketIndex];
        string date = ticket.getDate();
        string flight_no = ticket.getFlight();
        string place = ticket.getSeat().getPlace();

        Airplane* targetAirplane = nullptr;
        for (auto& airplane : airplanes) {
            if (airplane.getDate() == date && airplane.getFlightNo() == flight_no) {
                targetAirplane = &airplane;
                break;
            }
        }

        if (targetAirplane == nullptr) {
            cout << "Error: Could not find the corresponding airplane." << endl;
            return;
        }

        Seat* targetSeat = nullptr;
        for (auto& seat : targetAirplane->getSeats()) {
            if (seat.getPlace() == place) {
                targetSeat = &seat;
                break;
            }
        }

        if (targetSeat == nullptr) {
            cout << "Error: Could not find the corresponding seat in the airplane." << endl;
            return;
        }

        targetSeat->changeAvailability();
        cout << "Ticket with ID " << id << " has been returned." << endl;
        this->tickets.erase(this->tickets.begin() + ticketIndex);

        for (auto& passenger : passengers) {
            if (passenger.getName() == ticket.getPassenger()) {
                passenger.removeTicketById(id);
                break;
            }
        }
    }
};

class InputReader {

    CommandExecutor executor;

public:

    bool isValidInput(const vector<string>& inputParams) {
        string command = inputParams[0];

        if (command == "check") {
            return inputParams.size() == 3;
        }
        else if (command == "book") {
            return inputParams.size() == 5;
        }
        else if (command == "return") {
            return inputParams.size() == 2;
        }
        else if (command == "view") {
            if (inputParams[1] == "username") {
                return inputParams.size() == 3;
            }
            else {
                return inputParams.size() == 2;
            }
        }
        else if (command == "clear") {
            return inputParams.size() == 1;
        }
        else {
            return false;
        }
    }

    void ProcessInput() {

        while (true) {
            string inputline;
            cout << "> ";
            getline(cin, inputline);

            istringstream iss(inputline);
            string word;
            vector<string> inputParams;
            while (iss >> word) {
                inputParams.push_back(word);
            }

            if (inputParams.size() == 0) {
                cout << "You should enter something" << endl;
                continue;
            }

            if (!isValidInput(inputParams)) {
                cout << "Invalid input" << endl;
                continue;
            }

            string command = inputParams[0];

            if (command == "check") {
                // checking
                string date = inputParams[1];
                string flight = inputParams[2];
                vector<Seat> availability = executor.findAirplane(date, flight);

                cout << "Available places:" << endl;
                for (const auto& seat : availability) {
                    if (seat.isAvailable()) {
                        cout << seat.getPlace() << " - " << seat.getPrice() << endl;
                    }
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
                int id = stoi(inputParams[1]);
                executor.returnTicket(id);
            }

            else if (command == "view") {
                // viewing by id/username

                if (inputParams[1] == "username") {
                    // viewing by username 
                    string passenger = inputParams[2];
                    executor.showForUser(passenger);
                }

                else {
                    // viewing by ticket id
                    int id = stoi(inputParams[1]);
                    executor.viewTicket(id);
                }
            }

            else if (command == "clear") {
                system("CLS");
            }
        }
    }
};


int main() {
    
    InputReader inpReader;
    inpReader.ProcessInput();

    return 0;
}
