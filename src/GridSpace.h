#ifndef GRIDSPACE_H
#define GRIDSPACE_H

#include <string>
    using std::string;

#include "Enums.h"
#include "Stud.h"

class Stud;

class GridSpace {
    private:
        SpaceName spaceName;
        Column column;
        Row row;
        PlayerType ofPlayer;
        SpaceStatus status {UNTARGETED};
        Stud* stud {nullptr};
        string spaceString;
        char label {'+'};
        char primeLabel {'+'};
        char letter;
        char number;
        void set_space_data(SpaceName space_name);

    public:
        GridSpace();
        explicit GridSpace(SpaceName space_name);
        GridSpace(SpaceName space_name, PlayerType of_player);
        GridSpace(SpaceName space_name, PlayerType of_player, SpaceStatus space_status);
        GridSpace(SpaceName space_name, PlayerType of_player, SpaceStatus space_status, Stud* the_stud);
        ~GridSpace();
        
        SpaceName getSpaceName() const;
        Column getColumn() const;
        Row getRow() const;
        PlayerType getOfPlayer() const;
        SpaceStatus getStatus() const;
        Stud* getStud() const;
        string getSpaceString() const;
        char getLetter() const;
        char getNumber() const;
        char getLabel() const;
        char getPrimeLabel() const;
        
        void setSpaceName(SpaceName space_name);
        void setOfPlayer(PlayerType of_player);
        void setStatus(SpaceStatus space_status);
        void setStud(Stud* the_stud);
        void setLabel(char label);
        
        void addStud(Stud* the_stud);

        bool hasStud() const;
        bool wasTargeted() const;

        void modifyLabel();
        
        TargetResult target();
};

#endif