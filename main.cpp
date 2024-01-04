# include <jsoncpp/json/json.h>
# include <ncurses.h>
# include <iostream>
# include <fstream>
# include <string>

# define SHARE_DIRECTORY "/usr/local/share/bcc"

using namespace std;

WINDOW * makeMenu(int width, int height);
Json::Value fetchDatabase(string filename),
 catMenu(Json::Value cats);
void exitWindow(int errorcode),
 viewUnitLoop(Json::Value units),
 viewCat(Json::Value cats),
 stageMenu(Json::Value chapter, int degree),
 viewStage(Json::Value stage);
int wmake_MCM(WINDOW * menu, string * options, int length);

int highlight = 0, xMax, yMax, cats_CL = 0, choice = 0, cat_C, exitStatMenu = 0, unitmode, return_TMM;

int main() {
    setlocale(LC_ALL, "");
    initscr();
    getmaxyx(stdscr, yMax, xMax);
    curs_set(0);
    WINDOW * mainMenu = makeMenu(10, 5);
    string options[3] = {"cats", "enemies", "stages"};
    string filler = "", secondaryFiller = "";
    for (int i = 0; i < (xMax / 2.3); i++, filler += " ");
    for (int i = 0; i < xMax / 3.9; i++, secondaryFiller += " ");
    while (true) {
        printw("%s[CONTROLS]\n\
%s\u2190 \u2191           | Navigate Up \n\
%s\u2192 \u2193           | Navigate Down\n\
%s[ENTER]       | Menu Select / Level Magnification\n\
%s[F2, F3, F4]  | Confirm Search / Change Magnification\n\
%s[TAB]         | Go Back\n\
%s[SHIFT + TAB] | Exit Menu\n\
", filler.c_str(), secondaryFiller.c_str(), secondaryFiller.c_str(), secondaryFiller.c_str(), secondaryFiller.c_str(), secondaryFiller.c_str(), secondaryFiller.c_str());
        refresh();
        box(mainMenu, 0, 0);
        unitmode = wmake_MCM(mainMenu, options, 3);
        wrefresh(mainMenu);
        clear();
        refresh();
        return_TMM = 0;
        highlight = 0;
        choice = 0;
        cats_CL = 0;
        switch (unitmode) {
            case (0): { //Cats
                Json::Value cats = fetchDatabase("database.json");
                viewUnitLoop(cats["sampledata"]);
                break;
            } case (1): { //Enemies
                Json::Value enemies = fetchDatabase("enemies.json");
                viewUnitLoop(enemies["sampledata"]);
                break;
            } case (2): { //Stages
                Json::Value stages = fetchDatabase("stages.json");
                stageMenu(stages["sampledata"], 0);
                break;
            }
        }
    }
    return 0;
}

void stageMenu(Json::Value chapter, int degree) {
    int width, height;
    initscr();
    width = xMax - (xMax / 8);
    height = yMax - (yMax / 5);
    WINDOW * menu = makeMenu(width, height);
    string filler = "";
    for (int i = 0; i < (width / 2); i++, filler += " ");
    if (degree == 0) { printw("%sCHAPTERS", filler.c_str()); }
    refresh();
    filler = "";
    for (int i = 0; i < width - 2; i++, filler += " ");
    int stage_CL = 0, highlight = 0, choice = 0;
    WINDOW * searchBox = newwin(3, width, (yMax - height / 10), xMax / 16);
    box(searchBox, 0, 0);
    keypad(searchBox, true);
    wrefresh(searchBox);
    string searchBuffer;
    while (true) {
        for (int i = 0, l = stage_CL; i < yMax - 6; i++, l++) {
            string chapterName = (
                degree == 0 ? 
                    (l == 4 || l == 9 ? chapter[l][0]["category"].asString() : chapter[l][0][0]["category"].asString()) 
                :
                    (
                        degree == -4 ?
                            chapter[l]["chapter"].asString()
                        :
                            degree == 1 ?
                                chapter[l][0]["chapter"].asString()
                            :
                                chapter[l]["name"].asString()
                    )
            );
            if (highlight == i) {
                wattron(menu, A_REVERSE);
            } else {
                wattroff(menu, A_REVERSE);
            }
            mvwprintw(menu, i + 1, 1, filler.c_str());
            mvwprintw(menu, i + 1, 1, chapterName.c_str());
        }
        int input = wgetch(menu);
        if (input == KEY_UP || input == KEY_LEFT) {
            highlight--;
            choice--;
            if (highlight < 0) {
                highlight = 0;
                choice = stage_CL - (stage_CL == 0 ? 0 : 1);
                if (stage_CL > 0) {
                    stage_CL--;
                }
            }
        } else if (input == KEY_DOWN || input == KEY_RIGHT) {
            if (!chapter[choice + 1]) { continue; }
            highlight++;
            choice++;
            if (highlight >= height - 2) {
                stage_CL++; 
                highlight--;
            }
        } else if (input == 9 || input == 353) {
            if (degree == 0) { clear(); } 
            return;
        } else if (input == 10) {
            if (!chapter[choice]) { continue; }
            if (degree == 2) {
                viewStage(chapter[choice]);
            } else {
                stageMenu(chapter[choice], degree + (((choice == 4 || choice == 9) && degree == 0) ? -4 : 1));
            }
        } else if (input > 265 && input < 274 && searchBuffer.length()) {
            Json::Value filteredStages;
            int i = 0, l = 0;
            for (Json::Value stage_I : chapter) {
                string chapterName = (
                    degree == 0 ? 
                        (l == 4 || l == 9 ? chapter[l][0]["category"].asString() : chapter[l][0][0]["category"].asString()) 
                    :
                        (
                            degree == -4 ?
                                chapter[l]["chapter"].asString()
                            :
                                degree == 1 ?
                                    chapter[l][0]["chapter"].asString()
                                :
                                    chapter[l]["name"].asString()
                        )
                );
                if (chapterName.find(searchBuffer) != string::npos) {
                    filteredStages[i] = stage_I;
                    i++;
                }
                l++;
            }
            if (i) {
                highlight = 0;
                choice = 0;
                cats_CL = 0;
                clear();
                refresh();
                stageMenu(filteredStages, degree);
            }
        } else if (input == 263) {
            if (searchBuffer.length() < 0) { continue; }
            searchBuffer = searchBuffer.substr(0, searchBuffer.size() - 1);
            mvwprintw(searchBox, 1, searchBuffer.length() + 1, " ");
            wrefresh(searchBox);
        } else if (input == 8) {
            searchBuffer = "";
            mvwprintw(searchBox, 1, 1, filler.c_str());
            wrefresh(searchBox);
        } else if (searchBuffer.length() < width - 2) {
            searchBuffer += (char) input;
            mvwprintw(searchBox, 1, 1, searchBuffer.c_str());
            wrefresh(searchBox);
        }
        wrefresh(menu);
    }
}

void viewStage(Json::Value stage) {
    Json::Value enemies = fetchDatabase("enemies.json")["sampledata"];
    int width = xMax - (xMax / 8),
    height = yMax - (yMax / 5);
    WINDOW * menu = makeMenu(width, height);
    choice = 0;
    int coordinates[][2] = {
        {1, 1},
        {1, 15},
        {2, 1},
        {2, 15},
        {3, 1},
        {3, 15},
        {5, 1},
        {5, 15},
        {6, 1},
        {6, 15},
        {7, 1},
        {7, 15},
        {9, 2}
    };
    string data[] = {
        "Stage Name",
        stage["name"].asString(),
        "Energy Cost",
        stage["energy"].asString(),
        "XP Reward",
        stage["xp"].asString(),
        "Base Health",
        stage["base_hp"].asString(),
        "Enemy Limit",
        stage["max_enemy"].asString(),
        "Stage Size",
        stage["length"].asString(),
        "Enemy                | Mag %  | Total | Base% | Timing | Boss?"
    };
    WINDOW * enemyWindow = newwin((height / 2) - 1, width - 2, (height / 2) + (height / 8), ((xMax - width) - xMax / 16) + 1);
    box(enemyWindow, 0, 0);
    wrefresh(enemyWindow);
    int enemies_CL = 0, highlighter = 0;
    string filler = "";
    for (int i = 0; i < (width - 4); i++, filler += " ");
    while (true) {
        clear();
        refresh();
        box(menu, 0, 0);        
        box(enemyWindow, 0, 0);
        wrefresh(menu);
        wrefresh(enemyWindow);
        for (int i = 0; i < sizeof(coordinates) / sizeof(*coordinates); i++) {
            mvwprintw(menu, coordinates[i][0], coordinates[i][1], data[i].c_str());
        }
        for (int i = 0, l = enemies_CL; i < (height / 2) - 3; i++, l++) {
            if (!stage["enemies"][l]) { continue; }
            if (highlighter == i) {
                wattron(enemyWindow, A_REVERSE);
            } else {
                wattroff(enemyWindow, A_REVERSE);
            }
            Json::Value enemy = stage["enemies"][l];
            string name = enemy["enemy_name_en"].asString(),
             magnification = enemy["amplification"].asString(), 
             amount = enemy["number"].asString(),
             base_T = enemy["base_trigger"].asString(),
             time_T = enemy["time_trigger"].asString();
            while (name.length() < 20) { name += " "; }
            while (amount.length() < 5) { amount += " "; }
            while (base_T.length() < 5) { base_T += " "; }
            while (time_T.length() < 6) { time_T += " "; }
            while (magnification.length() < 5) { magnification += " "; }
            name += " | " + magnification + " | " + amount + " |  " + base_T + " | " + time_T + " | " + (enemy["is_boss"].asString() == "0" ? "False" : "True");
            mvwprintw(enemyWindow, i + 1, 1, filler.c_str());
            mvwprintw(enemyWindow, i + 1, 1, name.c_str());
        }
        wrefresh(menu);
        wrefresh(enemyWindow);
        int input = wgetch(menu);
        if (input == KEY_UP) {
            highlighter--;
            choice--;
            if (highlighter < 0) {
                highlighter = 0;
                choice = enemies_CL - (enemies_CL == 0 ? 0 : 1);
                if (enemies_CL > 0) {
                    enemies_CL--;
                }
            }
        } else if (input == KEY_DOWN) {
            if (!stage["enemies"][choice + 1]) { continue; }
            highlighter++;
            choice++;
            if (highlighter >= (height / 2) - 3) {
                highlighter--;
                enemies_CL++;
            }
        } else if (input == 9 || input == 353) {
            return;
        } else if (input == 10) {
            for (cat_C = 0; enemies[cat_C]["name"].asString() != stage["enemies"][choice]["enemy_name_en"].asString(); cat_C++);
            viewCat(enemies);
        }
    }
}

Json::Value catMenu(Json::Value cats) {
    int width, height;
    initscr();
    width = xMax - (xMax / 8);
    height = yMax - (yMax / 5);
    clear();
    printw("\n");
    for (int i = 0; i < xMax / 16; i++) { printw(" "); }
    printw("  ID   |               NAME                  |  HP   |  DMG  | SPEED |");
    refresh();
    WINDOW * menu = makeMenu(width, height);
    string filler = "", searchBuffer = "";
    for (int i = 0; i < width - 2; i++, filler += " ");
    WINDOW * searchBox = newwin(3, width, (yMax - height / 10), xMax / 16);
    box(searchBox, 0, 0);
    keypad(searchBox, true);
    wrefresh(searchBox);
    while (true) {
        for (int i = 0, l = cats_CL; i < yMax - 6; i++, l++) {
            if (!cats[l]) { continue; }
            Json::Value cat = cats[l];
            if (highlight == i) {
                wattron(menu, A_REVERSE);
            } else {
                wattroff(menu, A_REVERSE);
            }
            string key = cat["key"].asString() + (unitmode ? "  " : ""),
             name = cat["name"].asString(), health = cat["health"].asString(), damage = cat["damage"].asString(), speed = cat["speed"].asString();
            while (name.length() < 35) { name += " "; }
            while (health.length() < 5) { health += " "; }
            while (damage.length() < 5) { damage += " "; }
            string unit = key + " | " + name + " | " + health + " | " + damage + " | " + speed;
            mvwprintw(menu, i + 1, 1, filler.c_str());
            mvwprintw(menu, i + 1, 1, unit.c_str());
        }
        int input = wgetch(menu);
        if (input == KEY_UP || input == KEY_LEFT) {
            highlight--;
            choice--;
            if (highlight < 0) {
                highlight = 0;
                choice = cats_CL - (cats_CL == 0 ? 0 : 1);
                if (cats_CL > 0) {
                    cats_CL--;
                }
            }
        } else if ((input == KEY_DOWN || input == KEY_RIGHT)) {
            if (!cats[choice + 1]) { continue; }
            highlight++;
            choice++;
            if (highlight >= height - 2) {
                cats_CL++; 
                highlight--;
            }
        } else if (input == 353) {
            return_TMM = 1;
            clear();
            refresh();
            wrefresh(menu);
            return NULL;
        } else if (input == 10) {
            cat_C = choice + 1;
            return cats[choice];
        } else if (input > 265 && input < 274 && searchBuffer.length()) {
            Json::Value filteredCats;
            int i = 0;
            for (Json::Value catUnit : cats) {
                if (catUnit["name"].asString().find(searchBuffer) != string::npos) {
                    filteredCats[i] = catUnit;
                    i++;
                }
            }
            if (i) {
                highlight = 0;
                choice = 0;
                cats_CL = 0;
                return catMenu(filteredCats);
            }
        } else if (input == 263) {
            if (searchBuffer.length() < 0) { continue; }
            searchBuffer = searchBuffer.substr(0, searchBuffer.size() - 1);
            mvwprintw(searchBox, 1, searchBuffer.length() + 1, " ");
            wrefresh(searchBox);
        } else if (input == 8) {
            searchBuffer = "";
            mvwprintw(searchBox, 1, 1, filler.c_str());
            wrefresh(searchBox);
        } else if (searchBuffer.length() < width - 2) {
            searchBuffer += (char) input;
            mvwprintw(searchBox, 1, 1, searchBuffer.c_str());
            wrefresh(searchBox);
        }
        wrefresh(menu);
    }
}

void viewCat(Json::Value cats) {
    Json::Value cat = cats[cat_C];
    int width = xMax - (xMax / 5), height = yMax - (yMax / 3);
    WINDOW * menu = makeMenu(width, height);
    int level = (unitmode ? 100 : 30), input, highlight = 0, printAbilities = 0,
    coordinates[][2] = {
        {7, 1},
        {7, 3},
        {7, 5},
        {7, 6},
        {1, 1},
        {2, 1},
        {3, 1},
        {4, 1},
        {5, 2},
        {8, 1},
        {8, 11},
        {10, 1},
        {10, 11},
        {11, 1},
        {11, 11},
        {12, 1},
        {12, 11},
        {13, 1},
        {13, 11},
        {10, 19},
        {10, 29},
        {11, 19},
        {11, 29},
        {12, 19},
        {12, 29},
        {13, 19},
        {13, 29},
        {10, 38},
        {10, 50},
        {2, 10},
        {11, 38},
        {11, 50}
    };
    string ability = cat["ability"].asString();
    for (int i = 0; i < ability.length(); i++) {
        if ((ability[i] == 'u' || ability[i] == 'b') && (ability[i + 1] == '1' || ability[i + 1] == '2')) {
            ability.erase(i, 2);
        } if (ability[i] == '\n') {
            ability.insert(i + 1, 1, ' ');
        } if (ability[i] == ':' || ability[i] == ',') {
            ability.insert(i + 1, 1, '\n');
            ability.insert(i + 2, 1, ' ');
        }
    }
    string data[] = {
        "\u21e4",
        "\u2190",
        "\u2192",
        "\u21e5",
        cat["name"].asString(),
        cat["key"].asString(),
        " /\\_/\\", 
        "( o.o )", 
        "> ^ <",
        !unitmode ? "[LEVEL] [    ]" : "[MAG %] [     ]",
        "1",
        "[HEALTH]",
        "",
        "[DAMAGE]",
        "",
        "[DPS]",
        "",
        "[RANGE]",
        cat["range"].asString(),
        "[KB]",
        cat["kb"].asString(),
        "[Target]",
        cat["target"].asString(),
        "[TBA]",
        cat["tba"].asString(),
        "[SPEED]",
        cat["speed"].asString(),
        !unitmode ? "[COOLDOWN]" : " ",
        cat["spawn"].asString(),
        cat["rarity"].asString(),
        "[COST]",
        cat["cost"].asString(),
    };
    WINDOW * abilityWindow = newwin(height / 2, width / 2, (yMax - height / 2) / 4, (xMax - width / 2) / 1.2);
    keypad(abilityWindow, true);
    wrefresh(abilityWindow);
    string replacements_P[] = { "health", "damage", "dps" };
    while (true) {
        data[10] = to_string(level);
        for (int i = 12, l = 0; l < 3; i += 2, l++) {
            int key = ((data[5][0]) - 48) * 100 + ((data[5][1] - 48) * 10) + (data[5][2] - 48);
            mvwprintw(menu, coordinates[i][0], coordinates[i][1], "       ");
            if (!unitmode) {
                int m = (
                    (data[29] == "Special" || data[29] == "Basic") && key != 26 ? 60 :
                    key == 26 ? 30 :
                    data[29] == "Rare" ? 70 :
                    key > 91 && key < 101 ? 20 :
                    data[29] == "Super Rare" || data[29] == "Uber Super Rare" ? 60 :
                    100
                ), p = (
                    data[29] == "Rare" ? 90 :
                    data[29] == "Super Rare" ? 80 :
                    data[29] == "Uber Super Rare" ? 80 :
                    200 
                );
                data[i] = to_string(
                        level > 1 && level <= m ? (int) (cat[replacements_P[l]].asInt() * ((level + 4) / 5.0)) : 
                        level > m && level < p ? (int) (cat[replacements_P[l]].asInt() * (((m + 4) / 5.0) + ((level - m) / 10))) :
                        level > p ? (int) (cat[replacements_P[l]].asInt() * (((m + 4) / 5.0) + ((p - m) / 10) + ((level - p) / 20))) :
                        cat[replacements_P[l]].asInt()
                );
            } else {
                data[i] = to_string((int) (cat[replacements_P[l]].asInt() * (level / 100.0)));
            }
        }
        for (int i = 0; i < sizeof(coordinates) / sizeof(*coordinates); i++) {
            if (highlight == i) {
                wattron(menu, A_REVERSE);
            } else {
                wattroff(menu, A_REVERSE);
            }
            mvwprintw(menu, coordinates[i][0], coordinates[i][1], data[i].c_str());
        }
        string ability_P = "";
        for (int i = printAbilities; i < printAbilities + 85 && i < ability.length(); ability_P += ability[i], i++);
        for (int i = 0; i < (height / 2) - 2; i++) {
            string filler = "";
            for (int i = 0; i < (width / 2) - 2; i++, filler += " ");
            mvwprintw(abilityWindow, i, 1, filler.c_str());
        }
        mvwprintw(abilityWindow, 1, 1, ability_P.c_str());
        box(abilityWindow, 0, 0);
        wrefresh(abilityWindow);
        wrefresh(menu);
        input = wgetch(menu);
        if (input == KEY_LEFT) {
            highlight--;
            if (highlight < 0) { highlight = 3; }
        } else if (input == KEY_RIGHT) {
            highlight++;
            if (highlight > 3) { highlight = 0; }
        } else if (input == KEY_UP && printAbilities > 0) {
            printAbilities -= 10;
        } else if (input == KEY_DOWN && printAbilities < ability.length()) {
            printAbilities += 10;
        } else if (input == 10) {
            if (highlight == 1 && cat_C > 0) {
                cat_C--;
            } else if (highlight == 2) {
                cat_C++;
            } else if (highlight == 0 && cat_C > 0 && !unitmode) {
                for (cat_C--; ((cats[cat_C]["key"].asString()[4]) - 48) > 1; cat_C--);
            } else if (highlight == 3 && !unitmode) {
                for (cat_C++; ((cats[cat_C]["key"].asString()[4]) - 48) > 1 && cats[cat_C + 1]; cat_C++);
            }
            string filler = "";
            for (int i = 0; i < width - 2; i++, filler += " ");
            mvwprintw(menu, 1, 1, filler.c_str());
            viewCat(cats);
            cat_C += (highlight == 1 ? 1 : -1);
            if (exitStatMenu) { return; };
        } else if (input == 9) {
            return;
        } else if (input == 353) {
            exitStatMenu = 1;
            return;
        } else if (input > 265 && input < 274) {
            string level_AS = "";
            while (true) {
                int level_AI = wgetch(menu);
                if (level_AI == 10) {
                    level = stoi(level_AS);
                    break;
                } else if (level_AI == 263) {
                    if (level_AS.length() < 0) { continue; }
                    level_AS = level_AS.substr(0, level_AS.size() - 1);
                    mvwprintw(menu, coordinates[10][0], coordinates[10][1] + level_AS.length(), " ");
                } else {
                    mvwprintw(menu, coordinates[10][0], coordinates[10][1], "   ");
                    level_AS += level_AI;
                    mvwprintw(menu, coordinates[10][0], coordinates[10][1], level_AS.c_str());
                }
                wrefresh(menu);
            }
        }
        wrefresh(menu);
    }
}

WINDOW * makeMenu(int width, int height) {
    initscr();
      noecho();
      cbreak();
      WINDOW * menu = newwin(height, width, (yMax - height) / 2, (xMax - width) / 2);
      box(menu, 0, 0);
      keypad(menu, true);
      wrefresh(menu);
   return menu;
}

//I totally stole this from my own project (AM2R save manager)
int wmake_MCM(WINDOW * menu, string * options, int length) {
    int input, highlight = 0;
    while (true) {
        for (int i = 0; i < length; i++) {
                if (highlight == i) {
                    wattron(menu, A_REVERSE);
                } else {
                    wattroff(menu, A_REVERSE);
                }
                mvwprintw(menu, i + 1, 1, options[i].c_str());
            }
            input = wgetch(menu);
            if (input == KEY_UP || input == KEY_LEFT) {
                highlight--;
                if (highlight <= -1) { highlight = length - 1; };
            } else if (input == KEY_DOWN || input == KEY_RIGHT) {
                highlight++;
                if (highlight >= length) { highlight = 0; };
            } else if (input == 10) {
                break;
            } else if (input == 353) {
                exitWindow(0);
            }
            wrefresh(menu);
    }
    return highlight;
}

void exitWindow(int errorcode) {
    refresh();
    endwin();
    exit(errorcode);
}

Json::Value fetchDatabase(string filename) {
    Json::Value units;
    ifstream database((string) SHARE_DIRECTORY + "/" + filename, ifstream::binary);
    database >> units;
    return units;
}

void viewUnitLoop(Json::Value units) {
    while (true) {
        exitStatMenu = 0;
        Json::Value cat = catMenu(units);
        if (return_TMM) { break; };
        for (cat_C = 0; units[cat_C]["name"].asString() != cat["name"].asString(); cat_C++);
        viewCat(units);
    }
}
