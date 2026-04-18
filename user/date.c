#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define NS_PER_SEC 1000000000LL
#define SEC_PER_DAY 86400LL

int is_leap_year(int year)
{
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

int days_in_month(int month, int year)
{
    static const int days[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (month == 1 && is_leap_year(year))
        return 29;
    return days[month];
}

static int days_in_year(int year)
{
    return is_leap_year(year) ? 366 : 365;
}

int main(void)
{
    uint64 unix_time = get_time();

    uint64 year, month, day, hour, minute, second, nanosecond;

    uint64 total_seconds = unix_time / NS_PER_SEC;
    nanosecond = unix_time % NS_PER_SEC;

    uint64 days_since_epoch = total_seconds / SEC_PER_DAY;
    long long remaining_seconds = total_seconds % SEC_PER_DAY;

    hour = remaining_seconds / 3600;
    minute = (remaining_seconds % 3600) / 60;
    second = remaining_seconds % 60;

    year = 1970;
    while (days_since_epoch >= days_in_year(year))
    {
        days_since_epoch -= days_in_year(year);
        ++year;
    }

    month = 0;
    while (days_since_epoch >= days_in_month(month, year))
    {
        days_since_epoch -= days_in_month(month, year);
        ++month;
    }
    ++month; // month are numbered starting with 1 (January)
    day = days_since_epoch + 1;

    printf("%ld-%ld-%ld %ld:%ld:%ld.%ld\n", year, month, day, hour, minute, second, nanosecond);

    exit(0);
}