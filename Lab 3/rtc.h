#ifndef RTC_H
#define RTC_H

#include <stdint.h>
#include <stdbool.h>   // BOOL-equivalent type
#include <stddef.h>    // Needed for NULL and size_t

// --- Time structure to hold RTC results ---
typedef struct
{
    uint8_t seconds;
    uint8_t minutes;
    uint8_t hours;
    uint8_t day;
    uint8_t date;
    uint8_t month;
    uint16_t year;
} RTC_Time_t;


bool RTC_Init(void);


bool RTC_ReadTime(RTC_Time_t *timeData);


bool RTC_WriteTime(const RTC_Time_t *timeData);


bool RTC_Start(void);


bool RTC_Stop(void);


bool RTC_IsDone(void);

int RTC_BurstRead(uint8_t *buffer, int maxBytes);
bool RTC_BurstWrite(const uint8_t *buffer, int numBytes);

#endif  // RTC_H
