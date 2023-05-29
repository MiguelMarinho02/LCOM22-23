#include <lcom/lcf.h>
#include "timer.h"

#include <stdint.h>

#include "i8254.h"

int counter = 1;
static int hook_id = 0;

int (getTimerCounter)(){
  return counter;
}

void (setTimerCounter)(){
  counter = 1;
}

int (timer_set_frequency)(uint8_t timer, uint32_t freq) {
  /* To be implemented by the students */
  uint8_t st = 0;
  timer_get_conf(timer, &st);
  st = st << 4; //turns 4 MSB to 0;
  st = st >> 4;  
  st = st | TIMER_LSB_MSB;

  if(timer == 1){
    st = st | TIMER_SEL1;
  }
  if(timer == 2){
    st = st | TIMER_SEL2;
  }

  if(sys_outb(TIMER_CTRL,st) != OK){
    return -1;
  }

  uint16_t value_to_pass = TIMER_FREQ / freq;  //value to pass to timer register
  uint8_t lsb = 0, msb = 0; 

  util_get_LSB(value_to_pass,&lsb);
  util_get_MSB(value_to_pass,&msb); 

  sys_outb(TIMER_0+timer,lsb); //loads first the lsb and then the msb into the timer register
  sys_outb(TIMER_0+timer,msb);

  return 0;
}

int (timer_subscribe_int)(uint8_t *bit_no) {
  /* To be implemented by the students */
  *bit_no = hook_id;
  sys_irqsetpolicy(TIMER0_IRQ,IRQ_REENABLE, &hook_id);
  return 0;
}

int (timer_unsubscribe_int)() {
  /* To be implemented by the students */
  sys_irqrmpolicy(&hook_id);
  return 0;
}

void (timer_int_handler)() {
  /* To be implemented by the students */
  counter++;
}

int (timer_get_conf)(uint8_t timer, uint8_t *st) {
  /* To be implemented by the students */
  uint8_t command = TIMER_RB_CMD | BIT(5) | TIMER_RB_SEL(timer);
  int port = TIMER_CTRL;
  if(sys_outb(port,command) != OK){
    printf("Error in sys_out\n");
    return -1;
  }
  if(util_sys_inb(TIMER_0 + timer,st) != OK){
    printf("Error in sys_in_util\n");
    return -1;
  }
  //printf("%s is not yet implemented!\n", __func__);
  return 0;
}

int (timer_display_conf)(uint8_t timer, uint8_t st, enum timer_status_field field) {
  /* To be implemented by the students */
  union timer_status_field_val union_val;
  if(field == tsf_all){
    union_val.byte = st;
  }

  if(field == tsf_initial){
    if(st & BIT(5) && st & BIT(4)){
      union_val.in_mode = MSB_after_LSB;
    }
    else if(!(st & BIT(5)) && st & BIT(4)){
      union_val.in_mode = LSB_only;
    }
    else if(st & BIT(5) && !(st & BIT(4))){
      union_val.in_mode = MSB_only;
    }
  }

  if(field == tsf_mode){
  
    if(!(st & BIT(1)) && !(st & BIT(2)) && !(st & BIT(3))){ //for mode 0
      union_val.count_mode = 0;
    }
    else if((st & BIT(1)) && !(st & BIT(2)) && !(st & BIT(3))){ //for mode 1
      union_val.count_mode = 1;
    }
    else if((!(st & BIT(1)) && (st & BIT(2)) && !(st & BIT(3))) || (!(st & BIT(1)) && (st & BIT(2)) && (st & BIT(3)))){ //for mode 2
      union_val.count_mode = 2;
    }
    else if((st & BIT(1) && st & BIT(2) && st & BIT(3)) || (st & BIT(1) && st & BIT(2) && !(st & BIT(3)))){ // for mode 3
      union_val.count_mode = 3;
    }
    else if(!(st & BIT(1)) && !(st & BIT(2)) && (st & BIT(3))){ //for mode 4
      union_val.count_mode = 4;
    }
    else if((st & BIT(1)) && !(st & BIT(2)) && (st & BIT(3))){ //for mode 5
      union_val.count_mode = 5;
    }
  }

  if(field == tsf_base){
    if(st & BIT(0)){
      union_val.bcd = true;
    }
    else{
      union_val.bcd = false;
    }
  }
  
  timer_print_config(timer, field, union_val);
  //printf("%s is not yet implemented!\n", __func__);
  return 0;
}
