#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <switch.h>

#include "util.h"
#include "touch.h"
#include "menu.h"
#include "unzip.h"
#include "download.h"
#include "reboot_payload.h"

int appInit()
{
    plInitialize();
    socketInitializeDefault();
    setsysInitialize();
    splInitialize();
    romfsInit();
    sdlInit();
    romfsExit();
    return 0;
}

void appExit()
{
    sdlExit();
    socketExit();
    plExit();
    splExit();
    setsysExit();
}

int main(int argc, char **argv)
{
    // init stuff.
    appInit();
    mkdir(APP_PATH, 0777);
    chdir(ROOT);

    // write sys / ams version to char*.
    writeSysVersion();
    writeAmsVersion();
    refreshScreen(/*loaded=*/0);
    updateRenderer();
    writeLatestAtmosphereVersion();

    // set the cursor position to 0.
    short cursor = 0;

    // touch variables.
    int touch_lock = OFF;
    u32 tch = 0;
    touchPosition touch;

    // muh loooooop
    while(appletMainLoop())
    {
        // scan for button / touch input each frame.
        hidScanInput();
        u64 kDown = hidKeysDown(CONTROLLER_P1_AUTO);
        hidTouchRead(&touch, tch);
        u32 touch_count = hidTouchCount();

        // main menu display
        printOptionList(cursor);

        // move cursor down...
        if (kDown & KEY_DOWN)
        {
            if (cursor == CURSOR_LIST_MAX) cursor = 0;
            else cursor++;
        }

        // move cursor up...
        if (kDown & KEY_UP)
        {
            if (cursor == 0) cursor = CURSOR_LIST_MAX;
            else cursor--;
        }

        // select option
        if (kDown & KEY_A || (touch_lock == OFF && touch.px > 530 && touch.px < 1200 && touch.py > FIRST_LINE - HIGHLIGHT_BOX_MIN && touch.py < (NEWLINE * CURSOR_LIST_MAX) + FIRST_LINE + HIGHLIGHT_BOX_MAX))
        {
            // check if the user used touch to enter this option.
            if (touch_lock == OFF && touch_count > 0)
                cursor = touch_cursor(touch.px, touch.py);

            switch (cursor)
            {
            case UP_AMS:
                if (yesNoBox(cursor, 390, 250, "Update BPack?") == YES)
                    update_ams_hekate(AMS_URL, AMS_OUTPUT, cursor);
                break;

            case REBOOT_PAYLOAD:
                if (yesNoBox(cursor, 390, 250, "Reboot to Payload?") == YES)
                    reboot_payload("/atmosphere/reboot_payload.bin");
                break;
            }
        }

        // exit...
        if (kDown & KEY_PLUS || (touch.px > 1145 && touch.px < SCREEN_W && touch.py > 675 && touch.py < SCREEN_H))
            break;

        // lock touch if the user has already touched the screen (touch tap).
        if (touch_count > 0) touch_lock = ON;
        else touch_lock = OFF;

        // display render buffer
        updateRenderer();
    }

    // cleanup then exit
    appExit();
    return 0;
}
