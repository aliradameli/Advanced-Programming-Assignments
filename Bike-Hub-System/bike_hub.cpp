#include <iostream>
#include <vector>
#include <string>
using namespace std; 

struct bike {
    string bike_id;
    string bike_type; 
};

struct station {
    string station_id;
    int capacity; 
    vector<bike> avalable_bikes;
};

struct user{
    string user_id ;
    vector<bike> rented_bikes ;
    vector<int> rent_times ; 
    vector<int> return_times;
    bool has_bike;
};

// Global storage for stations in the system
vector<station> STATIONS;
// Global storage for users in the system
vector<user> USERS;

// 5 $/min : electrical and 2 $/min : regular
int ELECTRICAL_RATE = 5, REGULAR_RATE = 2; 

void handleAddStation();
void handleAddBike();
void handleRentBike();
void handelReturnBike();
void handleStationStatus();
void handleReportUsers();

int main() {
    string command;
    while (cin >> command) {
        if (command == "add_station") {
            handleAddStation();
        } 
        if (command == "add_bike") {
            handleAddBike();
        }
        if (command == "rent_bike"){
            handleRentBike();
        }
        if (command == "return_bike"){
            handelReturnBike();
        }
        if (command == "station_status"){
            handleStationStatus();
        }
        if (command == "report_users") {
            handleReportUsers();
        }
    }
    return 0;
}

/*
--------------------------------------------------------------------------------
-----------------------------Add station----------------------------------------
--------------------------------------------------------------------------------
*/

// Function to handle the add_station command
void handleAddStation() {
    string station_id;
    int capacity;
    cin >> station_id >> capacity;

    // Check if the station already exists
    for (const station& station : STATIONS) {
        if (station_id == station.station_id) {
            cout << "Station already exists" << endl; //
            return;
        }
    }

    // Add the new station if it doesn't exist
    station new_station;
    new_station.station_id = station_id;
    new_station.capacity = capacity;
    STATIONS.push_back(new_station); // appending the global list of stations.
    
    cout << "OK" << endl; //
}


/*
--------------------------------------------------------------------------------
-------------------------------Add bike-----------------------------------------
--------------------------------------------------------------------------------
*/

// Function to handle the add_bike command
void handleAddBike() {
    string bike_id, station_id, bike_type;
    cin >> bike_id >> station_id >> bike_type;

    // Find the target station
    station* target_station = nullptr;
    for (station& station : STATIONS) {
        if (station.station_id == station_id) {
            target_station = &station;
            break;
        }
    }

    if (target_station != nullptr) {
        // Check if the station capacity is full
        if (target_station->avalable_bikes.size() >= target_station->capacity) {
            cout << "Station is full" << endl; //
        } else {
            // Add the bike to the station
            bike new_bike;
            new_bike.bike_id = bike_id;
            new_bike.bike_type = bike_type;
            target_station->avalable_bikes.push_back(new_bike);
            cout << "OK" << endl; //
        }
    }
}


/*
--------------------------------------------------------------------------------
-------------------------------Rent bike----------------------------------------
--------------------------------------------------------------------------------
*/

// Returns the smallest bike_id among bikes of the given type, or "" if none found.
string findSmallestBikeId(vector<bike> bikes, string bike_type) {
    string smallest_id = "";
    for (const bike& b : bikes) {
        if (b.bike_type == bike_type) {
            if (smallest_id.empty() || b.bike_id < smallest_id) {
                smallest_id = b.bike_id;
            }
        }
    }
    return smallest_id;
}

user* findUserByID(string user_id){
    for(user& user: USERS) {
        if(user.user_id == user_id) {
            return &user;
        }
    }
    //For better UX pls uncomment this part:
    //cout << "New user registered (id: " << user_id << ")\n";
    return nullptr;
}

station* findStationByID(string station_id){
    for(station& station: STATIONS) {
        if(station.station_id == station_id) {
            return &station;
        }
    }
    //For better UX pls uncomment this part:
    //cout << "No such station id \n"; 
    return nullptr;
}

user* createNewUser(string user_id){
    user new_user;
    new_user.user_id = user_id;
    new_user.has_bike = false;
    USERS.push_back(new_user);
    return &USERS.back();
}

void rentBike(user* user, station* station, bike& rented_bike, int time) {
    vector<bike>& bikes = station->avalable_bikes;
    
    for (auto b = bikes.begin(); b != bikes.end(); ++b) {
        if (b->bike_id == rented_bike.bike_id) {
            rented_bike = *b;
            bikes.erase(b);
            break;
        }
    }
    
    // Optional: if removeBike needs to be called here
    // removeBike(rented_bike, station);

    user->rented_bikes.push_back(rented_bike);
    user->rent_times.push_back(time);
    user->has_bike = true;
}

void handleRentBike() {
    string user_id, station_id, bike_type;
    int time;

    cin >> user_id >> station_id >> bike_type >> time;

    // Find the user
    user* user = findUserByID(user_id);

    // Create the user if they don't exist yet
    if (user == nullptr) { 
        user = createNewUser(user_id);
    }

    // Check if the user already has an active ride
    if (user->has_bike) {
        cout << "User already has an active ride" << endl;
        return;
    }

    // Find the station (guaranteed to exist per problem statement)
    station* station = findStationByID(station_id);
    if (station == nullptr) {
        return;
    }

    // Find the smallest bike ID of the requested type
    string bike_id = findSmallestBikeId(station->avalable_bikes, bike_type);
    if (bike_id.empty()) {
        cout << "No available bike of this type" << endl;
        return;
    }

    // Create a dummy bike object with just the bike_id to pass by reference
    bike rented_bike;
    rented_bike.bike_id = bike_id;

    // Execute the rental using your helper function
    rentBike(user, station, rented_bike, time);

    cout << "OK" << endl;
}


/*
--------------------------------------------------------------------------------
-------------------------------Return bike----------------------------------------
--------------------------------------------------------------------------------
*/
// Helper function to handle the cost calculation
int calculateCost(string bike_type, int rent_time, int return_time) {
    int duration = return_time - rent_time;
    int rate = (bike_type == "electric") ? ELECTRICAL_RATE : REGULAR_RATE;
    return duration * rate;
}

void returnBike(user* user, station* station, int return_time) {
    bike returned_bike = user->rented_bikes.back();
    int rent_time = user->rent_times.back();
    
    // Calculate duration and cost using the helper function
    int duration = return_time - rent_time;
    int cost = calculateCost(returned_bike.bike_type, rent_time, return_time);
    
    // Add the bike back to the station's available bikes
    station->avalable_bikes.push_back(returned_bike);
    
    // Update user state
    user->return_times.push_back(return_time);
    user->has_bike = false;
    
    cout << "Ride finished: duration = " << duration << " mins, cost = " << cost << endl;
}

// Function to handle the return_bike command
void handelReturnBike() { 
    string user_id, station_id;
    int time;

    cin >> user_id >> station_id >> time;

    // Find the user
    user* user = findUserByID(user_id);

    // Check if the user exists and has an active ride
    if (user == nullptr || !user->has_bike) {
        cout << "User does not have an active ride" << endl;
        return;
    }

    // Find the destination station
    station* station = findStationByID(station_id);
    if (station == nullptr) {
        return;
    }

    // Check if the destination station is full
    if (station->avalable_bikes.size() >= station->capacity) {
        cout << "Station is full" << endl;
        return;
    }

    // Execute the return using the helper function (which prints the success message)
    returnBike(user, station, time);
}

/*
--------------------------------------------------------------------------------
-----------------------------Station status-------------------------------------
--------------------------------------------------------------------------------
*/

// Returns how many bikes of the given type are in the list.
int countBikesByType(const vector<bike>& bikes, const string& bike_type) {
    int count = 0;
    for (const bike& b : bikes) {
        if (b.bike_type == bike_type) {
            count++;
        }
    }
    return count;
}

// Returns all bike ids in ascending alphabetical order.
vector<string> getSortedBikeIds(const vector<bike>& bikes) {
    vector<string> ids;
    for (const bike& b : bikes) {
        ids.push_back(b.bike_id);
    }
    sort(ids.begin(), ids.end());
    return ids;
}

// Function to handle the station_status command
void handleStationStatus() {
    string station_id;
    cin >> station_id;

    // Find the station (guaranteed to exist per problem statement)
    station* station = findStationByID(station_id);
    if (station == nullptr) {
        return;
    }

    int total = station->avalable_bikes.size();
    int regular_count = countBikesByType(station->avalable_bikes, "regular");
    int electric_count = countBikesByType(station->avalable_bikes, "electric");
    int empty_slots = station->capacity - total;

    cout << "Station " << station_id << ":" << endl;
    cout << "Capacity: " << station->capacity << endl;
    cout << "Available bikes: " << total
         << " (Regular: " << regular_count
         << ", Electric: " << electric_count << ")" << endl;
    cout << "Empty slots: " << empty_slots << endl;

    if (total == 0) {
        cout << "Bikes: None" << endl;
        return;
    }

    vector<string> ids = getSortedBikeIds(station->avalable_bikes);
    cout << "Bikes: ";
    for (size_t i = 0; i < ids.size(); i++) {
        if (i > 0) {
            cout << ", ";
        }
        cout << ids[i];
    }
    cout << endl;
}



/*
--------------------------------------------------------------------------------
-----------------------------Report users---------------------------------------
--------------------------------------------------------------------------------
*/

// Returns the total duration of all completed rides of a user.
// A ride is completed when it has a matching entry in return_times.
int getUserTotalTime(const user* user) {
    int total_time = 0;
    for (size_t i = 0; i < user->return_times.size(); i++) {
        total_time += user->return_times[i] - user->rent_times[i];
    }
    return total_time;
}

// Returns the total cost of all completed rides of a user.
int getUserTotalCost(const user* user) {
    int total_cost = 0;
    for (size_t i = 0; i < user->return_times.size(); i++) {
        total_cost += calculateCost(user->rented_bikes[i].bike_type,
                                    user->rent_times[i],
                                    user->return_times[i]);
    }
    return total_cost;
}

// Sort order: total time descending, then user_id ascending.
bool compareUsersForReport(const user* user_a, const user* user_b) {
    int time_a = getUserTotalTime(user_a);
    int time_b = getUserTotalTime(user_b);
    if (time_a != time_b) {
        return time_a > time_b;
    }
    return user_a->user_id < user_b->user_id;
}

// Function to handle the report_users command
void handleReportUsers() {
    // Collect only users with at least one completed ride
    vector<user*> reported_users;
    for (user& u : USERS) {
        if (!u.return_times.empty()) {
            reported_users.push_back(&u);
        }
    }

    if (reported_users.empty()) {
        cout << "No rides completed yet" << endl;
        return;
    }

    sort(reported_users.begin(), reported_users.end(), compareUsersForReport);

    for (const user* u : reported_users) {
        cout << "User " << u->user_id
             << ": Total time = " << getUserTotalTime(u)
             << " mins, Total cost = " << getUserTotalCost(u) << endl;
    }
}