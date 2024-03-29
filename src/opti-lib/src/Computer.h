#ifndef __COMPUTER_H
#define __COMPUTER_H

#include <Util/Str.hpp>

class Computer
{
public:
	Computer();
	virtual ~Computer();

	EnjoLib::Str name;
	EnjoLib::Str hostname;
	EnjoLib::Str macAddr;
	double cores = 2;
	double wattPerCore = 11.5;
	double wattIdle = 5;
	double wattAsleep = 1;
	double hashPerCore = 250;
	double maxTempCelcius = 50;
	double scalingFactor = 0.85;
	int minRunHours = 2;

	bool isPoweroff = false; // For computers that get stuck or just should be powered off

	EnjoLib::Str Print() const;
	double GetHashRate(double freqGhz) const;
	double GetUsage(double freqGhz) const;

protected:

private:

};

#endif // __COMPUTER_H
