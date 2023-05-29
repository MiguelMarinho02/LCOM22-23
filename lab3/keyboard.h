#ifndef _LCOM_KEYBOARD_H_
#define _LCOM_KEYBOARD_H_

#include <minix/sysutil.h>
#include <lcom/lcf.h>
#include "i8042.h"

int getError();
int getCNT();
int getData();
void setData();
void logic_for_bytes();

int (keyboard_write_to_KBC)(int port, uint8_t cmd);

int (keyboard_read)(uint8_t *data);

int (enable_interrupts)();

int (util_sys_inb)(int port, uint8_t *value);

int (keyboard_subscribe_interrupts)(uint8_t *bit_no);

int (keyboard_unsubscribe_interrupts)();

void (kbc_ih)();

int (keyboard_restore)();

#endif
