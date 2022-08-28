#include "OptiTestUtil.h"
#include "Computer.h"
#include "ConfigSol.h"
#include "OptimizerEnProfit.h"
#include "OptiEnProfitDataModel.h"

#include <Ios/Ofstream.hpp>
#include <UnitTest++/UnitTest++.h>
#include <vector>

using namespace EnjoLib;

OptiTestUtil::OptiTestUtil(){}
OptiTestUtil::~OptiTestUtil(){}

Str OptiTestUtil::compSched_macAddr = "DE:AD:BE:EF:AA:BB";
Str OptiTestUtil::compSched_hostname = "Miner_2049er";

Computer OptiTestUtil::GetCompTestSched() const
{
    Computer compTest;
    compTest.macAddr = compSched_macAddr;
    compTest.hostname = compSched_hostname;

    return compTest;
}

OptimizerEnProfit OptiTestUtil::TestEdgeSolGetOptimizer(const VecD & genPower, int horizon, int startingPoint, int num_computers) const
{
    ConfigSol cfg;
    cfg.RANDOM_SEED = 1;
    cfg.NO_GNUPLOT = true;
    const Computer & comp0 = GetCompTestSched();
    const Str dataFname = cfg.m_outDir + "/" + OptiEnProfitDataModel::SOLAR_POS_FILE;
    {
        Ofstream ofs(dataFname);
    }

    std::vector<Habit> habits;
    System sys;
    BatteryParams batPars;

    OptiEnProfitDataModel dataModel(cfg, habits, sys, batPars, horizon, startingPoint);
    dataModel.SetPowerProduction(genPower);
    std::vector<Computer> comps;
    for (int i = 0; i < num_computers; ++i)
    {
        comps.push_back(comp0);
    }
    dataModel.SetComputers(comps);
    OptimizerEnProfit opti(dataModel);
    opti();

    CHECK(opti.GetGoals().size() > 0);

    return opti;
}
