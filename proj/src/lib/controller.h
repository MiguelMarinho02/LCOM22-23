#include <lcom/lcf.h>
#include <stdint.h>
#include <stdio.h>
#include "draw_elements.h"

#define RIGHT_LIMIT (int) getxRes() - 132
#define LEFT_LIMIT 100
#define NUM_ALIENS 50

enum dir {RIGHT,LEFT};

int movePlayer(int offset, enum dir); //false for left, true for right
int moveAlien(Sprite *alien, bool direction, bool ver); //false for left, true for right || true for vertical
int moveAliens();
int moveCursor(int16_t x, int16_t y);
int checkColisionWithEnemyShip();
int enemyBlastsControl();
int checkColisionWithPlayerShip();
void checkColisionWithEnemyBullet();
bool allEnemiesDead();
