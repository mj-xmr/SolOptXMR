#ifndef __BATTERYSIMULATION_H
#define __BATTERYSIMULATION_H

class ConfigSol;
class BatteryParams;
class System;

struct BatterySimulation
{
    BatterySimulation(const ConfigSol & confSol, const BatteryParams & batPars, const System & sys);
    const BatteryParams & pars;
    const System & m_sys;
    //state:
    //double load = MIN_LOAD * 0.95;
    double load = 0;
    static constexpr double T_DELTA_HOURS = 1;

    double initial_load = true;
    double num_undervolted = 0;
    double num_undervolted_initial = 0;
    double num_overvolted = 0;
    double num_overvolted_initial = 0;
    double num_overused = 0;
    double m_mulPowerToCapacity = 0;
    double m_dischargePerHour = 0;
    double m_maxCapacityAmph = 0;

    double iter_get_load(double inp, double out, double hours=T_DELTA_HOURS);
};

#endif // __BATTERYSIMULATION_H
