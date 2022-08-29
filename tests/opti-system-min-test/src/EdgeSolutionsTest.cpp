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

TEST(EdgeSol_happy)
{
    const int horizon = 2;
    const int startingPoint = 0;
    const double amplitude = 30;
    const VecD genPower = SolUtil().GenSolar(horizon, amplitude);

    const OptimizerEnProfit & opti = OptiTestUtil().TestEdgeSolGetOptimizer(genPower, horizon, startingPoint);
    CHECK(opti.GetHashes() > 0);
    CHECK_EQUAL(0, opti.GetPenality());
}

TEST(EdgeSol_high)
{
    const int horizon = 2;
    const int startingPoint = 0;
    const double amplitude = 150;
    const VecD genPower = SolUtil().GenSolar(horizon, amplitude);

    const OptimizerEnProfit & opti = OptiTestUtil().TestEdgeSolGetOptimizer(genPower, horizon, startingPoint);
    CHECK(opti.GetHashes() > 0);
    CHECK(opti.GetPenality() > 0);
}


TEST(EdgeSol_high_midday)
{
    const int horizon = 2;
    const int startingPoint = 12;
    const double amplitude = 150;
    const VecD genPower = SolUtil().GenSolar(horizon + 1, amplitude);

    const OptimizerEnProfit & opti = OptiTestUtil().TestEdgeSolGetOptimizer(genPower, horizon, startingPoint);
    CHECK(opti.GetGoals().size() > 0);
    CHECK(opti.GetHashes() > 0);
    CHECK(opti.GetPenality() > 0);
}


TEST(EdgeSol_high_low_power)
{
    const ConfigSol cfg;
    const int horizon = 2;
    const int startingPoint = 0;
    const double amplitude = 1;
    const VecD genPower = SolUtil().GenSolar(horizon, amplitude);

    const OptimizerEnProfit & opti = OptiTestUtil().TestEdgeSolGetOptimizer(genPower, horizon, startingPoint);
    CHECK(opti.GetGoals().size() > 0);
    CHECK(opti.GetHashes() > 0);
    CHECK_EQUAL(0, opti.GetPenality());
}

TEST(EdgeSol_2computers_short)
{
    const int horizon = 1;
    const int startingPoint = 0;
    const VecD computersHashrateMul = {1, 2};
    const double amplitude = 120;
    const VecD genPower = SolUtil().GenSolar(horizon, amplitude);

    const OptimizerEnProfit & opti = OptiTestUtil().TestEdgeSolGetOptimizer(genPower, horizon, startingPoint, computersHashrateMul);
    CHECK(opti.GetHashes() > 0);
    CHECK_EQUAL(0, opti.GetPenality());
}

TEST(EdgeSol_overcharged)
{
    const int horizon = 2;
    const int startingPoint = 0;
    const VecD computersHashrateMul = {1};
    const double amplitude = 30;
    const double batChargeAh = 100;
    const VecD genPower = SolUtil().GenSolar(horizon, amplitude);

    const OptimizerEnProfit & opti = OptiTestUtil().TestEdgeSolGetOptimizer(genPower, horizon, startingPoint, computersHashrateMul, batChargeAh);
    CHECK(opti.GetHashes() > 0);
    CHECK(opti.GetPenality() > 0); /// TODO: Try not to penalize the initial load
    //CHECK_EQUAL(0, opti.GetPenality());
}
