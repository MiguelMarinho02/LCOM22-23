#include "draw_elements.h"
#include <string.h>

struct Bullet bullets[MAX_BULLETS];
int numBullets = 0;

struct Bullet enemyBullets[2*MAX_BULLETS];
int numEnemyBullets = 0;

/**
 * @brief Draws player in current position
 * 
 * @return 0 
 */
int draw_player(){
    Sprite player = *getPlayer();
    vg_xpm(player.map,player.x,player.y);
    return 0;
}

/**
 * @brief Clears player sprite in current position
 * 
 * @return int 
 */
int clear_player(){
    Sprite player = *getPlayer();
    vg_clear(player.map,player.x,player.y);
    return 0;
}

/**
 * @brief Displays enemy of the correct sprite num
 * 
 * @param spriteNum 
 * @return int 
 */
int draw_aliens(int spriteNum){
    extern Sprite aliens[50];
    extern bool aliens_active[50];
    Sprite spr = getAlienSprite(spriteNum);

    for(int i = 0;i<50;i++){
        if(aliens_active[i]){
            vg_xpm(spr.map,aliens[i].x,aliens[i].y);
        }
    }
    return 0;
}

/**
 * @brief Enemy clear function
 * 
 * @return int 0
 */
int clear_aliens(){
    extern Sprite aliens[50];
    for(int i = 0;i<50;i++){
        vg_clear(aliens[i].map,aliens[i].x,aliens[i].y);
    }
    return 0;
}

/**
 * @brief Displays cursor in its current position 
 * 
 * @return int 
 */
int draw_cursor(){
    extern Sprite cursor;
    vg_xpm(cursor.map,cursor.x,cursor.y);
    return 0;
}

/**
 * @brief Clears cursor and resets old offset
 * 
 * @return int 
 */
int clear_cursor(){
    extern Sprite cursor;
    vg_clear(cursor.map,cursor.x - cursor.old_x_offset ,cursor.y + cursor.old_y_offset);
    cursor.old_x_offset = 0;
    cursor.old_y_offset = 0;
    return 0;
}

/**
 * @brief 
 * 
 * @param score 
 * @param timer 
 * @param hp 
 * @return int 
 */
int draw_arena(int score, int timer, int hp){
    for(int j = 0; j<5;j++){
        vg_draw_vline(100+j,0,getyRes(),0xFFFFFF);
        vg_draw_vline(getxRes() - 100 - j,0,getyRes(),0xFFFFFF);
    }

    char str_score[2];
    sprintf(str_score, "%d", score);
    draw_string("score ",2,2);
    draw_string(str_score,6*32,0); //lenght of score plus a space times the size of a letter

    char str_timer[2];
    sprintf(str_timer, "%d", timer);
    draw_string("time" , getxRes() - 230,2);
    draw_string(str_timer, getxRes() - 70,2);

    char str_hp[1];
    sprintf(str_hp, "%d", hp);
    draw_string("hp " , 2,40);
    draw_string(str_hp, 2*32 + 16,40);
    return 0;
}

/**
 * @brief updates the bullet movement
 * 
 */
void updateBullets() {
    //This part updates the player bullets
    for (int i = 0; i < numBullets; i++) {
        if (bullets[i].spr.y < 1 || bullets[i].spr.x < 100 || bullets[i].spr.x > 1040) {
            bullets[i].isActive = false;
            vg_clear(bullets[i].spr.map,bullets[i].spr.x,bullets[i].spr.y);
        }
        if (bullets[i].isActive) {
            int error2 = bullets[i].error1 * 2;

            if (error2 > - bullets[i].move_y) {
                bullets[i].error1 -= bullets[i].move_y;
                if (bullets[i].goRight) {
                    bullets[i].spr.x += 3;
                } else{
                    bullets[i].spr.x -= 3;
                }
            }
            if (error2 < bullets[i].move_x) {
                bullets[i].error1 += bullets[i].move_x;
                bullets[i].spr.y -= 3;
            }
            vg_xpm(bullets[i].spr.map,bullets[i].spr.x,bullets[i].spr.y);            
        }
    }
    //This part updates the enemy bullets
    for (int i = 0; i < numEnemyBullets; i++) {
        if (enemyBullets[i].spr.y >= getyRes() - 10 && enemyBullets[i].isActive) {
            enemyBullets[i].isActive = false;
            vg_clear(enemyBullets[i].spr.map,enemyBullets[i].spr.x,enemyBullets[i].spr.y);
        }

        if (enemyBullets[i].isActive) {
            enemyBullets[i].spr.y += enemyBullets[i].move_y;  // Bullet always go down
            vg_xpm(enemyBullets[i].spr.map,enemyBullets[i].spr.x,enemyBullets[i].spr.y);
        }
    }
}

/**
 * @brief Clears every singe bullet in its own position
 * 
 */
void clear_bullets() {
    for (int i = 0; i < numBullets; i++) {
        vg_clear(bullets[i].spr.map,bullets[i].spr.x,bullets[i].spr.y);
    }

    for (int i = 0; i < numEnemyBullets; i++) {
        vg_clear(enemyBullets[i].spr.map,enemyBullets[i].spr.x,enemyBullets[i].spr.y);
    }
}

/**
 * @brief Draws letter from the font's xpm
 * 
 * @param letter 
 * @param x 
 * @param y 
 * @return int 
 */
int draw_letter(char letter, int x, int y){
    extern Sprite font;
    uint8_t bpp = (font.map.size / (font.map.height * font.map.width));
    uint8_t *pnt_to_char = NULL;

    if(letter >= '0' && letter <= '9'){
        pnt_to_char = font.map.bytes + (513*bpp);
        uint8_t num = letter - '0';
        
        pnt_to_char += ((32) * (num)) * bpp; 
    }else{
        pnt_to_char = font.map.bytes + (1057*bpp);
        uint8_t num_of_letters = letter - 'a';
        if(num_of_letters <= 6){
            pnt_to_char += (32*num_of_letters) * bpp;
        }
        else{
            pnt_to_char = font.map.bytes + font.map.width * 32 * bpp;
            pnt_to_char += (32*(num_of_letters-7)) * bpp;
        }
    }

    vg_draw_char(font.map,x,y,pnt_to_char);
    return 0;
}

/**
 * @brief Draws a string of letters calling draw_letter()
 * 
 * @param string 
 * @param x 
 * @param y 
 * @return int 
 */
int draw_string(char *string, int x, int y){
    for(unsigned int i = 0; i < strlen(string); i++){
        if(string[i] != ' ' && string[i] != '\0'){
            draw_letter(string[i],x + i*33,y);
        }
    }

    return 0;
}

/**
 * @brief Draws all the elements of the game by calling the respective functions
 * 
 * @param spriteNum 
 * @param score 
 * @param timer 
 * @param hp 
 * @return int 
 */
//! \callergraph
//! \callgraph
int draw_elements(int spriteNum, int score, int timer, int hp){
    draw_arena(score, timer, hp);
    draw_player();
    draw_aliens(spriteNum);
    draw_cursor();
    return 0;
}

/**
 * @brief Clears all elements on the screen
 * 
 * @return int 
 */
int clear_elements(){
    clear_player();
    clear_aliens();
    clear_cursor();
    clear_bullets();
    vg_clear_area(getxRes() - 70, 2,70,40);
    return 0;
}

/**
 * @brief Draws the menu of the game
 * 
 * @return int 
 */
int draw_menu(){
    draw_string("start",(getxRes() / 2) - (5 * 34),350);
    draw_string("leaderboard",(getxRes() / 2) - (5 * 34),400);
    draw_string("quit",(getxRes() / 2) - (5 * 34),450);
    draw_cursor();
    
    return 0;
}

/**
 * @brief Clears the menu of the game
 * 
 * @return int 
 */
int clear_menu(){
    clear_cursor();
    vg_clear_area((getxRes() / 2) - (5 * 34),350,370,200);
    return 0;
}

/**
 * @brief Clear the leaderboard off screen
 * 
 * @return int 
 */
int clear_leaderboard(){
    clear_cursor();
    vg_clear_area(0, 0, getxRes(), getyRes());
    return 0;
}

/**
 * @brief Creates a new Bullet object
 * 
 * @param x_dest 
 * @param y_dest 
 */
void createBullet(int x_dest, int y_dest) {
    if (numBullets < MAX_BULLETS) {
        Sprite player = *getPlayer();
        struct Bullet b;
        b.spr.map = vg_xpm_map_to_image(bullet_blue10_xpm);
        b.spr.x = player.x+11;  // Initial position x
        b.spr.y = player.y-10;  // Initial position y
        b.isActive = true;
        b.x_dest = x_dest;
        b.y_dest = y_dest;
        if (b.spr.x > b.x_dest) {
            b.goRight = false;
        } else {
            b.goRight = true;
        }
        b.move_x = abs(b.spr.x - b.x_dest);
        b.move_y = abs(b.spr.y - b.y_dest);
        b.error1 = b.move_x - b.move_y;
        
        numBullets++;

        vg_xpm(b.spr.map,b.spr.x,b.spr.y);
        bullets[numBullets] = b;
        numBullets++;
    }
}

/**
 * @brief Create a Bullet Enemy object
 * 
 * @param x x position of enemy ship
 * @param y y position of enemy ship
 */
void createBulletEnemy(int x, int y){ //receives the position of the enemy ship
    if(numEnemyBullets < 2*MAX_BULLETS){
        struct Bullet b;
        b.spr.map = vg_xpm_map_to_image(bullet_red10_xpm);
        b.spr.x = x+16;  // Initial position x
        b.spr.y = y+30;  // Initial position y
        b.isActive = true;

        b.move_x = 0; //only moves vertically

        b.move_y = 10;

        enemyBullets[numEnemyBullets] = b;
        numEnemyBullets++;

        vg_xpm(b.spr.map,b.spr.x,b.spr.y);
    }
}

/**
 * @brief Reset the bullet's state to inactive
 * 
 */
void resetBullets(){
    numBullets = 0;
    numEnemyBullets = 0;
    for(int i = 0; i < MAX_BULLETS; i++){
        bullets[i].isActive = false;
    }

    for(int i = 0; i < 2*MAX_BULLETS; i++){
        enemyBullets[i].isActive = false;
    }
}
/*
void displayLeaderboard() {
    printf("Leaderboard:\n");
    for (int i = 0; i < 10; i++) {
        printf("%d. %s - %d\n", i + 1, leaderboard[i].name, leaderboard[i].score);
    }
}
*/
/**
 * @brief Converts an integer to a string
*/
char* tostring(char str[], int num)
{
    int i, rem, len = 0, n;
 
    n = num;
    while (n != 0)
    {
        len++;
        n /= 10;
    }
    for (i = 0; i < len; i++)
    {
        rem = num % 10;
        num = num / 10;
        str[len - (i + 1)] = rem + '0';
    }
    str[len] = '\0';

    return str;
}

/**
 * @brief Updates the leaderboard with the new score
 * 
 * @param newScore 
 */
void updateLeaderboard(int newScore){
  Player leaderboard[10];

  FILE *file = fopen("leaderboard.txt", "r");
  if (file != NULL) {
    fread(leaderboard, sizeof(Player), 10, file);
    fclose(file);
  }


  int position = -1;

  for(int i = 0; i < 10; i++){
    if(newScore > leaderboard[i].score){
      position = i; 
      break;
    }
  }

  int sz = sizeof(leaderboard) / sizeof(leaderboard[0]);

  if(sz < 10){
    for(int i = sz; i < 10; i++){
        leaderboard[i].score = 0;
    }
  }

  struct Date date = fecthCurrentDate(); 

  if(position != -1){
    for(int i = 9; i > position; i--){
      leaderboard[i] = leaderboard[i-1];
    }

    leaderboard[position].score = newScore;
    leaderboard[position].date = date;
  }

  file = fopen("leaderboard.txt", "w");
  if(file != NULL){
    fwrite(leaderboard, sizeof(Player), 10, file);
    fclose(file);
  }


}

/**
 * @brief Displays the leaderboard
 * 
 * @return int 
 */
int displayLeaderboard(){
    Player leaderboard[10];
    draw_cursor();
    draw_string("leaderboard", (getxRes() / 2) - (5 * 34),2);
    draw_string("back", 2, 2);

    draw_string("position", (getxRes() / 2) - (5 * 94), 150);
    draw_string("score", (getxRes() / 2) - (5 * 24), 150);
    draw_string("date", (getxRes() / 2) + (5 * 24), 150);

    FILE *file = fopen("leaderboard.txt", "r");
    if (file != NULL) {
        fread(leaderboard, sizeof(Player), 10, file);
        fclose(file);
    }
    int ytemp = 250;
    for(int i = 0; i < 10; i++){
        char c_temp[6];
        char str[3];
        sprintf(c_temp, "%d- ", i+1);
        struct Date date = leaderboard[i].date;
        char year[3], month[3], day[3];
        sprintf(year, "%d", date.year);
        sprintf(month, "%d", date.month);
        sprintf(day, "%d", date.day);
        
        draw_string(c_temp,(getxRes() / 2) - (5 * 94),ytemp);
        draw_string(tostring(str, leaderboard[i].score),(getxRes() / 2) - (5 * 24),ytemp);

        //draw date
        draw_string(day, (getxRes() / 2) + (5 * 24), ytemp);
        draw_string(month, (getxRes() / 2) + (5 * 24) + 3*32, ytemp);
        draw_string(year, (getxRes() / 2) + (5 * 24) + 6*32, ytemp);

        ytemp = ytemp + 50;
    }

    return 0;
}

/**
 * @brief Clears the leaderboard
 * 
 */
void cleanLeaderboard(){
    Player leaderboard[10];

    FILE *file = fopen("leaderboard.txt", "r");
    if (file != NULL) {
        fread(leaderboard, sizeof(Player), 10, file);
        fclose(file);
    }

    struct Date date = {0,0,0};
    for(int i = 0; i < 10; i++){
        leaderboard[i].score = 0;
        leaderboard[i].date = date;
    }

    file = fopen("leaderboard.txt", "w");
    if(file != NULL){
        fwrite(leaderboard, sizeof(Player), 10, file);
        fclose(file);
    }

}
