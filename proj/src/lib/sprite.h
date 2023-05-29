#include <lcom/lcf.h>
#include "gpu.h"
#include "../imgs/test.xpm"
#include "../imgs/ship32.xpm"
#include "../imgs/enemy32_1a.xpm"
#include "../imgs/enemy32_1b.xpm"
#include "../imgs/enemy32_1c.xpm"
#include "../imgs/enemy32_1d.xpm"
#include "../imgs/bullet_blue10.xpm"
#include "../imgs/bullet_red10.xpm"
#include "../imgs/crossair22.xpm"
#include "../imgs/text_font.xpm"
#include "../imgs/text_font_big.xpm"

#include <stdint.h>
#include <stdio.h>

typedef struct {
int x, y; // current position
int width, height; // dimensions
int old_x_offset,old_y_offset;
xpm_image_t map; // the pixmap
} Sprite;

struct Bullet{
    int x_dest;
    int y_dest;
    int move_x;
    int move_y;
    int error1;
    bool goRight;
    bool isActive;
    Sprite spr;
};

int createSprites();
Sprite * getPlayer();
Sprite getAlienSprite(int spriteNum);
