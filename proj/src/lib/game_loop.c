#include "game_loop.h"
#include <time.h>

#include <stdint.h>

//#define MODE 0x105
#define MODE 0x14C
#define W_KEY 0x91
#define A_KEY 0x9E
#define S_KEY 0x9F
#define D_KEY 0xA0
#define SPACE_KEY 0xB9
#define PIXELS_PER_MOVE 50

/**
 * @brief timer game handler 
 * 
 * @param counter 
 * @param spriteNum 
 * @param bullet 
 * @param score 
 * @param timer 
 * @param hp 
 * @return int 
 */
//! \callgraph
//! \callergraph
int timer_game_state(int *counter,int spriteNum,int *bullet, int *score, int *timer, int *hp){
  if(*counter == 60) {*bullet = 1;};
  if (*counter < 15) spriteNum = 1;
  else if ((*counter >= 15 && *counter < 30) || (*counter >= 75 && *counter < 90)) spriteNum = 2;
  else if ((*counter >= 30 && *counter < 45) || (*counter >= 60 && *counter < 75)) spriteNum = 3;
  else if (*counter >= 45 && *counter < 60) spriteNum = 4;
  else if (*counter >= 90){
    spriteNum = 1;
    *counter = 0;
    enemyBlastsControl();
  }

  timer_int_handler();

  if(getTimerCounter() == 60){ //actual timer to count seconds
    *timer = *timer - 1;
    setTimerCounter();
  }

  clear_elements();
  
  if(*counter == 60){
    moveAliens();
  }

  updateBullets();
  *score += checkColisionWithEnemyShip();
  *hp -= checkColisionWithPlayerShip();
  checkColisionWithEnemyBullet();
  int num = *score;
  int num2 = *timer;
  int num3 = *hp;
  draw_elements(spriteNum,num,num2,num3);
  flip_buffer();
  return 0;
}

/**
 * @brief Clears and redisplays the menu flipping the buffer
 * 
 * @return int 
 */
//! \callgraph
//! \callergraph
int timer_menu_state(){
  timer_int_handler();

  clear_menu();
  draw_menu();

  flip_buffer();
  return 0;
}

/**
 * @brief Clears menu and displays the leaderboard flipping the buffer
 * 
 * @return int 
 */
int timer_leaderboard_state(){
  timer_int_handler();
  clear_menu();
  displayLeaderboard();
  
  flip_buffer();
  return 0;
}

/**
 * @brief Mouse game position handler
 * 
 * @param x_dest 
 * @param y_dest 
 * @param bullet 
 * @return int 
 */
int mouse_game_state(int *x_dest, int *y_dest, int *bullet){
  extern bool packet_ready;
  if(packet_ready){
    extern int16_t mouse_xpos;
    extern int16_t mouse_ypos;
    moveCursor(mouse_xpos,mouse_ypos);
    extern bool lmbPressed;
    if(lmbPressed && *bullet == 1){
      extern Sprite cursor;
      *x_dest = cursor.x + cursor.width / 2; 
      *y_dest = cursor.y +cursor.height / 2; 
      *bullet = 2;
    }
  }
  return 0;  
}

/**
 * @brief Mouse menu position handler
 * 
 * @param finished 
 * @param state 
 * @return int 
 */
int mouse_menu_state(bool *finished, enum gameState *state){
  extern bool packet_ready;
  if(packet_ready){
    extern int16_t mouse_xpos;
    extern int16_t mouse_ypos;
    moveCursor(mouse_xpos,mouse_ypos);
    extern bool lmbPressed;
    if(lmbPressed){
      extern Sprite cursor;
      int xPos = cursor.x + cursor.width / 2, yPos = cursor.y + cursor.height / 2;
      //check collision
      if(xPos >= (getxRes() / 2) - (5 * 34) && xPos <= (getxRes() / 2) && yPos >= 350 && yPos < 390){
        *state = GAME;
        clear_menu();
        setTimerCounter();
      }
      if(xPos >= (getxRes() / 2) - (5 * 34) && xPos <= (getxRes() / 2) + (6 * 34) && yPos >= 400 && yPos < 440){
        *state = LEADERBOARD;
        clear_menu();
        setTimerCounter();
      }
      if(xPos >= (getxRes() / 2) - (5 * 34) && xPos <= (getxRes() / 2) - 34 && yPos > 450 && yPos <= 490){
        *finished = true; //if in exit area 
      }
    }
  }
  return 0;
}

/**
 * @brief Leaderboard mouse handler
 * 
 * @param state 
 * @return int 
 */
int mouse_leaderboard_state(enum gameState *state){
  extern bool packet_ready;

  if(packet_ready){
    extern int16_t mouse_xpos;
    extern int16_t mouse_ypos;
    moveCursor(mouse_xpos,mouse_ypos);
    extern bool lmbPressed;
    if(lmbPressed){
      extern Sprite cursor;
      int xPos = cursor.x + cursor.width / 2, yPos = cursor.y + cursor.height / 2;
      if(xPos >= (getxRes() / 2) - (5 * 114) && xPos <= (getxRes() / 2) - (5*94) && yPos >= 0 && yPos <= 20){
        *state = MENU;
        clear_leaderboard();
        setTimerCounter();
      }
    }

  }

  return 0;
}

/**
 * @brief Loops while game is running, handles interrupts and calls the respective functions
 * 
 * @param timer_bit_no 
 * @param keyboard_bit_no 
 * @param mouse_bit_no 
 * @return int 
 */
//! \callergraph
//! \callgraph
int start_game(uint8_t timer_bit_no, uint8_t keyboard_bit_no, uint8_t mouse_bit_no){

  keyboard_subscribe_interrupts(&keyboard_bit_no);
  mouse_subscribe_it(&mouse_bit_no);
  disableINT();
  mouse_enable_data_reporting_manMade();
  enableINT();
  timer_subscribe_int(&timer_bit_no);
  vg_init(MODE);
  createSprites();

  int r;
  int ipc_status;
  message msg;
  int counter = 0;
  int spriteNum = 1;
  int bullet = 1;
  enum gameState state = MENU;
  int score = 0, timer = 99, hp = 5;
  int x_dest; int y_dest;
  bool finished = false;
  //time_t currentTime = time(NULL);
  //struct tm *localTime = localtime(&currentTime);

  while ((getData() != 0x81 && getError() == 0) && !finished) {
    /* Get a request message. */
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
      printf("driver_receive failed with: %d", r);
      continue;
    }
    if (is_ipc_notify(ipc_status)) { /* received notification */
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE: /* hardware interrupt notification */
          if (msg.m_notify.interrupts & BIT(keyboard_bit_no)) {
            kbc_ih();
            logic_for_bytes();
            if(state == GAME){
              if(getData() == A_KEY){
                clear_player();
                movePlayer(PIXELS_PER_MOVE,LEFT);
              }
              else if(getData() == D_KEY){
                clear_player();
                movePlayer(PIXELS_PER_MOVE,RIGHT);
              }
              else{
              }
            }
          }
          if (msg.m_notify.interrupts & BIT(mouse_bit_no)) {
            mouse_ih();
            logic_for_packets();
            if(state == GAME){
              mouse_game_state(&x_dest,&y_dest,&bullet);
            }
            if(state == MENU){
              mouse_menu_state(&finished,&state);
            }
            if(state == LEADERBOARD){
              mouse_leaderboard_state(&state);
            }
          }
          if(bullet == 2 && state == GAME){
              createBullet(x_dest, y_dest);
              bullet = 0;
            }
          if (msg.m_notify.interrupts & BIT(timer_bit_no)) {
            if(state == GAME){
              //cleanLeaderboard(); //only uncomment if you want to clear the leaderboard
              counter++;
              timer_game_state(&counter,spriteNum,&bullet, &score, &timer, &hp);
              if(timer == 0 || hp <= 0 || allEnemiesDead()){
                vg_clear_area(0,0,getxRes(),getyRes());
                createSprites();
                resetBullets();
                updateLeaderboard(score);
                counter = 0;
                state = MENU;
                timer = 99;
                extern bool direction;
                extern bool vertical;
                direction = true;
                vertical = false;
                score = 0;
                hp = 5;
              }
            }
            if(state == MENU){
              timer_menu_state();
            }
            if(state == LEADERBOARD){
              timer_leaderboard_state();
            }
          }
          break;
        default:
          break;
      }
    }
    else {
      /* no standard messages expected: do nothing */
    }
    }
    keyboard_unsubscribe_interrupts();
    disableINT();
    mouse_disable_data_reporting();
    enableINT();
    mouse_unsubscribe_it();
    timer_unsubscribe_int();
    free_buffers();
    vg_exit();
    return 0;
}
