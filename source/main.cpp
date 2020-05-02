#include "editor.h"
#include "browser.h"
#include <switch.h>

int main(int argc, char **argv)
{
    consoleInit(NULL);
    browser_file();
    consoleExit(NULL);
    return 0;
}
