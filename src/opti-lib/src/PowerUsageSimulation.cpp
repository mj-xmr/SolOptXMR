#include "PowerUsageSimulation.h"
#include "OptiEnProfitDataModel.h"
#include "ConfigSol.h"

#include <Statistical/Matrix.hpp>

using namespace EnjoLib;

PowerUsageSimulation::~PowerUsageSimulation(){}
PowerUsageSimulation::PowerUsageSimulation(const OptiEnProfitDataModel & dataModel)
: m_dataModel(dataModel)
{}

PowerUsageSimulation::SimResult PowerUsageSimulation::Simulate(int i, int currHour, const EnjoLib::Matrix & dataMat,
                                                             double bonusMul, bool isInitialLoad, double hours) const
{
    SimResult res{};
    //const EnjoLib::Array<Computer> & comps = m_dataModel.GetComputers();
    res.sumPowerUsage += m_dataModel.GetHabitsUsage(i);
    const double bonusMulMA = m_dataModel.GetConf().HASHRATE_BONUS;

    const auto & comps = m_dataModel.GetComputers();
    const size_t compSize = comps.size();
    for (int ic = 0; ic < compSize; ++ic)
    {
        const Computer & comp = comps[ic];
        const VecD & inp = dataMat[ic];
        const double val = inp[i];
        const double hashe = comp.GetHashRate(val) * bonusMul;
        if (ic < comp.minRunHours + 1) // Assuming, that the bonus will last at least for the number of computer's running hours
        {
            res.sumHashes += hashe * (1 + bonusMulMA) * hours * 3600; // [H] = [H/s] * [adim] * [h] * 3600 [s/h]
        }
        else
        {
            res.sumHashes += hashe * hours * 3600; // [H] = [H/s] * 3600 [s/h]
        }
        res.sumPowerUsage += comp.GetUsage(val);
    }
    if (isInitialLoad)
    {
        //res.sumHashes = 0; // Penalize mining during the initial load
    }

    /*
    const EnjoLib::Array<Habit> & habits = m_dataModel.GetHabits();
    //for (const Habit & hab : habits)
    for (int ih = 0; ih < habits.size(); ++ih)
    {
        const Habit & hab = habits[ih];
        double usage = hab.watt_asleep;
        if (hab.IsOn(i))
        {
            usage = hab.watt;
        }
        res.sumPowerUsage += usage;
    }
    */
    return res;
}
