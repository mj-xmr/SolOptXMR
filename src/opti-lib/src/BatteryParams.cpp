#include "BatteryParams.h"

#include <Statistical/Assertions.hpp>

//const double BatteryParams::MUL_POWER_2_CAPACITY = 0.08;  // = 12 / 230.0 * 0.8; // [V / V] * % loss for conversion

BatteryParams::BatteryParams(){}
BatteryParams::~BatteryParams(){}


EnjoLib::Str BatteryParams::Print() const
{
    return ""; /// TODO
}

double BatteryParams::GetMulPowerToCapacity(double voltage) const
{
    if (voltage <= 0)
    {
        EnjoLib::Assertions::Throw("Voltage <= 0", "BatteryParams::GetMulPowerToCapacity");
    }
    return 1 / voltage;
}
