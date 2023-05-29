#include <lcom/lcf.h>

int vg_set_mode(uint16_t mode);
int vg_draw_pixel(uint16_t x, uint16_t y, uint32_t color);
int vg_xpm(xpm_image_t, uint16_t x, uint16_t y);
xpm_image_t vg_xpm_map_to_image(xpm_map_t xpm);
int vg_clear(xpm_image_t image, uint16_t x, uint16_t y);
int getxRes();
int getyRes();
int flip_buffer();
int free_buffers();
int vg_draw_char(xpm_image_t image, uint16_t x, uint16_t y,uint8_t *pnt);
int vg_clear_area(uint16_t x, uint16_t y, uint16_t x_len, uint16_t y_len);
int (vg_draw_hline)(uint16_t x, uint16_t y, uint16_t len, uint32_t color);
int (vg_draw_vline)(uint16_t x, uint16_t y, uint16_t len, uint32_t color);
