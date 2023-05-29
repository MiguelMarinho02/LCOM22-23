#pragma once
#include <lcom/lcf.h>
  
int mouse_subscribe_it(uint8_t *bit_no);
int mouse_unsubscribe_it();
 
void logic_for_packets();

int mouse_disable_data_reporting();
int mouse_enable_data_reporting_manMade();
int (mouse_write_to_KBC)(int port, uint8_t cmd);
void (set_count)(uint32_t j);
uint32_t (get_count)();
int disableINT();
int enableINT();
