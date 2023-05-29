// IMPORTANT: you must include the following line in all your C files

#include <lcom/lcf.h>

#include <lcom/proj.h>
#include <stdint.h>
#include <time.h>
#include "lib/game_loop.h"

int main(int argc, char* argv[]) {
    lcf_set_language("EN-US");

    if (lcf_start(argc, argv)){
        return 1;
    }

    lcf_cleanup();

    return 0;
}

int(proj_main_loop)(int argc, char* argv[]) {
    uint8_t timer_bit_no = 0, keyboard_bit_no = 1, mouse_bit_no = 2;
    fecthCurrentDate(); //this does basically nothing, is just to unfuck minix's rtc
    start_game(timer_bit_no,keyboard_bit_no,mouse_bit_no);

    return 0;
}
