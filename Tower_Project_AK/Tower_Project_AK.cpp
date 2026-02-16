#include <iostream>
#include <vector>
#include <string>

using namespace std;

const int EMPTY = 0;
const int WALL = 1;
const int GOAL = 2;
const int KEY = 3;
const int DOOR = 4;
const int ENEMY = 5;
const int HEALTH = 6;
const int STR = 7;
const int DEF = 8;
const int PORT = 9;

class Player {
public:
    int key;
    int def;
    int str;
    int HP;
    int row, col;

    Player() {
        reset();
        row = 0;
        col = 0;
    }

    void reset() {
        key = 0;
        def = 7;
        str = 8;
        HP = 40;
    }

};

class Dungeon {
private:
    int rows;
    int cols;
    int** grid;  //2D like vector vector
    string name;
    vector<int> teleR1, teleC1;
    vector<int> teleR2, teleC2;

public:
    Player player;

    void addTelePair(int r1, int c1, int r2, int c2) {
        if (r1 >= 0 && r1 < rows && c1 >= 0 && c1 < cols &&
            r2 >= 0 && r2 < rows && c2 >= 0 && c2 < cols) {

            grid[r1][c1] = PORT;
            grid[r2][c2] = PORT;

            teleR1.push_back(r1);
            teleC1.push_back(c1);
            teleR2.push_back(r2);
            teleC2.push_back(c2);
        }
    }

    Dungeon(int r, int c, string n) {
        rows = r;
        cols = c;
        name = n;

        grid = new int* [rows];
        for (int i = 0; i < rows; i++) {
           grid[i] = new int[cols];
            for (int j = 0; j < cols; j++)
                grid[i][j] = EMPTY;
        }
    }
    //empty grid 0
    ~Dungeon() {
        for (int i = 0; i < rows; i++)
            delete[] grid[i];
        delete[] grid;
    }
    string getName() {
        return name; 
    }

    void setName(const string& n) {// for dougeon editor
        name = n;
    }
    void placePlayer(int r, int c) {
        player.row = r;
        player.col = c;
    }

    void setTile(int r, int c, int type) {
        grid[r][c] = type;
    }

    void display() {
        cout << endl << "--- " << name << " ---" << endl;//regular play
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                if (i == player.row && j == player.col)
                    cout << "@ ";
                else {
                    int tile = grid[i][j];//changing to symbols
                    if (tile == EMPTY) { 
                        cout << ". "; 
                    }
                    else if (tile == WALL) { 
                        cout << "# "; 
                    }
                    else if (tile == GOAL) { 
                        cout << "G "; 
                    }
                    else if (tile == KEY) {
                        cout << "K "; 
                    }
                    else if (tile == DOOR) {
                        cout << "D "; 
                    }
                    else if (tile == ENEMY) {
                        cout << "E "; 
                    }
                    else if (tile == HEALTH) {
                        cout << "H "; 
                    }
                    else if (tile == STR) {
                        cout << "S ";
                    }

                    else if (tile == DEF) {
                        cout << "F ";
                    }
                    else if (tile == PORT) {
                        cout << "T ";
                    }
                    else {
                        cout << "? ";  // error val
                    }

                }
            }
            cout << endl;
        }

        cout << "\nHP: " << player.HP
            << " STR: " << player.str
            << " DEF: " << player.def
            << endl;
        cout << "Keys:[ ";
        for (int k = 0; k < player.key; k++) {
            cout << "# ";
        }
        cout << ']' << endl;
    }
    void display_edit() {//only differance is the numbers for ease of placing in the editor.
        cout << endl << "--- " << name << " ---" << endl;

          cout << "  ";
          for (int j = 0; j < cols; j++)
              cout << j << " ";
          cout << endl;
  
        for (int i = 0; i < rows; i++) {
            cout << i << " ";
            for (int j = 0; j < cols; j++) {
                if (i == player.row && j == player.col)
                    cout << "@ ";
                else {
                    int tile = grid[i][j];
                    if (tile == EMPTY) {
                        cout << ". ";
                    }
                    else if (tile == WALL) {
                        cout << "# ";
                    }
                    else if (tile == GOAL) {
                        cout << "G ";
                    }
                    else if (tile == KEY) {
                        cout << "K ";
                    }
                    else if (tile == DOOR) {
                        cout << "D ";
                    }
                    else if (tile == ENEMY) {
                        cout << "E ";
                    }
                    else if (tile == HEALTH) {
                        cout << "H ";
                    }
                    else if (tile == STR) {
                        cout << "S ";
                    }

                    else if (tile == DEF) {
                        cout << "F ";
                    }
                    else if (tile == PORT) {
                        cout << "T ";
                    }
                    else {
                        cout << "? ";  // error val
                    }

                }
            }
            cout << endl;
        }
    }

    bool battle() {//stats for enemys defalt: note might want rand later bool is fine just might want to make enemy class to then use and add to.
        int EHP = 20;
        int ene_str = 8;
        int ene_def = 3;

        while (player.HP > 0 && EHP > 0) {

            int dmg = player.str - ene_def;
            if (dmg < 0) dmg = 0;
            EHP -= dmg;

            if (EHP <= 0) break;

            int ene_Dmg = ene_str - player.def;
            if (ene_Dmg < 0) ene_Dmg = 0;
            player.HP -= ene_Dmg;
        }

        if (player.HP <= 0) {
            cout << "You were defeated!\n";
            return false;
        }

        cout << "Enemy defeated!\n";
        return true;
    }

    bool movePlayer(char dir) {
        int newRow = player.row;
        int newCol = player.col;

        if (dir == 'w' || dir == 'W') { newRow--; }
        else if (dir == 's' || dir == 'S') { newRow++; }
        else if (dir == 'a' || dir == 'A') { newCol--; }
        else if (dir == 'd' || dir == 'D') { newCol++; }
        else {
            cout << "Invalid direction!\n";
            return true;
        }
        // start of tile interactions
        if (newRow < 0 || newRow >= rows || newCol < 0 || newCol >= cols) {
            cout << "Out of bounds!\n";
            return true;
        }

        int tile = grid[newRow][newCol];

        if (tile == WALL) {
            cout << "You hit a wall\n";
            return true;
        }

        if (tile == KEY) {
            cout << "Picked up a key\n";
            player.key++;
            grid[newRow][newCol] = EMPTY;
        }

        if (tile == DOOR) {
            if (player.key > 0) {
                cout << "Unlocked door!\n";
                player.key--;
                grid[newRow][newCol] = EMPTY;
            }
            else {
                cout << "Door locked!\n";
                return true;
            }
        }

        if (tile == HEALTH) {
            cout << "Health +10\n";
            player.HP += 10;
            grid[newRow][newCol] = EMPTY;
        }

        if (tile == STR) {
            cout << "Strength +5\n";
            player.str += 5;
            grid[newRow][newCol] = EMPTY;
        }

        if (tile == DEF) {
            cout << "Defense +5\n";
            player.def += 5;
            grid[newRow][newCol] = EMPTY;
        }

        if (tile == ENEMY) {
            if (!battle()) return false;
            grid[newRow][newCol] = EMPTY;
        }

        if (tile == GOAL) {
            cout << "You escape\n";
            return false;
        }
        if (tile == PORT) {
            for (int i = 0; i < teleR1.size(); i++) {
                if (newRow == teleR1[i] && newCol == teleC1[i]) { //first
                    cout << "You step on a teleporter!\n";
                    player.row = teleR2[i];
                    player.col = teleC2[i];
                    return true;
                }
                if (newRow == teleR2[i] && newCol == teleC2[i]) {//other
                    cout << "You step on a teleporter!\n";
                    player.row = teleR1[i];
                    player.col = teleC1[i];
                    return true;
                }
            }
        }
        // end of tile interactions
        player.row = newRow;
        player.col = newCol;
        return true;
    }
};


void playDungeon(Dungeon* d) {
    d->player.reset();
    bool run = true;

    while (run) {
        d->display();
        cout << endl << "Move (W/A/S/D): ";

        char move;
        bool loop;

        do {
            cin >> move;

            if (cin.fail()) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                loop = false;
                cout << "\nInvalid character! Please enter W, A, S, or D\n";
            }
            else {
                cin.ignore(numeric_limits<streamsize>::max(), '\n'); // <-- FIX
                move = toupper(move);

                if (move == 'W' || move == 'A' || move == 'S' || move == 'D')
                    loop = true;
                else {
                    cout << "\nInvalid direction! Use W, A, S, or D\n";
                    loop = false;
                }
            }

        } while (!loop);

        run = d->movePlayer(move);

        if (d->player.HP <= 0)
            run = false;
    }

    cout << "Returning to menu..." << endl;
}


// makes the first 3 levels
Dungeon* createBasic() {
    Dungeon* d = new Dungeon(5, 5, "Tutorial");
    d->placePlayer(0, 0);
    d->setTile(1, 1, WALL);
    d->setTile(2, 2, ENEMY);
    d->setTile(4, 4, GOAL);
    d->setTile(0, 2, KEY);
    d->setTile(0, 3, DOOR);
    return d;
}

Dungeon* createLV2() {
    Dungeon* d = new Dungeon(6, 6, "1st FL Tower");
    d->placePlayer(0, 0);
    d->setTile(1, 1, ENEMY);
    d->setTile(2, 2, ENEMY);
    d->setTile(3, 1, HEALTH);
    d->setTile(5, 5, GOAL);
    d->setTile(0, 3, KEY);
    d->setTile(0, 4, DOOR);
    return d;
}
Dungeon* createPort() {
    Dungeon* d = new Dungeon(6, 6, "Port test");
    d->placePlayer(0, 0);
    d->addTelePair(1, 1, 2, 2);
    d->setTile(3, 1, HEALTH);
    d->setTile(5, 5, GOAL);
    d->setTile(0, 3, KEY);
    d->setTile(0, 4, DOOR);
    return d;
}


Dungeon* createCustomDungeon() {
    int r, c;
    int loop;
    cout << "Enter rows: ";
    do {
        cin >> r;

        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            loop = false;
            cout << "\nInvalid row. Please enter an integer" << endl;
        }
        else {
            loop = true;
        }
    } while (!loop);
    cout << "Enter cols: ";
    do {
        cin >> c;

        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            loop = false;
            cout << "\nInvalid column. Please enter an integer" << endl;
        }
        else {
            loop = true;
        }
    } while (!loop);

    Dungeon* d = new Dungeon(r, c, "Custom");

    int playr, playc;
    cout << "Player start row: ";
    do {
        cin >> playr;

        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            loop = false;
            cout << "\nInvalid row. Please enter an integer" << endl;
        }
        else {
            loop = true;
        }
    } while (!loop);
    cout << "Player start col: ";
    do {
        cin >> playc;

        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            loop = false;
            cout << "\nInvalid column. Please enter an integer" << endl;
        }
        else {
            loop = true;
        }
    } while (!loop);
   
    
    if (playr >= 0 && playr < r && playc >= 0 && playc < c)//checks for out of bounds
        d->placePlayer(playr, playc);
    else {
        cout << "out of bounds: Defaulting to (0,0)" << endl;;
        d->placePlayer(0, 0);
    }

    bool editing = true;

    while (editing) {
        d->display_edit();

        cout << endl << "1) Add Object" << endl;
        cout << "2) Save Dungeon" << endl;
        cout << "Choice: ";

        int choice;
        cin >> choice;

        if (choice == 1) {
            cout << "\n1) Wall\n2) Goal\n3) Key\n4) Door\n5) Enemy\n6) Health\n7) Strength\n8) Defense\n9) Teleporter";
            cout << "Select object: ";

            int obj;
           
            do {
                cin >> obj;

                if (cin.fail()||obj<1||obj>9) {
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    loop = false;
                    cout << "\nInvalid Object" << endl;
                }
                else {
                    loop = true;
                }
            } while (!loop);
            if (obj == PORT) {

                int r1, c1, r2, c2;

                cout << "First teleporter row: ";
                cin >> r1;
                cout << "First teleporter col: ";
                cin >> c1;

                cout << "Second teleporter row: ";
                cin >> r2;
                cout << "Second teleporter col: ";
                cin >> c2;

                if (r1 >= 0 && r1 < r && c1 >= 0 && c1 < c &&
                    r2 >= 0 && r2 < r && c2 >= 0 && c2 < c) {

                    d->addTelePair(r1, c1, r2, c2);
                }
                else {
                    cout << "Invalid teleporter positions!" << endl;
                }
            }
            else {

                int row, col;

                cout << "Row: ";
                cin >> row;
                cout << "Col: ";
                cin >> col;

                if (row >= 0 && row < r && col >= 0 && col < c)
                    d->setTile(row, col, obj);
                else
                    cout << "Invalid position" << endl;
            }

        }
        else if (choice == 2) {
            cout << "Enter dungeon name: ";
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            string newName;
            getline(cin, newName);
            d->setName(newName);
            editing = false;
        }
    }

    return d;
}
int main()
{
    vector<Dungeon*> dungeons;

    dungeons.push_back(createBasic());
    dungeons.push_back(createLV2());
    dungeons.push_back(createPort());

    bool run = true;
    while (run) {
        cout << "\nwelcome to Halos Dungeon\n";
        cout << "1) Enter Dungeon" << endl;
        cout << "2) Edit Your own dungeon" << endl;
        cout << "3) Leave" << endl;
        cout << "Choice:  ";
        int choose;
        int loop;
        do {
            cin >> choose;

            if (cin.fail() || choose < 1 || choose >3) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                loop = false;
                cout << "\nInvalid choice! Please enter 1,2, or 3" << endl;
            }
            else {
                loop = true;
            }
        } while (!loop);

        if (choose == 1) {
            for (int i = 0; i < dungeons.size(); i++)
                cout << i + 1 << ") " << dungeons[i]->getName() << endl;

            cout << "Select dungeon: ";
            int pick;
            cin >> pick;

            if (pick > 0 && pick <= dungeons.size())
                playDungeon(dungeons[pick - 1]);
        }
        else if (choose == 2) {
            Dungeon* custom = createCustomDungeon();
            dungeons.push_back(custom);
        }
        else if (choose == 3) {
            run = false;
        }
        


   }
}

/*do {
		cin >> player_char;
		
		if (cin.fail() || player_char=='*' || player_char == '-') {
			cin.clear();
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
			choice = false;
			cout << "\nInvalid character! Please enter an avalable character" << endl;
		}
		else {
			choice = true;
		}
	} while (!choice);
	
*/