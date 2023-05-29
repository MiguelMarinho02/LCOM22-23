#include "controller.h"
#include <time.h>

bool direction = true;
bool vertical = false;

/**
 * @brief Moves player according to the offset and direction given, left or right
 * 
 * @param offset 
 * @param d direction: LEFT or RIGHT
 * @return int 0
 */
//! \callergraph
//! \callgraph
int movePlayer(int offset, enum dir d){
    if(d == RIGHT){
        if(getPlayer()->x + offset >= RIGHT_LIMIT){
            return 0;
        }
        getPlayer()->x += offset; 
    }
    else if(d == LEFT){
        if(getPlayer()->x - offset < LEFT_LIMIT){
            return 0;
        }
        getPlayer()->x -= offset; 
    }
    return 0;
}

/**
 * @brief Updates aliens positions according to directions given, left or right or also vertically
 * 
 * @param alien aliens sprite
 * @param direction false for left, true for right
 * @param ver if true, aliens will move vertically
 * @return int 0
 */
int moveAlien(Sprite *alien, bool dir, bool ver){

    if(ver){
        alien->y += 50;
        return 0;
    }

    if(dir){
        alien->x += 32;
    }
    else{
        alien->x -= 32;
    }
    
    return 0;
}

/**
 * @brief Based on extreme ends (left and right) aliens  position, the direction in which they will be moving next is updated 
 * and all alien's positions are updated by calling moveAlien(alient,direction,vert) for each of them
 * 
 * @return int 0
 */
int moveAliens(){
    extern Sprite aliens[50];

    if(aliens[9].x + 50 >= RIGHT_LIMIT || aliens[0].x - 50 <= LEFT_LIMIT){
        vertical = !vertical;
        if(vertical == false){direction = !direction;}
    }

    for(int i = 0; i < NUM_ALIENS; i++){
        moveAlien(&aliens[i],direction,vertical);
    }

    return 0;
}

/**
 * @brief Moves cursor to the updated position
 * 
 * @param x position
 * @param y position
 * @return int 0
 */
int moveCursor(int16_t x, int16_t y){
    extern Sprite cursor;

    if(cursor.x + x < 0 || cursor.x + x >= getxRes()){
        return 0;
    }

    if(cursor.y - y < 0 || cursor.y - y >= getyRes()){
        return 0;
    }

    cursor.old_x_offset += x;
    cursor.old_y_offset += y;

    cursor.x += x;
    cursor.y -= y;

    return 0;
}

/**
 * @brief Generates a random number between 0 and 49 and checks if the alien in that position is active, if so, it creates a bullet coming from it
 * 
 * @return int 
 */
//! \callergraph
//! \callgraph
int enemyBlastsControl(){
    extern Sprite aliens[50];
    extern bool aliens_active[50];
    //Sprite *player = getPlayer();

    srand(time(NULL));
    int randomNumber = (rand() % 5) + 1;
    int it = randomNumber * 10 - 10;
    int old = 51;

    for(; it < 50; it++){
        if(aliens_active[it]){
            if(aliens[it].x +2 <= getPlayer()->x + 30 && aliens[it].x + 30 >= getPlayer()->x + 2){
                createBulletEnemy(aliens[it].x,aliens[it].y);
                old = it;
                break;
            }
        }
    }

    do{   //this way we won't have a ship dealing dual damage
        randomNumber = (rand() % 49) + 1;
    }while(randomNumber == old || !aliens_active[randomNumber]);
    
    createBulletEnemy(aliens[randomNumber].x,aliens[randomNumber].y);

    return 0;
}

/**
 * @brief Checks if a bullet has colided with an alien, returns the score according to that
 * 
 * @return int score
 */
int checkColisionWithEnemyShip(){
    extern struct Bullet bullets[MAX_BULLETS];
    extern int numBullets;
    extern Sprite aliens[50];
    extern bool aliens_active[50];
    int score = 0;

    for(int i = 0; i < numBullets; i++){
        if(bullets[i].isActive){
            for(int j = 0; j < 50; j++){
                if(aliens_active[j]){
                    if((bullets[i].spr.x + 10 >= aliens[j].x && bullets[i].spr.x <= aliens[j].x + 32 - 3) && (bullets[i].spr.y + 10 >= aliens[j].y && bullets[i].spr.y <= aliens[j].y + 32 - 6)){
                        bullets[i].isActive = false;
                        aliens_active[j] = false;
                        score++;
                    } 
                }
                
            }
        }
    }
    return score;
}

/**
 * @brief Checks if a bullet has colided with the player's ship, returns the number of hp deducted
 * 
 * @return int hpDeducted
 */
int checkColisionWithPlayerShip(){
    int hpDeducted = 0;
    extern struct Bullet enemyBullets[2*MAX_BULLETS];
    Sprite *player = getPlayer();
    extern int numEnemyBullets;

    for(int i = 0; i < numEnemyBullets; i++){
        if(enemyBullets[i].isActive){
            if(enemyBullets[i].spr.x + 10 >= player->x + 3 && enemyBullets[i].spr.x <= player->x + 30){ //check if is in x axis
               if(enemyBullets[i].spr.y + 10 >= player->y && enemyBullets[i].spr.y <= player->y + 30){ //check if is in y axis
                  hpDeducted++;
                  enemyBullets[i].isActive = false;
               }
            }
        }
    }

    return hpDeducted;
}

/**
 * @brief checks if a bullet has colided with an alien's bullet
 * 
 */
void checkColisionWithEnemyBullet(){
    extern struct Bullet enemyBullets[2*MAX_BULLETS];
    extern int numEnemyBullets;
    extern struct Bullet bullets[MAX_BULLETS];
    extern int numBullets;

    for(int i = 0; i < numEnemyBullets; i++){
        if(enemyBullets[i].isActive){
            for(int j = 0; j < numBullets; j++){
                if(bullets[j].isActive){
                    struct Bullet b1 = enemyBullets[i];
                    struct Bullet b2 = bullets[j];

                    if(b1.spr.x + 10 >= b2.spr.x && b1.spr.x <= b2.spr.x + 10){
                        if(b1.spr.y + 10 >= b2.spr.y && b1.spr.y <= b2.spr.y + 10){
                            enemyBullets[i].isActive = false;
                            bullets[j].isActive = false;
                        }
                    }

                }
            }
        }
    }
}

/**
 * @brief If all enemies are dead, returns true, else returns false
 * 
 * @return true 
 * @return false 
 */
bool allEnemiesDead(){
    extern bool aliens_active[50];
    for(int i = 0; i < 50; i++){
        if(aliens_active[i]){
            return false;
        }
    }
    return true;
}
