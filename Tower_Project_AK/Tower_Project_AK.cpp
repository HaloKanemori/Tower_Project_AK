#include <iostream>
#include <vector>
#include <string>
#include <limits>
#include <memory> 

using namespace std;
string controlMode = "WASD";

string getLine() {
    string s;
    getline(cin, s);
    return s;
}


int safeInt() {
    int x;

    while (true) {
        cin >> x;

        if (!cin.fail()) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return x;
        }

        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        cout << "Invalid number. Try again: ";
    }
}

int safeIntRange(int lo, int hi) {
    while (true) {
        int x = safeInt();
        if (x >= lo && x <= hi) return x;

        cout << "Enter " << lo << " - " << hi << ": ";
    }
}
int safeIntIndef(int lo) {
    while (true) {
        int x = safeInt();
        if (x >= lo) return x;

        cout << "Enter " << lo << ": ";
    }
}
class Player {
public:
    int HP, str, def;
    int row, col;
    vector<string> keys;

    Player() { reset(); }

    void reset() {
        HP = 40;
        str = 8;
        def = 7;
        row = col = 0;
        keys.clear();
    }
};

string normalize(string s) {
    for (char& c : s)
        c = toupper(c);
    return s;
}

char parseDirection(string m) {
    m = normalize(m);

    if (controlMode == "WASD") {
        if (m == "W" || m == "UP") return 'W';
        if (m == "S" || m == "DOWN") return 'S';
        if (m == "A" || m == "LEFT") return 'A';
        if (m == "D" || m == "RIGHT") return 'D';
    }
    else { // LRUD
        if (m == "U" || m == "UP") return 'W';
        if (m == "D" || m == "DOWN") return 'S';
        if (m == "L" || m == "LEFT") return 'A';
        if (m == "R" || m == "RIGHT") return 'D';
    }

    return '?';
}

class Tile {
public:
    virtual char symbol() = 0;
    virtual string name() = 0;
    virtual unique_ptr<Tile> clone() const = 0;;
    virtual void inspect() {}

    virtual bool walkable() { return true; }
    virtual void onEnter(Player&, bool& running, bool& consumed) {}

    virtual ~Tile() {}
};

class EmptyTile : public Tile {
public:
    char symbol() { return '.'; }
    string name() { return "Empty"; }
    void inspect() {}
    unique_ptr<Tile> clone() const {
        return make_unique<EmptyTile>(*this);
    }
};

class WallTile : public Tile {
public:
    char symbol() { return '#'; }
    string name() { return "Wall"; }

    unique_ptr<Tile> clone() const {
        return make_unique<WallTile>(*this);
    }
    void inspect() {}
    bool walkable() { return false; }

};

class GoalTile : public Tile {
public:
    char symbol() { return 'G'; }
    string name() { return "Goal"; }
    void inspect() {}
    unique_ptr<Tile> clone() const {
        return make_unique<GoalTile>(*this);
    }

    void onEnter(Player&, bool& running, bool&) {
        cout << "YOU WIN!\n";
        running = false;
    }
};

class EnemyTile : public Tile {
public:
    int hp, atk, def;

    EnemyTile(int h, int a, int d) : hp(h), atk(a), def(d) {}

    char symbol() { return 'E'; }
    string name() { return "Enemy"; }
    void inspect() {
        cout << "Enemy -> HP: " << hp
            << " ATK: " << atk
            << " DEF: " << def << "\n";
    }
    unique_ptr<Tile> clone() const {
        return make_unique<EnemyTile>(*this);
    }

    void onEnter(Player& p, bool& running, bool& consumed) {
        cout << "Battle Start!\n";

        while (p.HP > 0 && hp > 0) {
            int playerDmg = max(0, p.str - def);
            hp -= playerDmg;
            cout << "You hit enemy for " << playerDmg << " damage\n";

            if (hp <= 0) break;

            int enemyDmg = max(0, atk - p.def);
            p.HP -= enemyDmg;
            cout << "Enemy hits you for " << enemyDmg << " damage\n";
        }

        if (p.HP <= 0) {
            cout << "You died!\n";
            running = false;
            return;
        }

        cout << "Enemy defeated!\n";
        consumed = true;
    }
};

class PotionTile : public Tile {
public:
    int hpB, strB, defB;

    PotionTile(int h, int s, int d)
        : hpB(h), strB(s), defB(d) {
    }
    char symbol() {
        if (hpB >= strB && hpB >= defB) return 'H';
        if (strB >= defB) return 'S';
        return 'F';
    }
    void inspect() {
        cout << "Potion -> HP+" << hpB
            << " STR+" << strB
            << " DEF+" << defB << "\n";
    }
    string name() { return "Potion"; }
    unique_ptr<Tile> clone() const {
        return make_unique<PotionTile>(*this);
    }

    void onEnter(Player& p, bool&, bool& consumed) {
        p.HP += hpB;
        p.str += strB;
        p.def += defB;
        consumed = true;
    }

};

class KeyTile : public Tile {
public:
    string color;
    KeyTile(string c) : color(c) {}

    char symbol() { return 'K'; }
    string name() { return "Key"; }
    void inspect() {
        cout << "Key color: " << color << "\n";
    }
    unique_ptr<Tile> clone() const {
        return make_unique<KeyTile>(*this);
    }

    void onEnter(Player& p, bool&, bool& consumed) {
        p.keys.push_back(color);
        consumed = true;
    }
};

class DoorTile : public Tile {
public:
    string color;
    bool open = false;

    DoorTile(string c) : color(c) {}

    char symbol() { return open ? '.' : 'D'; }
    string name() { return "Door"; }

    unique_ptr<Tile> clone() const {
        return make_unique<DoorTile>(*this);
    }

    bool walkable() { return open; }
    void inspect() {
        cout << "Door color: " << color
            << " (" << (open ? "OPEN" : "LOCKED") << ")\n";
    }

    void onEnter(Player& p, bool&, bool&) {
        for (int i = 0; i < p.keys.size(); i++) {
            if (p.keys[i] == color) {
                p.keys.erase(p.keys.begin() + i);
                open = true;
                cout << "Door opened!\n";
                return;
            }
        }
        cout << "Locked!\n";
    }
};

class DamageTile : public Tile {
public:
    string type;
    int dmg;

    DamageTile(string t) : type(t) {
        if (t == "lava") dmg = 5;
        else if (t == "spikes") dmg = 3;
        else if (t == "poison") dmg = 2;
        else dmg = 1;
    }

    char symbol() { return '!'; }
    string name() { return "Damage"; }

    unique_ptr<Tile> clone() const {
        return make_unique<DamageTile>(*this);
    }
    void inspect() {
        cout << "Damage: " << type
            << " (" << dmg << ")\n";
    }

    void onEnter(Player& p, bool&, bool&) {
        cout << "Took " << dmg << " damage!\n";
        p.HP -= dmg;
    }
};

class Dungeon {
public:
    int rows;
    int cols;
    string name;
    int startRow = 0;
    int startCol = 0;

    Player player;
    vector<vector<unique_ptr<Tile>>> grid;
    vector<vector<unique_ptr<Tile>>> original;

    Dungeon(int r, int c, string n)
        : rows(r), cols(c), name(n)
    {
        grid.resize(rows);
        original.resize(rows);

        for (int i = 0; i < rows; i++) {
            grid[i].resize(cols);
            original[i].resize(cols);

            for (int j = 0; j < cols; j++) {
                grid[i][j] = make_unique<EmptyTile>();
                original[i][j] = make_unique<EmptyTile>();
            }
        }
    }

    void inspect(int r, int c) {
        if (r < 0 || r >= rows || c < 0 || c >= cols) {
            cout << "Out of bounds\n";
            return;
        }

        cout << "Tile: " << grid[r][c]->name() << "\n";
        grid[r][c]->inspect();
    }

    void setTile(int r, int c, unique_ptr<Tile> t) {
        if (r < 0 || r >= rows || c < 0 || c >= cols) return;

        original[r][c] = t->clone();
        grid[r][c] = std::move(t);
    }

    void resetDungeon() {
        for (int i = 0; i < rows; i++)
            for (int j = 0; j < cols; j++)
                grid[i][j] = original[i][j]->clone();

        player.reset();
        player.row = startRow;
        player.col = startCol;
    }

    void showLegend() {
        cout << "KEY:";
        cout << "@ Player\n";
        cout << ". Empty\n";
        cout << "# Wall\n";
        cout << "G Goal\n";
        cout << "E Enemy\n";
        cout << "H HP Potion\n";
        cout << "S STR Potion\n";
        cout << "F DEF Potion\n";
        cout << "K Key (type-based: B=Blue, R=Red, G=Green)\n";
        cout << "D Door (must match key type)\n";
        cout << "! Damage Floor\n";

    }

    void display() {
        showLegend();

        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                if (i == player.row && j == player.col)
                    cout << "@ ";
                else
                    cout << grid[i][j]->symbol() << " ";
            }
            cout << "\n";
        }

        cout << "HP:" << player.HP << " STR:" << player.str << " DEF:" << player.def << "\n";
        cout << "Keys: ";
        for (auto& k : player.keys) cout << k << " ";
        cout << "\n";
    }
    void display_edit() {

        cout << "\n--- EDITOR: " << name << " ---\n\n";

        // column headers
        cout << "    ";
        for (int j = 0; j < cols; j++)
            cout << j << "   ";
        cout << "\n";

        for (int i = 0; i < rows; i++) {

            // row header
            cout << i << " | ";

            for (int j = 0; j < cols; j++) {

                if (i == player.row && j == player.col)
                    cout << "@   ";
                else
                    cout << grid[i][j]->symbol() << "   ";
            }

            cout << "\n";
        }

        cout << "\n";
    }

    bool move(char d) {
        int nr = player.row;
        int nc = player.col;

        if (d == 'W') nr--;
        if (d == 'S') nr++;
        if (d == 'A') nc--;
        if (d == 'D') nc++;

        if (nr < 0 || nr >= rows || nc < 0 || nc >= cols)
            return true;

        bool run = true;
        bool consumed = false;

        grid[nr][nc]->onEnter(player, run, consumed);

        if (!run) return false;
        if (player.HP <= 0) return false;

        if (consumed) {
            grid[nr][nc] = make_unique<EmptyTile>();
        }

        if (grid[nr][nc]->walkable()) {
            player.row = nr;
            player.col = nc;
        }
        return true;
    }

    void play() {
        resetDungeon();

        bool run = true;
        while (run) {
            display();

            cout << "\nMove (W/A/S/D or UP/DOWN/LEFT/RIGHT, QUIT, INSPECT): ";
            string m;
            cin >> m;

            if (normalize(m) == "QUIT") {
                cout << "Exiting dungeon...\n";
                break;
            }

            if (normalize(m) == "INSPECT") {
                cout << "Rows: ";
                int r = safeIntRange(0, rows - 1);
                cout << "Cols: ";
                int c = safeIntRange(0, cols - 1);
                inspect(r, c);
                continue;
            }

            char d = parseDirection(m);
            if (d == '?') continue;

            run = move(d);

            if (player.HP <= 0) run = false;
        }

        cout << "Game Over\n";
    }
};
vector<Dungeon*> dungeons;

Dungeon* findDungeon(string name) {
    for (auto d : dungeons)
        if (d->name == name) return d;
    return nullptr;
}

void showPlaceMenu() {
    cout << "\nPLACE OBJECTS\n";
    cout << "1) Wall\n";
    cout << "2) Enemy (HP ATK DEF)\n";
    cout << "3) Potion (HP STR DEF)\n";
    cout << "4) Key (color)\n";
    cout << "5) Door (color)\n";
    cout << "6) Goal\n";
    cout << "7) Damage Floor (type)\n";
    cout << "You can type number OR name (e.g. WALL, ENEMY)\n";

}

void editor() {
    cout << "Dungeon name: ";
    string name;
    cin >> name;

    Dungeon* d = findDungeon(name);

    if (!d) {
        cout << "Rows: ";
        int r = safeIntIndef(1);

        cout << "Cols: ";
        int c = safeIntIndef(1);

        d = new Dungeon(r, c, name);
        dungeons.push_back(d);
    }
    else {
        cout << "Dungeon exists. Overwriting...\n";

        cout << "Rows: ";
        int r = safeIntIndef(1);

        cout << "Cols: ";
        int c = safeIntIndef(1);

        for (int i = 0; i < dungeons.size(); i++) {
            if (dungeons[i]->name == name) {
                delete dungeons[i];
                dungeons[i] = new Dungeon(r, c, name);
                d = dungeons[i];
                break;
            }
        }
    }

    cout << "Start Row: ";
    int sr = safeIntRange(0, d->rows - 1);

    cout << "Start Col: ";
    int sc = safeIntRange(0, d->cols - 1);

    d->startRow = sr;
    d->startCol = sc;
    d->player.row = sr;
    d->player.col = sc;

    while (true) {
        d->display_edit();

        cout << "\n=== EDITOR MENU ===\n";
        cout << "1) Place\n";
        cout << "2) Inspect\n";
        cout << "3) Playtest\n";
        cout << "4) Exit\n";
        cout << "Choice: ";

        string choice;
        cin >> choice;
        choice = normalize(choice);

        if (choice == "1" || choice == "PLACE") {

            showPlaceMenu();
            cout << "Enter Place Row\n";
            int r = safeIntRange(0, d->rows - 1);
            cout << "\n Enter Place Col\n";
            int c = safeIntRange(0, d->cols - 1);
            cout << "\nSelect tile: ";
            string t;
            cin >> t;
            t = normalize(t);

            if (t == "1" || t == "WALL") {
                d->setTile(r, c, make_unique<WallTile>());
            }

            else if (t == "2" || t == "ENEMY") {
                cout << "Enter HP: ";
                int hp = safeIntIndef(1);

                cout << "Enter ATK: ";
                int atk = safeIntIndef(1);

                cout << "Enter DEF: ";
                int def = safeIntIndef(1);

                d->setTile(r, c, make_unique<EnemyTile>(hp, atk, def));
            }

            else if (t == "3" || t == "POTION") {
                cout << "Enter HP bonus: ";
                int hp = safeIntIndef(0);

                cout << "Enter STR bonus: ";
                int str = safeIntIndef(0);

                cout << "Enter DEF bonus: ";
                int def = safeIntIndef(0);

                d->setTile(r, c, make_unique<PotionTile>(hp, str, def));
            }

            else if (t == "4" || t == "KEY") {
                string color;
                cout << "Enter key color: ";
                cin >> color;

                d->setTile(r, c, make_unique<KeyTile>(color));
            }

            else if (t == "5" || t == "DOOR") {
                string color;
                cout << "Enter door color: ";
                cin >> color;

                d->setTile(r, c, make_unique<DoorTile>(color));
            }

            else if (t == "6" || t == "GOAL") {
                d->setTile(r, c, make_unique<GoalTile>());
            }

            else if (t == "7" || t == "DAMAGE") {
                cout << "Enter type (lava / spikes / poison): ";
                string type;
                cin >> type;

                d->setTile(r, c, make_unique<DamageTile>(type));
            }

            else {
                cout << "Invalid tile type.\n";
            }
        }

        else if (choice == "2" || choice == "INSPECT") {
            cout << "Rows: ";
            int r = safeIntRange(0, d->rows - 1);
            cout << "Cols: ";
            int c = safeIntRange(0, d->cols - 1);
            d->inspect(r, c);
        }


        else if (choice == "3" || choice == "PLAY" || choice == "PLAYTEST") {
            d->play();
            d->resetDungeon();
        }

        else if (choice == "4" || choice == "EXIT" || choice == "QUIT") {
            break;
        }

        else {
            cout << "Invalid option.\n";
        }
    }
}

Dungeon* createStandard() {
    Dungeon* d = new Dungeon(6, 6, "Standard Dungeon");

    d->setTile(1, 1, make_unique<WallTile>());


    d->setTile(3, 1, make_unique<EnemyTile>(18, 5, 2));

    d->setTile(2, 1, make_unique<PotionTile>(8, 2, 1));
    d->setTile(4, 2, make_unique<KeyTile>("red"));

    d->setTile(4, 3, make_unique<DoorTile>("red"));
    d->setTile(5, 5, make_unique<GoalTile>());

    d->player.row = 0;
    d->player.col = 0;

    return d;
}



Dungeon* selectDungeon(vector<Dungeon*>& dungeons) {

    cout << "\n=== SELECT DUNGEON ===\n";

    for (int i = 0; i < dungeons.size(); i++) {
        cout << i + 1 << ") " << dungeons[i]->name << "\n";
    }

    cout << "Enter number or name: ";

    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    string input;
    getline(cin, input);

    string normInput = normalize(input);

    bool isNumber = true;
    if (input.empty()) {
        isNumber = false;
    }
    else {
        for (char c : input) {
            if (c < '0' || c > '9') {
                isNumber = false;
                break;
            }
        }
    }

    if (isNumber) {
        int idx = stoi(input) - 1;
        if (idx >= 0 && idx < dungeons.size())
            return dungeons[idx];
    }

    for (auto d : dungeons) {
        if (d->name == input)
            return d;
    }

    cout << "Invalid selection.\n";
    return nullptr;
}


int main() {
    dungeons.push_back(createStandard());

    while (true) {
        cout << "\nwelcome to Halos Dungeon\n";
        cout << "1) Enter Dungeon\n";
        cout << "2) Edit Your Own Dungeon\n";
        cout << "3) Toggle Controls (Current: " << controlMode << ")\n";
        cout << "4) Leave\n";
        cout << "Choice: ";

        string input;
        cin >> input;
        input = normalize(input);

        if (input == "1" || input == "ENTER") {
            Dungeon* d = selectDungeon(dungeons);
            if (d != nullptr) d->play();
        }
        else if (input == "2" || input == "EDIT") {
            editor();
        }
        else if (input == "3" || input == "TOGGLE") {
            controlMode = (controlMode == "WASD") ? "LRUD" : "WASD";
            cout << "Controls switched to " << controlMode << "\n";
        }
        else if (input == "4" || input == "EXIT" || input == "QUIT") {
            break;
        }
        else {
            cout << "Invalid choice.\n";
        }
    }

    for (auto d : dungeons)
        delete d;
}
