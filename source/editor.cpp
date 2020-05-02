#include "editor.h"
#include <string.h>
#include <stdio.h>
#include <dirent.h>
#include <string>
#include <vector>
#include <switch.h>

static int cur_row = 0;
static int cur_col = 0;
static std::vector<std::string> list;

static bool keyboard_caps = false;
static bool keyboard_shift = false;

static const u32 keyboard_table[][8] = {
    {7, 12, 23, 28, '1', '1', '!', '!'},
    {12, 17, 23, 28, '2', '2', '@', '@'},
    {17, 22, 23, 28, '3', '3', '#', '#'},
    {22, 27, 23, 28, '4', '4', '$', '$'},
    {27, 32, 23, 28, '5', '5', '%', '%'},
    {32, 37, 23, 28, '6', '6', '^', '^'},
    {37, 42, 23, 28, '7', '7', '&', '&'},
    {42, 47, 23, 28, '8', '8', '*', '*'},
    {47, 52, 23, 28, '9', '9', '(', '('},
    {52, 57, 23, 28, '0', '0', ')', ')'},
    {57, 62, 23, 28, '-', '-', '_', '_'},
    {62, 67, 23, 28, '=', '=', '+', '+'},
    {2, 9, 28, 33, ' ', ' ', ' ', ' '},
    {9, 14, 28, 33, 'q', 'Q', 'Q', 'q'},
    {14, 19, 28, 33, 'w', 'W', 'W', 'w'},
    {19, 24, 28, 33, 'e', 'E', 'E', 'e'},
    {24, 29, 28, 33, 'r', 'R', 'R', 'r'},
    {29, 34, 28, 33, 't', 'T', 'T', 't'},
    {34, 39, 28, 33, 'y', 'Y', 'Y', 'y'},
    {39, 44, 28, 33, 'u', 'U', 'U', 'u'},
    {44, 49, 28, 33, 'i', 'I', 'I', 'i'},
    {49, 54, 28, 33, 'o', 'O', 'O', 'o'},
    {54, 59, 28, 33, 'p', 'P', 'P', 'p'},
    {59, 64, 28, 33, '[', '[', '{', '{'},
    {64, 69, 28, 33, ']', ']', '}', '}'},
    {69, 78, 28, 33, '\\', '\\', '|', '|'},
    {10, 15, 33, 38, 'a', 'A', 'A', 'a'},
    {15, 20, 33, 38, 's', 'S', 'S', 's'},
    {20, 25, 33, 38, 'd', 'D', 'D', 'd'},
    {25, 30, 33, 38, 'f', 'F', 'F', 'f'},
    {30, 35, 33, 38, 'g', 'G', 'G', 'g'},
    {35, 40, 33, 38, 'h', 'H', 'H', 'h'},
    {40, 45, 33, 38, 'j', 'J', 'J', 'j'},
    {45, 50, 33, 38, 'k', 'K', 'K', 'k'},
    {50, 55, 33, 38, 'l', 'L', 'L', 'l'},
    {55, 60, 33, 38, ';', ';', ':', ';'},
    {60, 65, 33, 38, '\'', '\'', '\"', '\"'},
    {11, 16, 38, 43, 'z', 'Z', 'Z', 'z'},
    {16, 21, 38, 43, 'x', 'X', 'X', 'x'},
    {21, 26, 38, 43, 'c', 'C', 'C', 'c'},
    {26, 31, 38, 43, 'v', 'V', 'V', 'v'},
    {31, 36, 38, 43, 'b', 'B', 'B', 'b'},
    {36, 41, 38, 43, 'n', 'N', 'N', 'n'},
    {41, 46, 38, 43, 'm', 'M', 'M', 'm'},
    {46, 51, 38, 43, ',', ',', '<', '<'},
    {51, 56, 38, 43, '.', '.', '>', '>'},
    {56, 61, 38, 43, '/', '/', '?', '?'},
    {61, 78, 38, 43, ' ', ' ', ' ', ' '},
};

void print_keyboard() {
    printf("\033[%d;%dH", 24, 1);
    printf("   ---  ---  ---  ---  ---  ---  ---  ---  ---  ---  ---  ---  ---  --------- \n");
    printf("  | ~ || ! || @ || # || $ || %% || ^ || & || * || ( || ) || _ || + ||B        |\n");
    printf("  |   ||   ||   ||   ||   ||   ||   ||   ||   ||   ||   ||   ||   ||BACKSPACE|\n");
    printf("  | ` || 1 || 2 || 3 || 4 || 5 || 6 || 7 || 8 || 9 || 0 || - || = ||         |\n");
    printf("   ---  ---  ---  ---  ---  ---  ---  ---  ---  ---  ---  ---  ---  --------- \n");
    printf("   -----  ---  ---  ---  ---  ---  ---  ---  ---  ---  ---  ---  ---  ------- \n");
    printf("  |     ||   ||   ||   ||   ||   ||   ||   ||   ||   ||   || { || } ||   |   |\n");
    printf("  | SPC || Q || W || E || R || T || Y || U || I || O || P ||   ||   ||       |\n");
    printf("  |     ||   ||   ||   ||   ||   ||   ||   ||   ||   ||   || [ || ] ||   \\   |\n");
    printf("   -----  ---  ---  ---  ---  ---  ---  ---  ---  ---  ---  ---  ---  ------- \n");
    printf("   ------  ---  ---  ---  ---  ---  ---  ---  ---  ---  ---  ---  ----------- \n");
    printf("  |X     ||   ||   ||   ||   ||   ||   ||   ||   ||   || : || \" ||A          |\n");
    printf("  | CAPS || A || S || D || F || G || H || J || K || L ||   ||   ||   ENTER   |\n");
    printf("  |      ||   ||   ||   ||   ||   ||   ||   ||   ||   || ; || ' ||           |\n");
    printf("   ------  ---  ---  ---  ---  ---  ---  ---  ---  ---  ---  ---  ----------- \n");
    printf("   -------  ---  ---  ---  ---  ---  ---  ---  ---  ---  ---  --------------- \n");
    printf("  |Y      ||   ||   ||   ||   ||   ||   ||   || < || > || ? ||               |\n");
    printf("  | SHIFT || Z || X || C || V || B || N || M ||   ||   ||   ||     SPACE     |\n");
    printf("  |       ||   ||   ||   ||   ||   ||   ||   || , || . || / ||               |\n");
    printf("   -------  ---  ---  ---  ---  ---  ---  ---  ---  ---  ---  --------------- \n");
}

void print_keyboard_shift() {
    printf("\033[47m\033[30m");
    printf("\033[%d;%dH", 40, 4);
    printf("Y      ");
    printf("\033[%d;%dH", 41, 4);
    printf(" SHIFT ");
    printf("\033[%d;%dH", 42, 4);
    printf("       ");
    printf("\033[0m");
}

void print_keyboard_caps() {
    printf("\033[47m\033[30m");
    printf("\033[%d;%dH", 35, 4);
    printf("X     ");
    printf("\033[%d;%dH", 36, 4);
    printf(" CAPS ");
    printf("\033[%d;%dH", 37, 4);
    printf("      ");
    printf("\033[0m");
}

bool valid_position() {
    if (cur_row < int(list.size())) {
        if (cur_col < int(list.at(cur_row).size())) {
            return true;
        }
    }
    return false;
}

void refresh_editor() {
    printf("\033[2J");
    for (int i = cur_row - cur_row%20; i < cur_row - cur_row%20 + 20; i++) {
        if (i < int(list.size())) {
            printf("%s\n", list.at(i).c_str());
        }
    }
    printf("\033[%d;%dH", cur_row%20+1, cur_col+1);
    printf("\033[47m\033[30m");
    printf("%c\033[1D", ' ');
    printf("\033[0m");
    if (valid_position()) {
        char c = list.at(cur_row).at(cur_col);
        printf("\033[%d;%dH", cur_row%20+1, cur_col+1);
        printf("\033[47m\033[30m");
        printf("%c\033[1D", c);
        printf("\033[0m");
    }

    printf("\033[%d;%dH", 22, 1);
    printf("  (+)save (-)exit (ZL)last page (ZR)next page");
    if (valid_position()) {
        printf("                     R: %d, C: %d", cur_row+1, cur_col+1);
    }

    print_keyboard();
    if (keyboard_shift) {
        print_keyboard_shift();
    }
    if (keyboard_caps) {
        print_keyboard_caps();
    }
}

void backspace() {
    if (cur_row < int(list.size())) {
        if (cur_col == 0) {
            if (cur_row > 0) {
                cur_row--;
                cur_col = list.at(cur_row).size();
                list.at(cur_row).append(list.at(cur_row+1));
                list.erase(list.begin() + cur_row + 1);
            }
        } else {
            if (cur_col <= int(list.at(cur_row).size())) {
                list.at(cur_row).erase(cur_col-1, 1);
                cur_col--;
            }
        }
    }
}

void enter() {
    if (cur_row < int(list.size()) && cur_col <= int(list.at(cur_row).size())) {
        if (cur_col == int(list.at(cur_row).size())) {
            list.insert(list.begin() + cur_row + 1, std::string(""));
            cur_row++;
            cur_col = 0;
        } else {
            std::string l = list.at(cur_row).substr(cur_col);
            list.insert(list.begin() + cur_row + 1, l);
            list.at(cur_row) = list.at(cur_row).substr(0, cur_col);
            cur_row++;
            cur_col = 0;
        }
    }
}

void edit_file(const char* filename) {
    FILE * fp = NULL;
    fp = fopen(filename, "rb");
    if(!fp){
        printf("fopen failed\n");
        return;
    }
    list.clear();
    std::string tmpstr;
    while(1){
        char c = '\0';
        int i = fread(&c, 1, 1, fp);
        if (i != 1) {
            if (tmpstr != "") {
                list.push_back(tmpstr);
            }
            break;
        }
        if (c == '\n') {
            list.push_back(tmpstr);
            tmpstr = "";
        } else if (c != '\r') {
            tmpstr.push_back(c);
        }
    }
    fclose(fp);

    refresh_editor();

    u32 prev_touchcount=0;

    while(appletMainLoop()) {
        hidScanInput();
        u64 kDown = hidKeysDown(CONTROLLER_P1_AUTO);
 
        if (kDown & KEY_MINUS) {
            return;
        }

        if (kDown & KEY_PLUS) {
            fp = fopen(filename, "wb");
            if(!fp) {
                printf("fopen failed\n");
                return;
            }
            for (int i = 0; i < int(list.size()); i++) {
                fwrite(list.at(i).c_str(), 1, list.at(i).size(), fp);
                char c = '\n';
                fwrite(&c, 1, 1, fp);
            }
            fclose(fp);
            return;
        }

        if (kDown & KEY_UP) {
            if (cur_row > 0) cur_row--;
            refresh_editor();
        }
        if (kDown & KEY_LEFT) {
            if (cur_col > 0) cur_col--;
            refresh_editor();
        }
        if (kDown & KEY_RIGHT) {
            cur_col++;
            refresh_editor();
        }
        if (kDown & KEY_DOWN) {
            cur_row++;
            refresh_editor();
        }
        if (kDown & KEY_ZL) {
            if (cur_row >= 20) cur_row -= 20;
            refresh_editor();
        }
        if (kDown & KEY_ZR) {
            cur_row += 20;
            refresh_editor();
        }
        if (kDown & KEY_A) {
            enter();
            refresh_editor();
        }
        if (kDown & KEY_B) {
            backspace();
            refresh_editor();
        }

        if (kDown & KEY_X) {
            keyboard_caps = !keyboard_caps;
            refresh_editor();
        }
        if (kDown & KEY_Y) {
            keyboard_shift = !keyboard_shift;
            refresh_editor();
        }

        if (kDown & KEY_R) {
            if (valid_position()) {
                char c = list.at(cur_row).at(cur_col);
                if (c >= 'a' && c <= 'z') c = 'A' - 'a' + c;
                if (c >= '0' && c <= '8') c++;
                else if (c == '9') c = 'A';
                else if (c >= 'A' && c <= 'E') c++;
                else if (c == 'F') c = '0';
                list.at(cur_row).at(cur_col) = c;
            }
            refresh_editor();
        }

        if (kDown & KEY_L) {
            if (valid_position()) {
                char c = list.at(cur_row).at(cur_col);
                if (c >= 'a' && c <= 'z') c = 'A' - 'a' + c;
                if (c >= '1' && c <= '9') c--;
                else if (c == '0') c = 'F';
                else if (c >= 'B' && c <= 'F') c--;
                else if (c == 'A') c = '9';
                list.at(cur_row).at(cur_col) = c;
            }
            refresh_editor();
        }

        //touch handler
        touchPosition touch;
        u32 touch_count = hidTouchCount();
        if (touch_count != prev_touchcount) {
            prev_touchcount = touch_count;
            if (touch_count > 0) {
                hidTouchRead(&touch, touch_count-1);
                // set position
                if (touch.py/16.0 < 20) {
                    cur_row = cur_row - cur_row%20 + int(touch.py/16.0);
                    cur_col = int(touch.px/16.0);
                }
                // normal key
                for (int i = 0; i < 48; i++) {
                    if (touch.px/16.0 > keyboard_table[i][0] &&
                    touch.px/16.0 < keyboard_table[i][1] &&
                    touch.py/16.0 > keyboard_table[i][2] &&
                    touch.py/16.0 < keyboard_table[i][3]) {
                        char c[2] = {0, 0};
                        if (!keyboard_shift && !keyboard_caps) c[0] = keyboard_table[i][4];
                        if (!keyboard_shift && keyboard_caps) c[0] = keyboard_table[i][5];
                        if (keyboard_shift && !keyboard_caps) c[0] = keyboard_table[i][6];
                        if (keyboard_shift && keyboard_caps) c[0] = keyboard_table[i][7];
                        if (cur_row < int(list.size())) {
                            if (cur_col <= int(list.at(cur_row).size())) {
                                list.at(cur_row).insert(cur_col, c);
                                cur_col++;
                            }
                        }
                        keyboard_shift = false;
                    }
                }
                // backspace
                if (touch.px/16.0 > 67 &&
                    touch.px/16.0 < 78 &&
                    touch.py/16.0 > 23 &&
                    touch.py/16.0 < 28) {
                    backspace();
                }
                // enter
                if (touch.px/16.0 > 65 &&
                    touch.px/16.0 < 78 &&
                    touch.py/16.0 > 33 &&
                    touch.py/16.0 < 38) {
                    enter();
                }
                // caps
                if (touch.px/16.0 > 2 &&
                    touch.px/16.0 < 10 &&
                    touch.py/16.0 > 33 &&
                    touch.py/16.0 < 38) {
                    keyboard_caps = !keyboard_caps;
                }
                // shift
                if (touch.px/16.0 > 2 &&
                    touch.px/16.0 < 11 &&
                    touch.py/16.0 > 38 &&
                    touch.py/16.0 < 43) {
                    keyboard_shift = !keyboard_shift;
                }
                refresh_editor();
            }
        }

        consoleUpdate(NULL);
    }
}