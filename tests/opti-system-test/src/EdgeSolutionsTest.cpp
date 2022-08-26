#include "OptimizerEnProfit.h" /// TODO: Use a smaller header after the extraction
#include "OptiEnProfitDataModel.h"
#include "ConfigSol.h"
#include "OptiTestUtil.h"
#include "SolUtil.h"
#include "Computer.h"

#include <Ios/Ofstream.hpp>
#include <Util/CoutBuf.hpp>

#include <UnitTest++/UnitTest++.h>
#include <vector>

using namespace EnjoLib;

static OptimizerEnProfit TestEdgeSolGetOptimizer(const VecD & genPower, int horizon, int startingPoint)
{
    const ConfigSol cfg;
    const Computer & comp0 = OptiTestUtil().GetCompTestSched();
    const Str dataFname = cfg.m_outDir + "/" + OptiEnProfitDataModel::SOLAR_POS_FILE;
    {
        Ofstream ofs(dataFname);
    }

    std::vector<Habit> habits;
    System sys;
    BatteryParams batPars;

    OptiEnProfitDataModel dataModel(cfg, habits, sys, batPars, horizon, startingPoint);
    dataModel.SetPowerProduction(genPower);
    dataModel.SetComputers(std::vector<Computer>{comp0});
    OptimizerEnProfit opti(dataModel);
    opti();

    CHECK(opti.GetGoals().size() > 0);

    return opti;
}

TEST(EdgeSol_happy)
{
    const int horizon = 2;
    const int startingPoint = 0;
    const double amplitude = 30;
    const VecD genPower = SolUtil().GenSolar(horizon, amplitude);

    const OptimizerEnProfit & opti = TestEdgeSolGetOptimizer(genPower, horizon, startingPoint);
    CHECK(opti.GetGoal() > 0);
}

TEST(EdgeSol_high)
{
    const Computer & comp0 = OptiTestUtil().GetCompTestSched();
    const ConfigSol cfg;
    const int horizon = 2;
    const int startingPoint = 0;
    const double amplitude = 150;
    const VecD genPower = SolUtil().GenSolar(horizon, amplitude);

    const OptimizerEnProfit & opti = TestEdgeSolGetOptimizer(genPower, horizon, startingPoint);
    CHECK(opti.GetGoal() > OptimizerEnProfit::GOAL_INITIAL);
}


TEST(EdgeSol_high_midday)
{
    const Computer & comp0 = OptiTestUtil().GetCompTestSched();
    const ConfigSol cfg;
    const int horizon = 2;
    const int startingPoint = 12;
    const double amplitude = 150;
    const VecD genPower = SolUtil().GenSolar(horizon + 1, amplitude);

    const OptimizerEnProfit & opti = TestEdgeSolGetOptimizer(genPower, horizon, startingPoint);
    CHECK(opti.GetGoals().size() > 0);
    CHECK(opti.GetGoal() > OptimizerEnProfit::GOAL_INITIAL);
}

