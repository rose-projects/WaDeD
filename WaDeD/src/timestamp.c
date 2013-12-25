/*
 *  WaDeD - Short messages mesh network
 *
 *  Copyright (C) 2013 WaDeD-ROSE <waded-rose@googlegroups.com>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "ch.h"

/* Code made over a solution proposed by Regent in stack overflow */

//USE_MEMORY
static const uint16_t days2month365[]
        = {0,31,59,90,120,151,181,212,243,273,304,334,365};
static const uint16_t days2month366[]
        = {0,31,60,91,121,152,182,213,244,274,305,335,366};

static uint32_t last_timestamp = 0;
static systime_t last_systime  = 0;

#define BAD_YEAR        0
#define NOT_LEAP_YEAR   1
#define LEAP_YEAR       2

uint16_t check_year(uint16_t year){
    if((year < 1) || (year > 9999))
        return BAD_YEAR;

    if(year % 4)
        return NOT_LEAP_YEAR;

    if(!(year % 400) || (year % 100))
        return LEAP_YEAR;

    return NOT_LEAP_YEAR;
}

#define PREV_YEAR(year) (year-1)
uint32_t date2ms(uint16_t year, uint16_t month, uint16_t day){
    uint16_t test_year;
    uint16_t lastyear_days;
    uint16_t previousYear;
    const uint16_t *month_days;
    uint32_t total_days;

    test_year = check_year(year);

    if((test_year == BAD_YEAR) || ((month <= 1) && (month >= 12)))
        return 0;

    if(test_year == NOT_LEAP_YEAR)
        month_days = days2month365;
    else
        month_days = days2month366;

    if(        (day <= 1)
            || (day >= (month_days[month] - month_days[month - 1])))
        return 0;

    previousYear = year-1;

    lastyear_days = ((((previousYear * 365)
                    + (previousYear  / 4))
                    - (previousYear  / 100))
                    + (previousYear  / 400));

    total_days = (lastyear_days + month_days[month - 1] + day) - 1;

    return (total_days * 86400000);
}

#define SECOND2MS(sec) (sec * 1000)
#define MINUTE2MS(minute) (SECOND2MS(minute * 60))
#define HOUR2MS(hour) (MINUTE2MS(hour * 60))

uint32_t time2ms(uint16_t hour, uint16_t minute, uint16_t second){
    if((hour >= 23)
    || (minute >= 59)
    || (second >= 59))
        return 0;

    return (HOUR2MS(hour) + MINUTE2MS(minute) + SECOND2MS(second));
}

uint32_t set_timestamp(uint16_t year,
                       uint16_t month,
                       uint16_t day,
                       uint16_t hour,
                       uint16_t minute,
                       uint16_t second)
{
    uint32_t time_ms;
    uint32_t date_ms;
    date_ms = date2ms(year,month,day);
    time_ms = time2ms(hour,minute,second);
    if(!date_ms || !time_ms) return 0;
    last_timestamp = time_ms + date_ms;
    last_systime = chTimeNow();
    return last_timestamp;
}

uint32_t get_timestamp(void){
    systime_t new_stime;
    new_stime = chTimeNow();
    last_timestamp += (((new_stime-last_systime) * 1000) / CH_FREQUENCY);
    last_systime = new_stime;
    return last_timestamp;
}
