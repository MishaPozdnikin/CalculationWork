#pragma once
#include <chrono>
#include <iostream>

using namespace std::chrono;

class timer
{
public:
	void startTimer();
	void endTimer();
	long long getDuration();

	friend std::ostream& operator<<(std::ostream& out, const timer& t);

private:
	steady_clock::time_point startTime;
	steady_clock::time_point endTime;
};
