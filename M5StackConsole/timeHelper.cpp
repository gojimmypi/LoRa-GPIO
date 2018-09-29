// 
// 
// 
#include "GlobalDefine.h"

#include "timeHelper.h"


// see https://www.epochconverter.com/
#define RTC_TEST 1537304154 // = Monday 13 November 2017 17:07:05 UTC

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
	// if no time is avalable, then try to set time from the network
	if (now <= 1500000000) {
		// try to set network time via ntp packets
		configTime(myTimezone * SecondsPerHour, dst * 0, "pool.ntp.org", "time.nist.gov");
		setenv("TZ", "PST8PDT,M4.1.0/02:00:00,M10.5.0/02:00:00", 1); // see https://users.pja.edu.pl/~jms/qnx/help/watcom/clibref/global_data.html
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
	Serial.print("Network time:");  Serial.println(now);
	Serial.println(ctime(&now));

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