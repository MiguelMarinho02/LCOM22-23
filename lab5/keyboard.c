#include <lcom/lcf.h>

#include <lcom/lab3.h>
#include "i8042.h"

#include <stdbool.h>
#include <stdint.h>

static int hook_id = 1;
int error = 0;
uint8_t data = 0;
uint8_t bytes[2];
int cnt = 0;

int (util_sys_inb)(int port, uint8_t *value) {
  uint32_t value_32bit = 0;
  if(sys_inb(port, &value_32bit) != 0){
    printf("Error in util_sys_inb\n");
    return -2;
  }
  #ifdef LAB3
  cnt++;
  #endif
  *value =  (uint8_t)value_32bit;
  return 0;
}

uint8_t getData(){
    return data;
}

void setData(uint8_t val){
    data = val;
}

int getError(){
    return error;
}

int getCNT(){
    return cnt;
}

void logic_for_bytes(){
    if(error ==  1){
        return;
    }
    bool make = data & MAKE_CODE;
    int bit_count = 0;
    if(data == TWO_BYTES && bytes[0] == 0){
        bytes[0] = data;
        return;
    }
    else if(bytes[0] == TWO_BYTES){
        bytes[1] = data;
        make = data & MAKE_CODE;
        bit_count = 2;
    }
    else{
        bytes[0] = data;
        make = data & MAKE_CODE;
        bit_count = 1;
    }

    //kbd_print_scancode(!make,bit_count,bytes);
    bytes[0] = 0;
    bytes[1] = 0;
}

void (kbc_ih)() {
    uint8_t stat = 0;
    for(int i = 0; i < 5; i++) {
        util_sys_inb(KBC_STATUS_REG, &stat); /* assuming it returns OK */
        /* loop while 8042 output buffer is empty */
        if( stat & FULL_OUT_BUFFER ) {
            util_sys_inb(KBC_OUT_CMD, &data); /* ass. it returns OK */
            if ( (stat &(PARITY_ERROR | TIMEOUT_ERROR)) == 0 ){
                error = 0;
            }
            else{
                error = 1;
            }
        }
        //tickdelay(micros_to_ticks(WAIT_KBC)); //This has to be commented for some reason 
    }
}

int (keyboard_read)(uint8_t *data){
    uint8_t stat = 0;
    for(int i = 0; i < MAX_ATTEMPS; i++) {
        util_sys_inb(KBC_STATUS_REG, &stat); /* assuming it returns OK */
        /* loop while 8042 output buffer is empty */
        if( stat & FULL_OUT_BUFFER ) {
            util_sys_inb(KBC_OUT_CMD, data); /* ass. it returns OK */
            if ( (stat &(PARITY_ERROR | TIMEOUT_ERROR)) == 0 ){
                return 0;
            }
            else{
            }
        }
        tickdelay(micros_to_ticks(WAIT_KBC));
    }
    return -1;
}

int (keyboard_write_to_KBC)(int port, uint8_t cmd){
    uint8_t stat;
    for(int i = 0; i < MAX_ATTEMPS; i++) {
        util_sys_inb(KBC_STATUS_REG, &stat); /* assuming it returns OK */
        /* loop while 8042 input buffer is not empty */
        if( (stat & FULL_IN_BUFFER) == 0 ) {
            sys_outb(port, cmd); /* no args command */
            return 0;
        }
        tickdelay(micros_to_ticks(WAIT_KBC)); // e.g. tickdelay()
    }
    return -1;
}

int (enable_interrupts)(){
    uint8_t data;
    if(keyboard_write_to_KBC(KBC_STATUS_REG,KBC_READ_CMD)!=OK){
        return -1;
    }

    if(keyboard_read(&data) != OK){
        return -1;
    }

    data |= ENABLE_INT;

    if(keyboard_write_to_KBC(KBC_STATUS_REG,KBC_WRITE_CMD)!=OK){
        return -1;
    }

    if(keyboard_write_to_KBC(KBC_WRITE_CMD,data)!=OK){
        return -1;
    }

    return 0;
}

int (keyboard_subscribe_interrupts)(uint8_t *bit_no){
    *bit_no = hook_id;
    sys_irqsetpolicy(IRQ_KEYBOARD, IRQ_REENABLE | IRQ_EXCLUSIVE, &hook_id);
    return 0;
}

int (keyboard_unsubscribe_interrupts)(){
    sys_irqrmpolicy(&hook_id);
    return 0;
}

int (keyboard_restore)(){
    return 0;
}
