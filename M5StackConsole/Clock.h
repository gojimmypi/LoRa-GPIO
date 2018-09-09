#pragma once
class Clock
{
public:
    bool useNetworkTimeConfig = false;
    Clock();
	Clock(int HourValue, int MinuteValue, int SecondValue);
	void refreshDisplay();
	~Clock();


private:
	uint32_t targetTime = 0;                    // for next 1 second timeout

	uint8_t hh = conv2d(__TIME__), mm = conv2d(__TIME__ + 3), ss = conv2d(__TIME__ + 6); // Get H, M, S from compile time
																						 // Function to extract numbers from compile time string
	static uint8_t conv2d(const char* p) {
		uint8_t v = 0;
		if ('0' <= *p && *p <= '9')
			v = *p - '0';
		return 10 * v + *++p - '0';
	}

};

