#include "timer.h"

void timer::startTimer()
{
	startTime = high_resolution_clock::now();
}

void timer::endTimer()
{
	endTime = high_resolution_clock::now();
}

long long timer::getDuration()
{
	auto duration = duration_cast<std::chrono::microseconds>(endTime - startTime);
	return duration.count();
}

std::ostream& operator<<(std::ostream& out, const timer& t)
{
	auto duration = duration_cast<std::chrono::microseconds>(t.endTime - t.startTime);

	long long mcs_duration = duration.count();

	long long ms_duration = mcs_duration / 1000;
	mcs_duration %= 1000;

	long long s_duration = ms_duration / 1000;
	ms_duration %= 1000;

	out << s_duration << "s " << ms_duration << "ms " << mcs_duration << "mcs";

	return out;
}
