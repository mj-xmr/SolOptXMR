#ifndef __BATTERYPARAMS_H
#define __BATTERYPARAMS_H

#include <Util/Str.hpp>

struct BatteryParams
{
	BatteryParams();
	virtual ~BatteryParams();

    double MAX_DISCHARGE_AMP = 11;
    /// TODO: Get the conversion right:
    static constexpr const double MUL_POWER_2_CAPACITY = 0.1;  // = 12 / 230.0 * 0.8; // [V / V] * % loss for conversion
    double MAX_CAPACITY_AMPH = 60;
    double MIN_LOAD_AMPH = MAX_CAPACITY_AMPH / 2; //# Assuming a lead-acid
    double DISCHARGE_PER_HOUR_PERCENT = 0.5;

    EnjoLib::Str Print() const;
};

#endif // __BATTERYPARAMS_H
