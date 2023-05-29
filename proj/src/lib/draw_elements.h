#include <lcom/lcf.h>
#include "gpu.h"
#include "sprite.h"
#include "rtc.h"

#include <stdint.h>
#define MAX_BULLETS 200

int draw_player();
int clear_player();
int draw_aliens(int spriteNum);
int clear_aliens();
//int draw_bullet();
int draw_elements(int spriteNum, int score, int timer, int hp);
int clear_elements();
int draw_menu();
int clear_menu();
int draw_letter(char letter, int x, int y);
int draw_string(char *string, int x, int y);
int draw_arena(int score, int timer, int hp);
void resetBullets();

void updateBullets();
void createBullet(int x_dest, int y_dest);
int displayLeaderboard();
void updateLeaderboard(int newScore);
void cleanLeaderboard();
int clear_leaderboard();

typedef struct {
    int score;
    struct Date date;
}Player;

void createBulletEnemy(int x, int y);
