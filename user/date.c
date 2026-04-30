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
    int64 unix_time = get_time();

    int64 year, month, day, hour, minute, second, nanosecond;
    int64 epoch_day, sec_of_day, nsec;

    if (unix_time >= 0) {
        epoch_day = unix_time / (NS_PER_SEC * SEC_PER_DAY);
        int64 rem_ns = unix_time % (NS_PER_SEC * SEC_PER_DAY);
        sec_of_day = rem_ns / NS_PER_SEC;
        nsec = rem_ns % NS_PER_SEC;
    } else {
        int64 abs_ts = -unix_time;
        epoch_day = -(abs_ts / (NS_PER_SEC * SEC_PER_DAY));
        int64 rem_ns = abs_ts % (NS_PER_SEC * SEC_PER_DAY);
        if (rem_ns != 0) {
            epoch_day--;
            rem_ns = (NS_PER_SEC * SEC_PER_DAY) - rem_ns;
        }
        sec_of_day = rem_ns / NS_PER_SEC;
        nsec = rem_ns % NS_PER_SEC;
    }
    hour = sec_of_day / 3600;
    minute = (sec_of_day % 3600) / 60;
    second = sec_of_day % 60;
    nanosecond = nsec;

    if (epoch_day >= 0) {
        uint64 days = (uint64) epoch_day;
        year = 1970;
        while (days >= days_in_year(year)) {
            days -= days_in_year(year);
            year++;
        }
        month = 0;
        while (days >= days_in_month(month, year)) {
            days -= days_in_month(month, year);
            month++;
        }
        month++; // month are numbered starting with 1 (January)
        day = days + 1;
    } else {
        int64 days_before = -epoch_day;
        year = 1969;
        while (days_before > days_in_year((int)year)) {
            days_before -= days_in_year((int)year);
            year--;
        }
        month = 12;
        day = 31;
        int64 rem = days_before - 1;
        while (rem > 0) {
            if (day > 1) {
                day--;
            } else {
                month--;
                if (month == 0) {
                    month = 12;
                    year--;
                }
                day = days_in_month((int)month - 1, (int)year);
            }
            rem--;
        }
    }

    printf("%ld-%ld-%ld %ld:%ld:%ld.%ld\n", year, month, day, hour, minute, second, nanosecond);

    exit(0);
}