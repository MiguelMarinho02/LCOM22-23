#include <lcom/lcf.h>

#include "utils.h"
#include "i8042.h"
#include "mouse.h"

#define IRQ12 12
#define AUX BIT(5)
#define DISABLE_DATA_REPORTING 0xF5
#define ENABLE_DATA_REPORTING 0xF4
#define WRITE_B_TO_MOUSE 0xD4

static int hook_id = 1;
static uint8_t data = 0;
int error = 0;
int bit_count = 0;
struct packet p;
uint32_t count = 0;

void (set_count)(uint32_t j){
    count = j;
}

uint32_t (get_count)(){
    return count;
}

int (mouse_subscribe_it)(uint8_t *bit_no){
    hook_id = *bit_no;
    return sys_irqsetpolicy(IRQ12, IRQ_REENABLE | IRQ_EXCLUSIVE, &hook_id);
}

int (mouse_unsubscribe_it)(){
    return sys_irqrmpolicy(&hook_id);
}

void logic_for_packets(){
    if(error == 1){
        bit_count = 0;
        return;
    }
    if(bit_count == 2){
        p.bytes[2] = data;

        if(BIT(7) & p.bytes[0]){
            p.y_ov = true;
        }
        if(BIT(6) & p.bytes[0]){
            p.x_ov = true;
        }
        if(BIT(5) & p.bytes[0]){
            p.delta_y = p.bytes[2] | 0xFF00;
        }
        else{
            p.delta_y = p.bytes[2];
        }
        if(BIT(4) & p.bytes[0]){
            p.delta_x = p.bytes[1] | 0xFF00;
        }
        else{
            p.delta_x = p.bytes[1];
        }
        if(BIT(0) & p.bytes[0]){
            p.lb = true;
        }    
        if(BIT(1) & p.bytes[0]){
            p.rb = true;
        }
        if(BIT(2) & p.bytes[0]){
            p.mb = true;
        }

        mouse_print_packet(&p);
        count--;

        for(int i = 0; i < 3; i++){
            p.bytes[i] = 0;
        }

        p.delta_x = 0;
        p.delta_y = 0;
        p.lb = false;
        p.mb = false;
        p.rb = false;
        p.x_ov = false;
        p.y_ov = false;
        bit_count = 0;
        return;
    }
    if(bit_count == 1){
        p.bytes[1] = data;
    }
    if(bit_count == 0 && (data & BIT(3))){
        p.bytes[0] = data;
    }
    bit_count++;
}

void (mouse_ih)(){
    uint8_t stat = 0;
    for(int i = 0; i < MAX_ATTEMPS; i++) {
        util_sys_inb(KBC_STATUS_REG, &stat); /* assuming it returns OK */
        /* loop while 8042 output buffer is empty */
        if( (stat & FULL_OUT_BUFFER) && (stat & AUX) ) {
            util_sys_inb(KBC_OUT_CMD, &data); /* ass. it returns OK */
            if ( (stat &(PARITY_ERROR | TIMEOUT_ERROR)) == 0 ){
                error = 0;
            }
            else{
                error = 1;
            }
        }
    }
}

int (mouse_write_to_KBC)(int port, uint8_t cmd){
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

int (mouse_read_KBC)(int port, uint8_t *data){
    uint8_t stat;
    for(int i = 0; i < MAX_ATTEMPS; i++) {
        util_sys_inb(KBC_STATUS_REG, &stat); /* assuming it returns OK */
        /* loop while 8042 input buffer is not empty */
        if( (stat & FULL_OUT_BUFFER) ) {
            util_sys_inb(port,data); /* no args command */
            return 0;
        }
        tickdelay(micros_to_ticks(WAIT_KBC)); // e.g. tickdelay()
    }
    return -1;
}


int mouse_disable_data_reporting(){

    if(mouse_write_to_KBC(0x64,WRITE_B_TO_MOUSE) != 0){
        return -1;
    } //0x64 is kbc status reg port

    if(mouse_write_to_KBC(0x60,DISABLE_DATA_REPORTING) != 0){
        return -1;
    }

    uint8_t data;
    if(mouse_read_KBC(0x60,&data) != 0){
        return -1;
    }
    if(data == 0xFA){
        return 0;
    }

    return -1;
}

int mouse_enable_data_reporting_manMade(){

    if(mouse_write_to_KBC(0x64,WRITE_B_TO_MOUSE) != 0){
        return -1;
    } //0x64 is kbc status reg port

    if(mouse_write_to_KBC(0x60,ENABLE_DATA_REPORTING) != 0){
        return -1;
    }

    uint8_t data;
    if(mouse_read_KBC(0x60,&data) != 0){
        return -1;
    }
    if(data == 0xFA){
        return 0;
    }

    return -1;
}

int disableINT(){
    sys_irqdisable(&hook_id);
    return 0;
}

int enableINT(){
    sys_irqenable(&hook_id);
    return 0;
}
