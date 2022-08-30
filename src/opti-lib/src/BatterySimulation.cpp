#include "BatterySimulation.h"
#include "BatteryParams.h"
#include "System.h"

#include "ConfigSol.h"

#include <Statistical/Assertions.hpp>

using namespace EnjoLib;

BatterySimulation::BatterySimulation(const ConfigSol & confSol, const BatteryParams & batPars, const System & sys)
: pars(batPars)
, m_sys(sys)
, m_mulPowerToCapacity(pars.GetMulPowerToCapacity(sys.voltage))
, m_dischargePerHour(pars.DISCHARGE_PER_HOUR_PERCENT / 100.0)
, m_maxCapacityAmph(pars.MAX_CAPACITY_AMPH * (confSol.BATTERY_CHARGE_MAX_PERCENTAGE > 0 ? confSol.BATTERY_CHARGE_MAX_PERCENTAGE : 1))
{
    if (confSol.BATTERY_CHARGE > 0)
    {
        load = confSol.BATTERY_CHARGE; /// TODO limit with sanity checks
    }
    else
    {
        load = pars.MIN_LOAD_AMPH * 1.1; /// TODO: Read the parameter from user input, later from the measurements
    }
}

double BatterySimulation::iter_get_load(double inp, double out, double hours)
{
    //initial_load = false;
    const double discharge = hours * m_dischargePerHour * load;
    if (inp < 0)
    {
        Assertions::Throw("input < 0", "BatterySimulation::iter_get_load");
        //inp = 0;
    }
    const double balance = inp - out - discharge;
    double change = balance * m_mulPowerToCapacity;
    if (change > pars.MAX_DISCHARGE_AMP)
    {
        //if out > m_maxCapacityAmph: # A valid possibility
        ++num_overused;
        //change = pars.MAX_DISCHARGE_AMP;
    }
    //#print(change)
    load += change;
    //LOGL << "Cap " <<  m_maxCapacityAmph << Nl;
    if (load > m_maxCapacityAmph)
    {
        if (initial_load)
        {
            ++num_overvolted_initial; // this should be only informational
        }
        const double diff = load - m_maxCapacityAmph;
        //num_overvolted += 1 + diff;
        //num_overvolted += 1 + diff * 2;
        num_overvolted += 1 + diff;
        //num_overvolted += GMat().Pow(1 + diff, 1.01);
    }
    if (load < pars.MIN_LOAD_AMPH)
    {
        const double diff = pars.MIN_LOAD_AMPH - load;
        num_undervolted += 1 + diff;
    }
    //if (load < 0)
    //  load = 0;

    if (initial_load)
        if (load > pars.MIN_LOAD_AMPH)
        {
            /// TODO: Unit test this, as lack of this should cause a crash
            //LOGL << "Initial load done.\n";
            initial_load = false;
        }
        else
        {
            //LOGL << "Still loading.\n";
        }


    return load;
}
