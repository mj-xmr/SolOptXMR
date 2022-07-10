#ifndef __BATTERYPARAMS_H
#define __BATTERYPARAMS_H

#include <Util/Str.hpp>

struct BatteryParams
{
	BatteryParams();
	virtual ~BatteryParams();

    int DISCHARGE_RATE_C_BY = 10;
    double MAX_DISCHARGE_AMP = 11;
    double GetMulPowerToCapacity(double systemVoltage) const;
    double MAX_CAPACITY_AMPH = 60;
    double MIN_LOAD_AMPH = MAX_CAPACITY_AMPH / 2; //# Assuming a lead-acid
    double DISCHARGE_PER_HOUR_PERCENT = 0.5;

    EnjoLib::Str Print() const;
};

#endif // __BATTERYPARAMS_H
