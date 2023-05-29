// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>

#include <stdint.h>
#include <stdio.h>
#include "mouse.h"
#include "timer.h"

// Any header files included below this line should have been created by you

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need/ it]
  lcf_trace_calls("/home/lcom/labs/lab4/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab4/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}


int (mouse_test_packet)(uint32_t cnt) {
    int r;
    message msg;
    int ipc_status;
    set_count(cnt);
    uint8_t bit_no = 0;
    mouse_subscribe_it(&bit_no);
    disableINT();
    mouse_enable_data_reporting_manMade();
    enableINT();
    while( get_count() != 0 ) { 
      /* Get a request message. */
      if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) { 
        printf("driver_receive failed with: %d", r);
        continue;
      }
      if (is_ipc_notify(ipc_status)) { /* received notification */
         switch (_ENDPOINT_P(msg.m_source)) {
            case HARDWARE: /* hardware interrupt notification */				
               if (msg.m_notify.interrupts & BIT(bit_no)) { /* subscribed interrupt */
                  mouse_ih();
                  logic_for_packets();
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
    disableINT();
    mouse_disable_data_reporting();
    enableINT();
    mouse_unsubscribe_it();
    return 0;
}

int (mouse_test_async)(uint8_t idle_time) {
    int r;
    message msg;
    int ipc_status;
    uint8_t bit_no_mouse = 1;
    uint8_t bit_no_timer = 0;
    timer_subscribe_int(&bit_no_timer);
    mouse_subscribe_it(&bit_no_mouse);
    disableINT();
    mouse_enable_data_reporting_manMade();
    enableINT();

    uint8_t n = idle_time;
    while( n != 0 ) { 
      /* Get a request message. */
      if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) { 
        printf("driver_receive failed with: %d", r);
        continue;
      }
      if (is_ipc_notify(ipc_status)) { /* received notification */
         switch (_ENDPOINT_P(msg.m_source)) {
            case HARDWARE: /* hardware interrupt notification */				
               if (msg.m_notify.interrupts & BIT(bit_no_mouse)) { /* subscribed interrupt */
                  mouse_ih();
                  logic_for_packets();
                  setTimerCounter();
                  n = idle_time;
               }
               if (msg.m_notify.interrupts & BIT(bit_no_timer)) { /* subscribed interrupt */
                  if(getTimerCounter() % 60 == 0){
                    n--;
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
    disableINT();
    mouse_disable_data_reporting();
    enableINT();
    mouse_unsubscribe_it();
    return 0;
}

int (mouse_test_gesture)(uint8_t x_len, uint8_t tolerance) {
    /* To be completed */
    printf("%s: under construction\n", __func__);
    return 1;
}

int (mouse_test_remote)(uint16_t period, uint8_t cnt) {
    /* This year you need not implement this. */
    printf("%s(%u, %u): under construction\n", __func__, period, cnt);
    return 1;
}
