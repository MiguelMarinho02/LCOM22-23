#include "sprite.h"

Sprite player, enemyBlasts, playerBlasts, font;
Sprite aliens[50];
Sprite bullet;
Sprite cursor;

bool aliens_active[50];

/**
 * @brief Get the Player Sprite object
 * 
 * @return the player Sprite  
 */
Sprite * getPlayer(){
    return &player;
}

/**
 * @brief Creates Sprites of game's elements
 * 
 */
int (createSprites)(){
    //init player
    player.map = vg_xpm_map_to_image(ship32_xpm);
    player.y = 750;

    //init cursor
    cursor.map = vg_xpm_map_to_image(crossair22_xpm);
    cursor.x = getxRes() / 2 - cursor.map.width / 2;
    cursor.y = 300;

    //init font
    font.map = vg_xpm_map_to_image(text_font_big_xpm);
    
    //init aliens
    
    int x = getxRes() / 2 - (vg_xpm_map_to_image(enemy32_1a_xpm).width + 50) * 3, y = 50;
    for(int i = 0; i < 50; i++){
        Sprite alien;
        alien.map = vg_xpm_map_to_image(enemy32_1a_xpm);
        aliens_active[i] = true;
        //alien.map = vg_xpm_map_to_image(alien_xpm);
        if(i % 10 == 0){ //when it reaches 10 in a row, we advance
            y += 50;
        }
        if(i == 4){
            player.x = x + i*50;
        }
        if(i >= 10){
            alien.x = aliens[i-10].x;
            alien.y = y;
            aliens[i] = alien;
        }
        else{
            alien.x = x + i*50;
            alien.y = y;
            aliens[i] = alien;
        }
    }
    return 0;
}

/**
 * @brief Get the Alien Sprite object
 * 
 * @param spriteNum 
 * @return respective Sprite 
 */

Sprite getAlienSprite(int spriteNum){
    Sprite spr;
    if (spriteNum == 1) spr.map = vg_xpm_map_to_image(enemy32_1a_xpm);
    else if (spriteNum == 2) spr.map = vg_xpm_map_to_image(enemy32_1b_xpm);
    else if (spriteNum == 3) spr.map = vg_xpm_map_to_image(enemy32_1c_xpm);
    else if (spriteNum == 4) spr.map = vg_xpm_map_to_image(enemy32_1d_xpm);
    return spr;
}
