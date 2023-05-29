#include <lcom/lcf.h>
#include "gpu.h"
#include "keyboard.h"
#include "mouse.h"
#include "timer.h"
#include "controller.h"

enum gameState {MENU,GAME,LEADERBOARD};

int start_game(uint8_t timer_bit_no, uint8_t keyboard_bit_no, uint8_t mouse_bit_no);
