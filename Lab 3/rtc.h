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

// --- Initialization ---
// Configure RTC device settings, addressing, and verify communication.
// Return true on success, false on error.
bool RTC_Init(void);

// --- Single Read/Write ---
// Retrieve the full date/time from RTC. Returns true if successful.
bool RTC_ReadTime(RTC_Time_t *timeData);

// Write a full date/time to RTC. Returns true if successful.
bool RTC_WriteTime(const RTC_Time_t *timeData);

// --- Alarm or Timing operations (depending on device capability) ---
// Start asynchronous timing function (e.g., enable alarm/periodic interrupt).
bool RTC_Start(void);

// Stop timing/alarm operation prematurely.
bool RTC_Stop(void);

// Return true when RTC indicates timing/alarm event completed.
bool RTC_IsDone(void);

// Optional: Burst read/write for multiple registers if needed
int RTC_BurstRead(uint8_t *buffer, int maxBytes);
bool RTC_BurstWrite(const uint8_t *buffer, int numBytes);

#endif  // RTC_H
