// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>

#include <lcom/lab5.h>

#include <stdint.h>
#include <stdio.h>
#include "keyboard.h"
#include "gpu.h"
#include "timer.h"

// Any header files included below this line should have been created by you

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/lab5/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab5/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int(video_test_init)(uint16_t mode, uint8_t delay) {
  vg_init(mode);
  sleep(delay);
  vg_exit();
  return 0;
}

int(video_test_rectangle)(uint16_t mode, uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color) {
  vg_init(mode);
  vg_draw_rectangle(x,y,width,height,color);
  uint8_t bit_no = 1;
  int r;
  int ipc_status;
  message msg;

  keyboard_subscribe_interrupts(&bit_no);
  while( getData() != 0x81 && getError() == 0 ) { 
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
  vg_exit();
  return 0;
}

int(video_test_pattern)(uint16_t mode, uint8_t no_rectangles, uint32_t first, uint8_t step) {
  vg_init(mode);

  uint8_t bit_no = 1;
  int r;
  int ipc_status;
  message msg;

  keyboard_subscribe_interrupts(&bit_no);
  while( getData() != 0x81 && getError() == 0 ) { 
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
  vg_exit();
  return 0;
}

int(video_test_xpm)(xpm_map_t xpm, uint16_t x, uint16_t y) {
  vg_init(0x105);
  xpm_image_t image = vg_xpm_map_to_image(xpm);
  vg_xpm(image,x,y);
  uint8_t bit_no = 1;
  int r;
  int ipc_status;
  message msg;

  keyboard_subscribe_interrupts(&bit_no);
  while( getData() != 0x81 && getError() == 0 ) { 
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
  vg_exit();
  return 0;
}

int(video_test_move)(xpm_map_t xpm, uint16_t xi, uint16_t yi, uint16_t xf, uint16_t yf, int16_t speed, uint8_t fr_rate) {
  vg_init(0x105);
  xpm_image_t image = vg_xpm_map_to_image(xpm);
  uint8_t bit_no_kbc = 1;
  uint8_t bit_no_timer = 0;
  int r;
  int ipc_status;
  message msg;
  bool finished = true;

  keyboard_subscribe_interrupts(&bit_no_kbc);
  timer_subscribe_int(&bit_no_timer);
  vg_xpm(image,xi,yi);
  while( getData() != 0x81 && finished ) { 
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
              }
              if (msg.m_notify.interrupts & BIT(bit_no_timer)) { /* subscribed interrupt */
                timer_int_handler();
                if(xi >= xf && yi >= yf){
                  finished = false;
                  continue;
                }
                if(speed > 0){
                  if (getTimerCounter() % (sys_hz() / fr_rate) == 0){
                    setTimerCounter();
                    if(xi == xf){
                      vg_clear(image,xi,yi);
                      yi += speed;
                      vg_xpm(image,xi,yi);
                    }
                    else if(yi == yf){
                      vg_clear(image,xi,yi);
                      xi += speed;
                      vg_xpm(image,xi,yi);
                    }
                  }
                }
                else{
                  if(getTimerCounter() % abs(speed) == 0){
                    if(xi == xf){
                      vg_clear(image,xi,yi);
                      yi += 1;
                      vg_xpm(image,xi,yi);
                    }
                    else if(yi == yf){
                      vg_clear(image,xi,yi);
                      xi += 1;
                      vg_xpm(image,xi,yi);
                    }
                  }
                  else{
                    vg_xpm(image,xi,yi);
                  }
                }
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
  timer_unsubscribe_int();
  vg_exit();
  return 0;
}

int(video_test_controller)() {
  return 0;
}
