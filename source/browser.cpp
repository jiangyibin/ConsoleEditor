#include "browser.h"
#include "editor.h"
#include <string.h>
#include <string>
#include <vector>
#include <stdio.h>
#include <dirent.h>
#include <switch.h>

static std::vector<std::string> dirs;
static std::vector<std::string> list;
static int cur_row = 0;

void refresh_browser() {
    printf("\033[2J");
    for (int i = cur_row - cur_row%40; i < cur_row - cur_row%40 + 40; i++) {
        if (i < int(list.size())) {
            printf("%s\n", list.at(i).c_str());
        }
    }
    if (cur_row >= 2 && cur_row < int(list.size())) {
        printf("\033[%d;%dH", cur_row%40+1, 1);
        printf("> ");
    }
    printf("\033[45;1H");
    printf("warning: console editor do not support tab and line longer than 105 characters");
}

void browser_file() {
    DIR* dir;
    struct dirent* ent;
    dirs.push_back("sdmc:/");

    while (1) {
        std::string cur_dir = dirs.at(0);
        for (int i = 1; i < int(dirs.size()); i++) {
            cur_dir = cur_dir + "/" + dirs.at(i);
        }
        dir = opendir(cur_dir.c_str());
        list.clear();
        list.push_back(cur_dir);
        list.push_back("");
        while ((ent = readdir(dir)))
        {
            if (ent->d_type == DT_DIR) {
                list.push_back("  d " + std::string(ent->d_name));
            } else {
                list.push_back("  - " + std::string(ent->d_name));
            }
        }
        closedir(dir);
        cur_row = 2;
        refresh_browser();

        while(appletMainLoop()) {
            hidScanInput();
            u64 kDown = hidKeysDown(CONTROLLER_P1_AUTO);

            if (kDown & KEY_UP) {
                cur_row--;
                if (cur_row < 2) cur_row = list.size() - 1;
                refresh_browser();
            }
            if (kDown & KEY_DOWN) {
                cur_row++;
                if (cur_row >= int(list.size())) cur_row = 2;
                refresh_browser();
            }
            if (kDown & KEY_A) {
                if (cur_row >= 2 && cur_row < int(list.size())) {
                    std::string select_item = list.at(cur_row);
                    if (select_item.at(2) == 'd') {
                        dirs.push_back(select_item.substr(4, -1));
                        break;
                    } else {
                        std::string cur_file = cur_dir + "/" + select_item.substr(4, -1);
                        edit_file(cur_file.c_str());
                        break;
                    }
                }
            }
            if (kDown & KEY_B) {
                if (int(dirs.size()) > 1) {
                    dirs.pop_back();
                    break;
                }
            }
            if (kDown & KEY_MINUS) {
                return;
            }

            consoleUpdate(NULL);
        }
    }
}