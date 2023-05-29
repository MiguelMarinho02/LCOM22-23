#include <lcom/lcf.h>

#include "gpu.h"

#include <stdint.h>
#include <stdio.h>

static void *video_mem;
static unsigned int xRes = 0;
static unsigned int yRes = 0;
static unsigned int bytes_per_scanline = 0;
static unsigned int bits_per_pixel = 0;

void *(vg_init)(uint16_t mode){
    vbe_mode_info_t vmi_p;
    vbe_get_mode_info(mode, &vmi_p);

    int r;
    struct minix_mem_range mr;
    unsigned int vram_base = vmi_p.PhysBasePtr;  /* VRAM's physical addresss */
    unsigned int vram_size = vmi_p.XResolution * vmi_p.YResolution * vmi_p.BitsPerPixel / 8;  /* VRAM's size, but you can use */
    xRes = vmi_p.XResolution;
    yRes = vmi_p.YResolution;
    bytes_per_scanline = vmi_p.BytesPerScanLine;
    bits_per_pixel = vmi_p.BitsPerPixel;
    mr.mr_base = (phys_bytes) vram_base;	
    mr.mr_limit = mr.mr_base + vram_size;  

    if( 0 != (r = sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr))){
        panic("sys_privctl (ADD_MEM) failed: %d\n", r);
    }

    /* Map memory */

    video_mem = vm_map_phys(SELF, (void *)mr.mr_base, vram_size);

    if(video_mem == MAP_FAILED){
        panic("couldn't map video memory");   
    }
 
    vg_set_mode(mode);
    return video_mem;
}

int vg_set_mode(uint16_t mode) {
  reg86_t r86;
   
  /* Specify the appropriate register values */
  
  memset(&r86, 0, sizeof(r86));	/* zero the structure */

  r86.intno = 0x10; /* BIOS video services */
  r86.ah = 0x4f;    /* Set Video Mode function */
  r86.al = 0x02;    /* 80x25 text mode */
  r86.bx = mode | BIT(14);
  
  /* Make the BIOS call */

  if( sys_int86(&r86) != OK ) {
    printf("\tvg_exit(): sys_int86() failed \n");
    return 1;
  }
  return 0;
}

int (vg_draw_rectangle)(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color){
  for(int i = 0; i < height; i++){
    vg_draw_hline(x,y+i,width,color);
  }
  return 0;
}

int (vg_draw_hline)(uint16_t x, uint16_t y, uint16_t len, uint32_t color){
  for(int i = 0; i < len; i++){
    vg_draw_pixel(x+i,y,color);
  }
  return 0;
}

int vg_draw_pixel(uint16_t x, uint16_t y, uint32_t color){
  uint64_t offset = (y*xRes + x)*(bytes_per_scanline / xRes);
  void *addr = (void *)((char *) video_mem + offset);

  memcpy(addr,&color,bytes_per_scanline / xRes);

  return 0;
}

xpm_image_t vg_xpm_map_to_image(xpm_map_t xpm){
  xpm_image_t image;
  xpm_load(xpm,XPM_INDEXED,&image);
  return image;
}


int vg_xpm(xpm_image_t image, uint16_t x, uint16_t y){

  if (y + image.height > yRes)
    y = yRes - image.height;
  if (x + image.width > xRes)
    x = xRes - image.width;

  for(int i = 0; i < image.height; i++){
    for(int j = 0; j < image.width; j++){
      uint64_t offset = ((y+i)*xRes + x + j)*(bytes_per_scanline / xRes); //calculates the pos of the pixel we want to change
      void *addr = (void *)((char *) video_mem + offset);

      uint8_t value_to_advance = image.size / (image.height * image.width);

      memcpy(addr,image.bytes + value_to_advance*(i*(image.width) + j),bytes_per_scanline / xRes);     
    }
  }

  return 0;  
}

int vg_clear(xpm_image_t image, uint16_t x, uint16_t y){
  uint32_t transparent = 0x0000;
  if (y + image.height > yRes)
    y = yRes - image.height;
  if (x + image.width > xRes)
    x = xRes - image.width;

  for(int i = 0; i < image.height; i++){
    for(int j = 0; j < image.width; j++){
      uint64_t offset = ((y+i)*xRes + x + j)*(bytes_per_scanline / xRes); //calculates the pos of the pixel we want to change
      void *addr = (void *)((char *) video_mem + offset);

      //uint8_t value_to_advance = image.size / (image.height * image.width);

      memcpy(addr,image.bytes + transparent*(i*(image.width) + j),bytes_per_scanline / xRes);     
    }
  }

  return 0;
}
