// 
// 
// 
#include "GlobalDefine.h"

#include "timeHelper.h"


// see https://www.epochconverter.com/
#define RTC_TEST 1537304154 // = Monday 13 November 2017 17:07:05 UTC

// Timezone strings from https://sites.google.com/a/usapiens.com/opnode/time-zones
//
//  AUSTRALIA
//    Melbourne,Canberra,Sydney	AEST-10AEDT-11,M10.5.0/02:00:00,M4.1.0/03:00:00
//    Perth	AWST-8AWDT-9,M10.5.0,M3.5.0/03:00:00
//    Brisbane	AEST-10
//    Adelaide	ACST-9:30ACDT-10:30,M10.5.0/02:00:00,M4.1.0/03:00:00
//    Darwin	ACST-9:30
//    Hobart	AEST-10AEDT-11,M10.1.0/02:00:00,M4.1.0/03:00:00
//    
//    
//  EUROPE
//    Amsterdam, Netherlands	CET-1CEST-2,M3.5.0/02:00:00,M10.5.0/03:00:00
//    Athens, Greece	EET-2EEST-3,M3.5.0/03:00:00,M10.5.0/04:00:00
//    Barcelona, Spain	CET-1CEST-2,M3.5.0/02:00:00,M10.5.0/03:00:00
//    Berlin, Germany	CET-1CEST-2,M3.5.0/02:00:00,M10.5.0/03:00:00
//    Brussels, Belgium	CET-1CEST-2,M3.5.0/02:00:00,M10.5.0/03:00:00
//    Budapest, Hungary	CET-1CEST-2,M3.5.0/02:00:00,M10.5.0/03:00:00
//    Copenhagen, Denmark	CET-1CEST-2,M3.5.0/02:00:00,M10.5.0/03:00:00
//    Dublin, Ireland	GMT+0IST-1,M3.5.0/01:00:00,M10.5.0/02:00:00
//    Geneva, Switzerland	CET-1CEST-2,M3.5.0/02:00:00,M10.5.0/03:00:00
//    Helsinki, Finland	EET-2EEST-3,M3.5.0/03:00:00,M10.5.0/04:00:00
//    Kyiv, Ukraine	EET-2EEST,M3.5.0/3,M10.5.0/4
//    Lisbon, Portugal	WET-0WEST-1,M3.5.0/01:00:00,M10.5.0/02:00:00
//    London, Great Britain	GMT+0BST-1,M3.5.0/01:00:00,M10.5.0/02:00:00
//    Madrid, Spain	CET-1CEST-2,M3.5.0/02:00:00,M10.5.0/03:00:00
//    Oslo, Norway	CET-1CEST-2,M3.5.0/02:00:00,M10.5.0/03:00:00
//    Paris, France	CET-1CEST-2,M3.5.0/02:00:00,M10.5.0/03:00:00
//    Prague, Czech Republic	CET-1CEST-2,M3.5.0/02:00:00,M10.5.0/03:00:00
//    Roma, Italy	CET-1CEST-2,M3.5.0/02:00:00,M10.5.0/03:00:00
//    Moscow, Russia	MSK-3MSD,M3.5.0/2,M10.5.0/3
//    Sofia, Bulgaria	EET-2EEST-3,M3.5.0/03:00:00,M10.5.0/04:00:00
//    St.Petersburg, Russia 
//    MST-3MDT,M3.5.0/2,M10.5.0/3
//    Stockholm, Sweden	CET-1CEST-2,M3.5.0/02:00:00,M10.5.0/03:00:00
//    Tallinn, Estonia	EET-2EEST-3,M3.5.0/03:00:00,M10.5.0/04:00:00
//    Warsaw, Poland	CET-1CEST,M3.5.0,M10.5.0/3
//    
//    
//  NEW ZELAND
//    Auckland, Wellington	NZST-12NZDT-13,M10.1.0/02:00:00,M3.3.0/03:00:00
//    
//    
//    
//  USA AND CANADA
//    Hawaii Time
//    HAW10
//    Alaska Time	AKST9AKDT,M3.2.0,M11.1.0
//    Pacific Time
//    PST8PDT,M3.2.0,M11.1.0
//    Mountain Time	MST7MDT,M3.2.0,M11.1.0
//    Mountain Time (Arizona, no DST)
//    MST7
//    Central Time	CST6CDT,M3.2.0,M11.1.0
//    Eastern Time	EST5EDT,M3.2.0,M11.1.0
//    Atlantic Time	AST4ADT
//    Atlantic Time (New Brunswick)	AST4ADT,M4.1.0/00:01:00,M10.5.0/00:01:00
//    Newfoundland Time	NST+3:30NDT+2:30,M3.2.0/00:01:00,M11.1.0/00:01:00
//    
//    
//    
//  ASIA
//    Jakarta	WIB-7
//    Singapore	SGT-8
//    Hong Kong	HKT-8
//    Ulaanbaatar, Mongolia	ULAT-8ULAST,M3.5.0/2,M9.5.0/2
//    
//    
//  CENTRAL AND SOUTH AMERICA
//    Brazil, São Paulo	BRST+3BRDT+2,M10.3.0,M2.3.0
//    Colombia	UTC+5
//    Argentina	UTC+3
//    Central America	CST+6

// *******************************************************************************************
void setupLocalTime()
// *******************************************************************************************
{
    // see https://github.com/esp8266/Arduino/issues/4637
    time_t now;
    now = time(nullptr); // if there's no time, this will have a value of 28800; Thu Jan  1 08:00:00 1970 
    Serial.print("Initial time:"); Serial.println(now);
    Serial.println(ctime(&now));

    int myTimezone = -7;
    int dst = 0;
    int SecondsPerHour = 3600;
    int MAX_TIME_RETRY = 60;
    int i = 0;

    // it is unlikely that the time is already set since we have no battery; 
    // if no time is available, then try to set time from the network
    // TODO - investigate if somehow the time settings are saved in non-volatile memory
    if (now <= 1500000000) {
        // try to set network time via ntp packets
        configTime(0, 0, "pool.ntp.org", "time.nist.gov"); // see https://github.com/esp8266/Arduino/issues/4749#issuecomment-390822737

        // Starting in 2007, most of the United States and Canada observe DST from
        // the second Sunday in March to the first Sunday in November.
        // example setting Pacific Time:
        // setenv("TZ", "PST8PDT,M3.2.0/02:00:00,M11.1.0/02:00:00", 1); // see https://users.pja.edu.pl/~jms/qnx/help/watcom/clibref/global_data.html
        //                     | month 3, second sunday at 2:00AM
        //                                    | Month 11 - firsst Sunday, at 2:00am  
        // Mm.n.d
        //   The dth day(0 <= d <= 6) of week n of month m of the year(1 <= n <= 5, 1 <= m <= 12, where 
        //     week 5 means "the last d day in month m", which may occur in the fourth or fifth week).
        //     Week 1 is the first week in which the dth day occurs.Day zero is Sunday.

        // setenv("TZ", "Europe/Paris,M3.5.0/02:00:00,M10.5.0/02:00:00", 1); // see https://users.pja.edu.pl/~jms/qnx/help/watcom/clibref/global_data.html
        //                         | month 3, last Sunday at 2:00AM
        //                                         | month 10 - last Sunday, at 2:00am  
        //                                                            | overwrite = 1
        // 
        setenv("TZ", "CET-1CEST-2,M3.5.0/02:00:00,M10.5.0/03:00:00", 1);
        //                         | month 3, last Sunday at 2:00AM
        //                                         | month 10, last Sunday, at 2:00am  
        //                                                            | overwrite = 1
        tzset();
        Serial.print("Waiting for time(nullptr).");
        i = 0;
        while (!time(nullptr)) {
            Serial.print(".");
            delay(1000);
            i++;
            if (i > MAX_TIME_RETRY) {
                Serial.println("Gave up waiting for time(nullptr) to have a valid value.");
                break;
            }
        }
    }
    else {
        Serial.println("Skipped configTime();");
    }
    Serial.println("");

    // wait and determine if we have a valid time from the network. 
    now = time(nullptr);
    i = 0;
    Serial.print("Waiting for network time.");
    while (now <= 1500000000) {
        Serial.print(".");
        delay(1000); // allow a few seconds to connect to network time.
        i++;
        now = time(nullptr);
        if (i > MAX_TIME_RETRY) {
            Serial.println("Gave up waiting for network time(nullptr) to have a valid value.");
            break;
        }
    }
    Serial.println("");

    // get the time from the system
    char *tzvalue;
    tzvalue = getenv("TZ");
    Serial.print("Network time:");  Serial.println(now);
    Serial.println(ctime(&now));
    Serial.print("tzvalue for timezone = "); Serial.println(tzvalue);

    // TODO - implement a web service that returns current epoch time to use when NTP unavailable (insecure SSL due to cert date validation)

    // some networks may not allow ntp protocol (e.g. guest networks) so we may need to fudge the time
    if (now <= 1500000000) {
        Serial.println("Unable to get network time. Setting to fixed value. \n");
        // set to RTC text value
        // see https://www.systutorials.com/docs/linux/man/2-settimeofday/
        //
        //struct timeval {
        //	time_t      tv_sec;     /* seconds */
        //	suseconds_t tv_usec;    /* microseconds */
        //};
        timeval tv = { RTC_TEST, 0 };
        //
        //struct timezone {
        //	int tz_minuteswest;     /* minutes west of Greenwich */
        //	int tz_dsttime;         /* type of DST correction */
        //};
        timezone tz = { myTimezone * 60 , 0 };

        // int settimeofday(const struct timeval *tv, const struct timezone *tz);
        settimeofday(&tv, &tz);
    }

    now = time(nullptr);
    Serial.println("Final time:");  Serial.println(now);
    Serial.println(ctime(&now));
}

void printLocalTime()
{
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) {
        Serial.println("Failed to obtain time.");
        return;
    }
    Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
}