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

    Seat(Seat&& other) = delete;
    Seat(Seat& other) = delete;
};

class Airplane {

    string date;
    string flight_no;
    int seats;
    vector<shared_ptr<Seat>> availability; // change availability to hold shared_ptr<Seat>

public:

    Airplane(const string& date, const string& flight_no, int seats, const vector<shared_ptr<Seat>>& availability)
        : date(date), flight_no(flight_no), seats(seats), availability(availability) {} // change parameter to vector<shared_ptr<Seat>>

    vector<shared_ptr<Seat>>& getSeats() { // return a vector of shared_ptr<Seat>
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
    shared_ptr<Seat> seat; // make seat a shared_ptr

public:
    Ticket() : seat(nullptr) {} // initialize seat to nullptr in the default constructor

    Ticket(const string& date, const string& flight_no, const string& passenger, shared_ptr<Seat> seat, const int& id)
        : date(date), flight_no(flight_no), passenger(passenger), seat(seat), id(id) { // use shared_ptr
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

    shared_ptr<Seat> getSeat() const { // return a shared_ptr to the seat
        return this->seat;
    }
};



class Passenger {
    string name;
    vector<shared_ptr<Ticket>> tickets; // change tickets to hold shared_ptr<Ticket>

public:
    Passenger(const string& name) : name(name) {}

    void addTicket(const shared_ptr<Ticket>& ticket) { // change parameter to shared_ptr<Ticket>
        this->tickets.push_back(ticket);
    }

    string getName() const {
        return this->name;
    }

    vector<shared_ptr<Ticket>> getTickets() const { // return a vector of shared_ptr<Ticket>
        return this->tickets;
    }

    void removeTicketById(const int& id) {
        for (auto it = tickets.begin(); it != tickets.end(); ) {
            if ((*it)->getId() == id) {
                it = tickets.erase(it);
            }
            else {
                ++it;
            }
        }
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
            vector<shared_ptr<Seat>> seats;
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
                        shared_ptr<Seat> seat(new Seat(place, price, true)); // create a shared_ptr<Seat>
                        seats.push_back(seat); // add the shared_ptr<Seat> to the vector
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

    vector<shared_ptr<Seat>>& findAirplane(const string& date, const string& flight_no) {
        for (auto& airplane : this->airplanes) {
            if (airplane.getDate() == date && airplane.getFlightNo() == flight_no) {
                return airplane.getSeats();
            }
        }

        static vector<shared_ptr<Seat>> emptyVector;
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

        vector<shared_ptr<Seat>>& seats = targetAirplane->getSeats();
        shared_ptr<Seat> targetSeat = nullptr;

        for (shared_ptr<Seat>& seat : seats) {
            if (seat->getPlace() == place && seat->isAvailable()) {
                targetSeat = seat;
                break;
            }
        }


        if (targetSeat == nullptr) {
            cout << "The seat " << place << " is already booked." << endl;
            return;
        }

        targetSeat->changeAvailability();

        shared_ptr<Seat> seatPtr(targetSeat);
        Ticket ticket(date, flight_no, passengerName, seatPtr, id);

        id++;
        ticketBooked = true;
        tickets.push_back(ticket);

        bool passengerExists = false;
        for (auto& passenger : passengers) {
            if (passenger.getName() == passengerName) {
                shared_ptr<Ticket> ticketPtr = make_shared<Ticket>(ticket);
                passenger.addTicket(ticketPtr);
                passengerExists = true;
                break;
            }
        }

        if (!passengerExists) {
            Passenger newPassenger(passengerName);

            shared_ptr<Ticket> ticketPtr = make_shared<Ticket>(ticket);
            newPassenger.addTicket(ticketPtr);

            passengers.push_back(newPassenger);
        }
    }


    void viewTicket(const int& id_number) {

        int ticketIndex = findTicket(id_number);
        if (ticketIndex != - 1) {
            Ticket ticket = tickets[ticketIndex];

            cout << "Flight: " << ticket.getFlight() << endl
                << "Date: " << ticket.getDate() << endl
                << "Place: " << ticket.getSeat()->getPlace() << endl
                << "Price: " << ticket.getSeat()->getPrice() << endl
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
                vector<shared_ptr<Ticket>> pasTickets = passengers[i].getTickets();
                if (pasTickets.size() == 0) {
                    cout << "There is no ticket for this user" << endl;
                    return;
                }
                else {
                    for (size_t j = 0; j < pasTickets.size(); ++j) {
                        if (pasTickets[j] != nullptr) {
                            int id = pasTickets[j]->getId();
                        }
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
        string place = ticket.getSeat()->getPlace();

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

        shared_ptr<Seat> targetSeat = nullptr;
        for (auto& seat : targetAirplane->getSeats()) {
            if (seat->getPlace() == place) { // use -> to access members of Seat
                targetSeat = seat;
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
                vector<shared_ptr<Seat>> availability = executor.findAirplane(date, flight);

                cout << "Available places:" << endl;
                for (const auto& seat : availability) {
                    if (seat->isAvailable()) {
                        cout << seat->getPlace() << " - " << seat->getPrice() << endl;
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
