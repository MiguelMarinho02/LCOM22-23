#include <lcom/lcf.h>

#include <stdint.h>

int(util_get_LSB)(uint16_t val, uint8_t *lsb) {
  /* To be implemented by the students */
  *lsb = (uint8_t) val;
  //printf("%s is not yet implemented!\n", __func__);
  return 0;
}

int(util_get_MSB)(uint16_t val, uint8_t *msb) {
  /* To be implemented by the students */
  val = val>>8;
  *msb = (uint8_t) val;
  //printf("%s is not yet implemented!\n", __func__);
  return 0;
}

/*int (util_sys_inb)(int port, uint8_t *value) { 
  uint32_t value_32bit = 0;
  if(sys_inb(port, &value_32bit) != 0){
    printf("Error in util_sys_inb\n");
    return -2;
  }
  *value =  (uint8_t)value_32bit;
  return 0;
}*/
