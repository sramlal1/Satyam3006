#include "rtc.h"

// ---------------------------------------------------------------------------
// NOTE: This implementation is communication-agnostic.
// Hardware-specific read/write functions must be supplied elsewhere.
// For example: low-level I2C/SPI functions like:
//     bool RTC_HW_Read(uint8_t reg, uint8_t *data, int len);
//     bool RTC_HW_Write(uint8_t reg, const uint8_t *data, int len);
// ---------------------------------------------------------------------------

// DS3231/RTC register addresses (example)
#define RTC_REG_SECONDS      0x00
#define RTC_REG_MINUTES      0x01
#define RTC_REG_HOURS        0x02
#define RTC_REG_DAY          0x03
#define RTC_REG_DATE         0x04
#define RTC_REG_MONTH        0x05
#define RTC_REG_YEAR         0x06

// Helper: BCD <-> Decimal conversion
static uint8_t bcd_to_bin(uint8_t val) { return (val & 0x0F) + ((val >> 4) * 10); }
static uint8_t bin_to_bcd(uint8_t val) { return ((val / 10) << 4) | (val % 10); }

bool RTC_Init(void)
{
    // TODO: Perform device presence check
    // Example: Try reading seconds register
    uint8_t dummy;
    if (!RTC_HW_Read(RTC_REG_SECONDS, &dummy, 1))
        return false;

    // TODO: Configure control registers if needed

    return true;
}

bool RTC_ReadTime(RTC_Time_t *timeData)
{
    if (timeData == NULL)
        return false;

    uint8_t raw[7];

    if (!RTC_HW_Read(RTC_REG_SECONDS, raw, sizeof(raw)))
        return false;

    // Convert BCD registers into readable decimal format
    timeData->seconds = bcd_to_bin(raw[0]);
    timeData->minutes = bcd_to_bin(raw[1]);
    timeData->hours   = bcd_to_bin(raw[2]);
    timeData->day     = bcd_to_bin(raw[3]);
    timeData->date    = bcd_to_bin(raw[4]);
    timeData->month   = bcd_to_bin(raw[5]);
    timeData->year    = 2000 + bcd_to_bin(raw[6]); // Adjust century

    return true;
}

bool RTC_WriteTime(const RTC_Time_t *timeData)
{
    if (timeData == NULL)
        return false;

    uint8_t raw[7];
    raw[0] = bin_to_bcd(timeData->seconds);
    raw[1] = bin_to_bcd(timeData->minutes);
    raw[2] = bin_to_bcd(timeData->hours);
    raw[3] = bin_to_bcd(timeData->day);
    raw[4] = bin_to_bcd(timeData->date);
    raw[5] = bin_to_bcd(timeData->month);
    raw[6] = bin_to_bcd(timeData->year - 2000); // Lower century only

    return RTC_HW_Write(RTC_REG_SECONDS, raw, sizeof(raw));
}

bool RTC_Start(void)
{
    // TODO: Enable periodic update or alarm if needed
    // Dummy implementation is always OK
    return true;
}

bool RTC_Stop(void)
{
    // TODO: Disable alarms or periodic update
    return true;
}

bool RTC_IsDone(void)
{
    // TODO: Poll status flag from RTC if needed
    // For now, always return false for polling examples
    return false;
}

int RTC_BurstRead(uint8_t *buffer, int maxBytes)
{
    if (buffer == NULL || maxBytes <= 0)
        return -1;

    // Read from start of time register block
    if (!RTC_HW_Read(RTC_REG_SECONDS, buffer, maxBytes))
        return 0;

    return maxBytes;
}

bool RTC_BurstWrite(const uint8_t *buffer, int numBytes)
{
    if (buffer == NULL || numBytes <= 0)
        return false;

    return RTC_HW_Write(RTC_REG_SECONDS, buffer, numBytes);
}
