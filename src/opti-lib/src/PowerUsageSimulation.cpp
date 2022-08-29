#include "PowerUsageSimulation.h"
#include "OptiEnProfitDataModel.h"
#include "ConfigSol.h"

#include <Statistical/Matrix.hpp>

using namespace EnjoLib;

PowerUsageSimulation::~PowerUsageSimulation(){}
PowerUsageSimulation::PowerUsageSimulation(const OptiEnProfitDataModel & dataModel)
: m_dataModel(dataModel)
{}

PowerUsageSimulation::SimResult PowerUsageSimulation::Simulate(int i, int currHour, size_t compSize, const EnjoLib::Matrix & dataMat,
                                                             double bonusMul, bool isInitialLoad) const
{
    SimResult res{};
    //const EnjoLib::Array<Computer> & comps = m_dataModel.GetComputers();
    res.sumPowerUsage += m_dataModel.GetHabitsUsage(i);
    const double bonusMulMA = m_dataModel.GetConf().HASHRATE_BONUS;
    if (isInitialLoad)
    {
        return res;
    }
    const auto & comps = m_dataModel.GetComputers();
    for (int ic = 0; ic < compSize; ++ic)
    {
        const Computer & comp = comps[ic];
        const VecD & inp = dataMat[ic];
        const double val = inp[i];
        const double hashe = comp.GetHashRate(val) * bonusMul;
        if (ic < comp.minRunHours + 1) // Assuming, that the bonus will last at least for the number of computer's running hours
        {
            res.sumHashes += hashe * (1 + bonusMulMA);
        }
        else
        {
            res.sumHashes += hashe;
        }
        res.sumPowerUsage += comp.GetUsage(val);
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
