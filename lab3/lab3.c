#include <lcom/lcf.h>

#include "i8042.h"
#include "keyboard.h"
#include "timer.h"

#include <stdbool.h>
#include <stdint.h>

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/lab3/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab3/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int(kbd_test_scan)() {
    uint8_t bit_no = 0;
    int r;
    int ipc_status;
    message msg;

    keyboard_subscribe_interrupts(&bit_no);
    while( getData() != BREAK_ESC && getError() == 0 ) { 
      /* Get a request message. */
      if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) { 
        printf("driver_receive failed with: %d", r);
        continue;
      }
      if (is_ipc_notify(ipc_status)) { /* received notification */
         switch (_ENDPOINT_P(msg.m_source)) {
            case HARDWARE: /* hardware interrupt notification */				
               if (msg.m_notify.interrupts & BIT(bit_no)) { /* subscribed interrupt */
                  kbc_ih();
                  logic_for_bytes();
               }
               break;
            default:
               break; /* no other notifications expected: do nothing */	
           }
      } 
      else { /* received a standard message, not a notification */
           /* no standard messages expected: do nothing */
      }
    }   
    keyboard_unsubscribe_interrupts();
    kbd_print_no_sysinb(getCNT());   
    return 0;
}

int(kbd_test_poll)() {
  
  uint8_t stat;
  while(getData() != BREAK_ESC){
    uint8_t data;
    util_sys_inb(KBC_STATUS_REG, &stat);
    if(stat & FULL_OUT_BUFFER && (stat &(PARITY_ERROR | TIMEOUT_ERROR)) == 0) {
      util_sys_inb(KBC_OUT_CMD, &data);
      setData(data);
      logic_for_bytes();
    }
  }
  enable_interrupts();
  kbd_print_no_sysinb(getCNT());
  return 0;
}

int(kbd_test_timed_scan)(uint8_t n) {
    uint8_t bit_no_timer = 0;
    uint8_t bit_no_kbc = 1;
    int timer = n;
    int r;
    int ipc_status;
    message msg;
    timer_subscribe_int(&bit_no_timer);
    keyboard_subscribe_interrupts(&bit_no_kbc);
    while((getData() != BREAK_ESC && getError() == 0) && timer > 0) { 
      /* Get a request message. */
      if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) { 
        printf("driver_receive failed with: %d", r);
        continue;
      }
      if (is_ipc_notify(ipc_status)) { /* received notification */
         switch (_ENDPOINT_P(msg.m_source)) {
            case HARDWARE: /* hardware interrupt notification */				
               if (msg.m_notify.interrupts & BIT(bit_no_kbc)) { /* subscribed interrupt */
                  kbc_ih();
                  logic_for_bytes();
                  setTimerCounter();
                  timer = n;
               }
               if (msg.m_notify.interrupts & BIT(bit_no_timer)) {
                  if(getTimerCounter() % 60 == 0){
                    timer--;
                  }
                  timer_int_handler();
               }
               break;   
            default:
               break; /* no other notifications expected: do nothing */	
           }
      } 
      else { /* received a standard message, not a notification */
           /* no standard messages expected: do nothing */
      }
    }
    timer_unsubscribe_int();   
    keyboard_unsubscribe_interrupts();
    kbd_print_no_sysinb(getCNT());   
    return 0;
}
