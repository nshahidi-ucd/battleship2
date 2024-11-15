#include "Player.h"

#include "Battleship.h"
#include "Carrier.h"
#include "Cruiser.h"
#include "Destroyer.h"
#include "Enums.h"
#include "Grid.h"
#include "Ship.h"
#include "Submarine.h"

#include <string>
    using std::string;

#include <iostream>
    using std::cout;
    using std::cin;
    using std::endl;

#include <array>
    using std::array;

#include <vector>
    using std::vector;

#include <stdexcept>
    using std::domain_error;
    using std::out_of_range;



Player::Player() {};

Player::Player(PlayerType player_type) : type {player_type} 
{
    if (player_type == CPU) {
        this->name = "Camden";
    }
    this->grid = new Grid(player_type);
}

Player::Player(PlayerType player_type, string player_name) : type {player_type}, name {player_name} 
{
    this->grid = new Grid(player_type);
}

Player::Player(PlayerType player_type, string player_name, Player* the_foe) : type {player_type}, name {player_name} 
{
    this->grid = new Grid(player_type);
    this->makeFoe(the_foe);
}

Player::~Player() {
    this->floatingShips.clear();
    this->sunkenShips.clear();
    for(size_t i = 0; i < this->ships.size(); ++i) {
        delete this->ships[i];
        this->ships[i] = nullptr;
    }
    delete this->grid;
    this->grid = nullptr;
    this->foe = nullptr;
    this->foeGrid = nullptr;
}

PlayerType Player::getPlayerType() const {
    return this->type;
}

string Player::getName() const {
    return this->name;
}

Grid* Player::getGrid()& {
    return this->grid;
}

Player* Player::getFoe() const {
    return this->foe;
}

Grid* Player::getFoeGrid()& {
    return this->foeGrid;
}

array<Ship*, 5> Player::getShips() const {
    return this->ships;
}

vector<Ship*> Player::getFloatingShips() const {
    return this->floatingShips;
}

vector<Ship*> Player::getSunkenShips() const {
    return this->sunkenShips;
}

vector<string> Player::getTargetedSpaces() const {
    return this->targetedSpaces;
}

vector<string> Player::getHitSpaces() const {
    return this->hitSpaces;
}

vector<string> Player::getMissSpaces() const {
    return this->missSpaces;
}

vector<char> Player::getHMHist() const {
    return this->HMHist;
}

void Player::setPlayerType(PlayerType player_type) {
    this->type = player_type;
}

void Player::setName(string player_name) {
    this->name = player_name;
}

void Player::setFoe(Player* the_foe) {
    this->foe = the_foe;
}

void Player::setFoeGrid(Grid* the_grid) {
    this->foeGrid = the_grid;
}

void Player::makeFoe(Player* &the_foe) {
    this->foe = the_foe;
    this->foeGrid = the_foe->getGrid();
}

bool Player::hasShip(Ship* ship) const {
    for (Ship* the_ship : this->ships)
        if(ship == the_ship)
            return true;
    return false;
}

bool Player::shipIsFloating(Ship* ship) const {
    if(!this->hasShip(ship))
        return false;
    for (Ship* the_ship : this->floatingShips)
        if(ship == the_ship)
            return true;
    return false;
}

bool Player::shipIsSunken(Ship* ship) const {
    if(!this->hasShip(ship))
        return false;
    for (Ship* the_ship : this->sunkenShips)
        if(ship == the_ship)
            return true;
    return false;
}

void Player::stillFloating() const {
    cout << "" << endl;
    for(Ship* ship : this->floatingShips)
        cout << ship->getShipName() << endl;
    cout << "" << endl;
}

void Player::notSunkYet() const {
    cout << "" << endl;
    for(Ship* ship : this->foe->getFloatingShips())
        cout << ship->getShipName() << endl;
    cout << "" << endl;
}

void Player::showFoe() const {
    cout << "" << endl;
    this->foeGrid->showGrid();
    cout << "" << endl;
}

void Player::showOwn() const {
    cout << "" << endl;
    this->grid->showGrid();
    cout << "" << endl;
}

Ship* Player::justSunkenShip() const {
    for(Ship* ship : this->floatingShips)
        if(ship->wasSunk())
            return ship;
    return nullptr;
}

void Player::sinkShip(Ship* ship) {
    if(!(this->hasShip(ship) && this->shipIsFloating(ship))){
        cout << "Oopsie." << endl;
        return;
    }
    for(size_t i = 0; i < this->floatingShips.size(); ++i)
        if(this->floatingShips[i] == ship) {
            this->floatingShips.erase(this->floatingShips.begin() + i);
            this->sunkenShips.push_back(ship);
        }
}

bool Player::spaceWasTargeted(string space) const {
    for(string the_space : this->targetedSpaces)
        if(space == the_space)
            return true;
    return false;
}

bool Player::target(string space, bool do_cout) {
    if(this->foeGrid == nullptr)
        throw domain_error("Foe grid not set.");
    if(!Spaces::isSpaceString(space)) {
        if(do_cout) cout << "Invalid entry." << endl;
        return false;
    }
    if(this->spaceWasTargeted(space)) {
        if(do_cout) cout << "Space already targeted." << endl;
        return false;
    }
    TargetResult shot;
    try {
        shot = this->foeGrid->target(space);
    } catch(std::exception& e) {
        cout << e.what() << endl;
        return false;
    }
    this->targetedSpaces.push_back(space);
    if (!do_cout) cout << space << endl;
    switch (shot) {
        case MISS: {
            this->missSpaces.push_back(space);
            this->HMHist.push_back('M');
            cout << "Miss" << endl;
            break;
        }
        case HIT: {
            this->hitSpaces.push_back(space);
            this->HMHist.push_back('H');
            cout << "Hit" << endl;
            Ship* sunk_ship = this->foe->justSunkenShip();
            if(sunk_ship != nullptr) {
                cout << this->foe->getName() << "\'s " << sunk_ship->getShipName() << " has been sunk!" << endl;
                this->foe->sinkShip(sunk_ship);
            }
            break;
        }
    };
    return true;
}

bool Player::processInput(string input) {
    if (input == "unsunk") {
        this->notSunkYet();
        return false;
    } else if (input == "afloat") {
        this->stillFloating();
        return false;
    } else if (input == "foe") {
        this->showFoe();
        return false;
    } else if(input == "own") {
        this->showOwn();
        return false;
    } else {
        return this->target(input);
    }
}

bool Player::allShipsAreSunk() const {
    return ! this->floatingShips.size() && this->sunkenShips.size() == 5;
} 

void Player::setCarrier() {
    Carrier* carrier = new Carrier(this->grid);
    do {
        string start_space;
        char direction;
        cout << "Enter Carrier Start Space: ";
        cin >> start_space;
        while(!Spaces::isSpaceString(start_space)) {
            cout << "Invalid Space." << endl;
            cout << "Enter Carrier Start Space: ";
            cin >> start_space;
        }
        cout << "Enter Carrier Direction: ";
        cin >> direction;
        while (!(direction == 'N' || direction == 'S' || direction == 'E' || direction == 'W')) {
            cout << "Invalid Direction." << endl;
            cout << "Enter Carrier Direction: ";
            cin >> direction;
        }
        carrier->setIsReady(carrier->placeOnGrid(start_space, direction));
    } while (!carrier->getIsReady());
    this->ships[0] = carrier;
    this->floatingShips.push_back(carrier);
    cout << "" << endl;
    this->grid->showGrid();
    cout << "" << endl;
}

void Player::setBattleship() {
    Battleship* battleship = new Battleship(this->grid);
    do {
        string start_space;
        char direction;
        cout << "Enter Battleship Start Space: ";
        cin >> start_space;
        while(!Spaces::isSpaceString(start_space) || this->grid->isNoGoSpace(start_space)) {
            if(!Spaces::isSpaceString(start_space))
                cout << "Invalid Space." << endl;
            else
                cout << "Space is occupied by or adjacent to an existing Ship. Please choose another space." << endl;
            cout << "Enter Battleship Start Space: ";
            cin >> start_space;
        }
        cout << "Enter Battleship Direction: ";
        cin >> direction;
        while (!(direction == 'N' || direction == 'S' || direction == 'E' || direction == 'W')) {
            cout << "Invalid Direction." << endl;
            cout << "Enter Battleship Direction: ";
            cin >> direction;
        }
        battleship->setIsReady(battleship->placeOnGrid(start_space, direction));
    } while (!battleship->getIsReady());
    this->ships[1] = battleship;
    this->floatingShips.push_back(battleship);
    cout << "" << endl;
    this->grid->showGrid();
    cout << "" << endl;
}

void Player::setDestroyer() {
    Destroyer* destroyer = new Destroyer(this->grid);
    do {
        string start_space;
        char direction;
        cout << "Enter Destroyer Start Space: ";
        cin >> start_space;
        while(!Spaces::isSpaceString(start_space) || this->grid->isNoGoSpace(start_space)) {
            if(!Spaces::isSpaceString(start_space))
                cout << "Invalid Space." << endl;
            else
                cout << "Space is occupied by or adjacent to an existing Ship. Please choose another space." << endl;
            cout << "Enter Destroyer Start Space: ";
            cin >> start_space;
        }
        cout << "Enter Destroyer Direction: ";
        cin >> direction;
        while (!(direction == 'N' || direction == 'S' || direction == 'E' || direction == 'W')) {
            cout << "Invalid Direction." << endl;
            cout << "Enter Destroyer Direction: ";
            cin >> direction;
        }
        destroyer->setIsReady(destroyer->placeOnGrid(start_space, direction));
    } while (!destroyer->getIsReady());
    this->ships[2] = destroyer;
    this->floatingShips.push_back(destroyer);
    cout << "" << endl;
    this->grid->showGrid();
    cout << "" << endl;
}

void Player::setSubmarine() {
    Submarine* submarine = new Submarine(this->grid);
    do {
        string start_space;
        char direction;
        cout << "Enter Submarine Start Space: ";
        cin >> start_space;
        while(!Spaces::isSpaceString(start_space) || this->grid->isNoGoSpace(start_space)) {
            if(!Spaces::isSpaceString(start_space))
                cout << "Invalid Space." << endl;
            else
                cout << "Space is occupied by or adjacent to an existing Ship. Please choose another space." << endl;
            cout << "Enter Submarine Start Space: ";
            cin >> start_space;
        }
        cout << "Enter Submarine Direction: ";
        cin >> direction;
        while (!(direction == 'N' || direction == 'S' || direction == 'E' || direction == 'W')) {
            cout << "Invalid Direction." << endl;
            cout << "Enter Submarine Direction: ";
            cin >> direction;
        }
        submarine->setIsReady(submarine->placeOnGrid(start_space, direction));
    } while (!submarine->getIsReady());
    this->ships[3] = submarine;
    this->floatingShips.push_back(submarine);
    cout << "" << endl;
    this->grid->showGrid();
    cout << "" << endl;
}

void Player::setCruiser() {
    Cruiser* cruiser = new Cruiser(this->grid);
    do {
        string start_space;
        char direction;
        cout << "Enter Cruiser Start Space: ";
        cin >> start_space;
        while(!Spaces::isSpaceString(start_space) || this->grid->isNoGoSpace(start_space)) {
            if(!Spaces::isSpaceString(start_space))
                cout << "Invalid Space." << endl;
            else
                cout << "Space is occupied by or adjacent to an existing Ship. Please choose another space." << endl;
            cout << "Enter Cruiser Start Space: ";
            cin >> start_space;
        }
        cout << "Enter Cruiser Direction: ";
        cin >> direction;
        while (!(direction == 'N' || direction == 'S' || direction == 'E' || direction == 'W')) {
            cout << "Invalid Direction." << endl;
            cout << "Enter Cruiser Direction: ";
            cin >> direction;
        }
        cruiser->setIsReady(cruiser->placeOnGrid(start_space, direction));
    } while (!cruiser->getIsReady());
    this->ships[4] = cruiser;
    this->floatingShips.push_back(cruiser);
    cout << "" << endl;
    this->grid->showGrid();
    cout << "" << endl;
}

void Player::manuallySetShips() {
    this->setCarrier();
    this->setBattleship();
    this->setDestroyer();
    this->setSubmarine();
    this->setCruiser();
}

void Player::autoPutShip(Ship* ship, int(*rand_func)()) {
    char directions[] = {'N','S','E','W'};
    while(!ship->getIsReady()){
        string start_space = Spaces::spaceStrings[static_cast<size_t>(rand_func() % 100)];
        char direction = directions[static_cast<size_t>(rand_func() % 4)];
        ship->setIsReady(ship->placeOnGrid(start_space, direction, false));
    }
    this->ships[static_cast<size_t>(ship->getShipType())] = ship;
    this->floatingShips.push_back(ship);
}

void Player::autoSetShip(char ship_char, int(*rand_func)()) {
    switch (ship_char) {
        case 'A': {
            this->autoPutShip(new Carrier(this->grid), rand_func);
            break;
        }
        case 'B': {
            this->autoPutShip(new Battleship(this->grid), rand_func);
            break;
        }
        case 'C': {
            this->autoPutShip(new Cruiser(this->grid), rand_func);
            break;
        }
        case 'D': {
            this->autoPutShip(new Destroyer(this->grid), rand_func);
            break;
        }
        case 'S': {
            this->autoPutShip(new Submarine(this->grid), rand_func);
        }
    }
}

void Player::autoSetShips(int(*rand_func)()) {
    this->autoSetShip('A', rand_func);
    this->autoSetShip('B', rand_func);
    this->autoSetShip('D', rand_func);
    this->autoSetShip('S', rand_func);
    this->autoSetShip('C', rand_func);
}

void Player::askToSetShips(int(*rand_func)()) {
    char response;
    cout << "Auto set ships ? (y/n) > ";
    cin >> response;
    while (!(response == 'y' || response == 'n')) {
        cout << "Invalid response. Please enter y or n." << endl;
        cout << "Auto set ships (y/n) > ";
        cin >> response;
    }
    if(response == 'y')
        this->autoSetShips(rand_func);
    else
        this->manuallySetShips();
}

void Player::doTurn() {
    bool input_result;
    do {
        string user_input;
        cout << "> ";
        cin >> user_input;
        input_result = this->processInput(user_input);
    } while(!input_result);
}