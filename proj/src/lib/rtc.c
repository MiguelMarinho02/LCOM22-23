#include "rtc.h"

#include <stdint.h>

/**
 * @brief Prepares the RTC to read or write to the RTC register
 * 
 */
int (prepareReadingWriting)(uint8_t reg){
    if(reg > 9 || reg < 0){return 1;}
    return sys_outb(RTC_ADDR_REG,reg);
}

/**
 * @brief Reads the data from the RTC register
 * 
 */
int (readData)(uint8_t *num){
    return util_sys_inb(RTC_DATA_REG,num);
}

/**
 * @brief  Writes data to the RTC register
 * 
 */
int (writeDataRTC)(uint8_t data){
    return sys_outb(RTC_DATA_REG,data);
}

/**
 * @brief Entire process necessary to read and write the current date to the RTC register and adapt it to a date structure that can be easily used
 * 
 * @return struct Date 
 */
struct Date fecthCurrentDate(){
    uint8_t num;

    prepareReadingWriting(REGISTER_B);
    readData(&num);

    num |= RTC_DM;

    prepareReadingWriting(REGISTER_B);
    writeDataRTC(num);

    uint8_t regA = 0;

    do{
        prepareReadingWriting(REGISTER_A);
        readData(&regA);
    } while(regA & RTC_UIP);

    tickdelay(micros_to_ticks(244));

    struct Date date;
    prepareReadingWriting(DAY_MONTH_REG);
    readData(&num);
    date.day = num;

    prepareReadingWriting(MONTH_REG);
    readData(&num);
    date.month = num;

    prepareReadingWriting(YEAR_REG);
    readData(&num);
    date.year = num;

    return date;
}
