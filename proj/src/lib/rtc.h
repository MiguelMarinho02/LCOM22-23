#include <lcom/lcf.h>

#define RTC_ADDR_REG 0x70
#define RTC_DATA_REG 0x71
#define DAY_MONTH_REG 0x07
#define MONTH_REG 0x08
#define YEAR_REG 0x09
#define REGISTER_B 0x0B
#define REGISTER_A 0x0A
#define RTC_DM BIT(2)
#define RTC_UIP BIT(7)

struct Date{
    uint8_t day;
    uint8_t month;
    uint8_t year;
};

int (prepareReadingWriting)(uint8_t reg);
int (readData)(uint8_t *num);
int (writeDataRTC)(uint8_t data);
struct Date fecthCurrentDate();
