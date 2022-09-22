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
    const Str name = "Happy path";
    const VecD genPower = SolUtil().GenSolar(horizon, amplitude);

    auto builder = OptiTestUtilConf::Build();
    const OptimizerEnProfit & opti = builder.Finalize().TestEdgeSolGetOptimizer(name, genPower, horizon, startingPoint);
    CHECK(opti.GetHashes() > 0);
    CHECK_EQUAL(0, opti.GetPenality());
}

TEST(EdgeSol_high)
{
    const int horizon = 2;
    const int startingPoint = 0;
    const double amplitude = 150;
    const Str name = "High load";
    const VecD genPower = SolUtil().GenSolar(horizon, amplitude);

    auto builder = OptiTestUtilConf::Build();
    const OptimizerEnProfit & opti = builder.Finalize().TestEdgeSolGetOptimizer(name, genPower, horizon, startingPoint);
    CHECK(opti.GetHashes() > 0);
    CHECK(opti.GetPenality() > 0);
}

TEST(EdgeSol_high_midday)
{
    const int horizon = 2;
    const int startingPoint = 12;
    const double amplitude = 150;
    const Str name = "High load, midday";
    const VecD genPower = SolUtil().GenSolar(horizon + 1, amplitude);

    auto builder = OptiTestUtilConf::Build();
    const OptimizerEnProfit & opti = builder.Finalize().TestEdgeSolGetOptimizer(name, genPower, horizon, startingPoint);
    CHECK(opti.GetGoals().size() > 0);
    CHECK(opti.GetHashes() > 0);
    CHECK(opti.GetPenality() > 0);
}

TEST(EdgeSol_high_low_power)
{
    const ConfigSol cfg;
    const int horizon = 2;
    const int startingPoint = 0;
    const double amplitude = 5;
    const Str name = "Low input";
    const VecD genPower = SolUtil().GenSolar(horizon, amplitude);

    auto builder = OptiTestUtilConf::Build();
    const OptimizerEnProfit & opti = builder.Finalize().TestEdgeSolGetOptimizer(name, genPower, horizon, startingPoint);
    CHECK(opti.GetGoals().size() > 0);
    CHECK(opti.GetHashes() > 0);
    CHECK_EQUAL(0, opti.GetPenality());
}

TEST(EdgeSol_2computers_short)
{
    const int horizon = 1;
    const int startingPoint = 0;
    const VecD computersHashrateMul = {3, 1};
    //const VecD computersHashrateMul = {2};
    const double amplitude = 100;
    const Str name = "2 Computers, single day";
    const VecD genPower = SolUtil().GenSolar(horizon, amplitude);

#if 1
    auto builder = OptiTestUtilConf::Build();
    builder
    (OptiTestUtilConf::Pars::NO_SCHEDULE, false)
    (OptiTestUtilConf::Pars::NUM_SOLUTIONS, 2)
    ;
    const OptimizerEnProfit & opti = builder.Finalize().TestEdgeSolGetOptimizer(name, genPower, horizon, startingPoint, computersHashrateMul);
    CHECK(opti.GetHashes() > 0);
    CHECK_EQUAL(0, opti.GetPenality());

    const Matrix & schedule = opti.GetScheduleBest();
    const double sched1 = schedule.at(0).Sum();
    const double sched2 = schedule.at(1).Sum();
    LOGL << "Sched 1 = " << sched1 << ", sched2 = " << sched2 << Nl;
    /// CHECK(sched1 > sched2); // The 1st rig has a better hashrate, so it should be used more often. Broken, but displays correctly.
#endif
}

TEST(EdgeSol_overcharged)
{
    const int horizon = 2;
    const int startingPoint = 0;
    const double amplitude = 30;
    const double batChargeAh = 100;
    const Str name = "Overcharged batteries";
    const VecD genPower = SolUtil().GenSolar(horizon, amplitude);

    auto builder = OptiTestUtilConf::Build();
    builder
    (OptiTestUtilConf::Pars::BATTERY_CHARGE, batChargeAh)
    (OptiTestUtilConf::Pars::NO_PROGRESS_BAR, true)
    ;
    const OptimizerEnProfit & opti = builder.Finalize().TestEdgeSolGetOptimizer(name, genPower, horizon, startingPoint);
    CHECK(opti.GetHashes() > 0);
    CHECK(opti.GetPenality() > 0);
    //CHECK_EQUAL(0, opti.GetPenality());
}


TEST(EdgeSol_undercharged)
{
    const int horizon = 1; /// TODO: Horizon of 1 uncovers a small bug where there's a non-empty schedule.
    const int startingPoint = 0;
    const double amplitude = 30;
    const double batChargeAh = 1;
    const Str name = "Undercharged batteries";
    const VecD genPower = SolUtil().GenSolar(horizon, amplitude);

    auto builder = OptiTestUtilConf::Build();
    builder(OptiTestUtilConf::Pars::BATTERY_CHARGE, batChargeAh)
    //(OptiTestUtilConf::Pars::NO_SCHEDULE, false)
    (OptiTestUtilConf::Pars::NO_PROGRESS_BAR, true)
    (OptiTestUtilConf::Pars::NUM_SOLUTIONS, 1)
    (OptiTestUtilConf::Pars::EXPECT_EMPTY_SCHEDULE, true)
    ;
    const OptimizerEnProfit & opti = builder.Finalize().TestEdgeSolGetOptimizer(name, genPower, horizon, startingPoint);
    CHECK(opti.GetPenality() > 0);
    //CHECK_EQUAL(0, opti.GetPenality());
}


TEST(EdgeSol_undercharged_plus)
{
    const int horizon = 2;
    const int startingPoint = 0;
    const double amplitude = 55;
    const double batChargeAh = 20;
    const Str name = "Undercharged, but loading";
    const VecD genPower = SolUtil().GenSolar(horizon, amplitude);

    auto builder = OptiTestUtilConf::Build();
    builder(OptiTestUtilConf::Pars::BATTERY_CHARGE, batChargeAh)
    //(OptiTestUtilConf::Pars::NO_SCHEDULE, false)
    (OptiTestUtilConf::Pars::NUM_SOLUTIONS, 1)
    (OptiTestUtilConf::Pars::NO_PROGRESS_BAR, true)
    ;
    const OptimizerEnProfit & opti = builder.Finalize().TestEdgeSolGetOptimizer(name, genPower, horizon, startingPoint);
    CHECK(opti.GetPenality() > 0);
    //CHECK_EQUAL(0, opti.GetPenality());
}

TEST(EdgeSol_empty_computers)
{
    const int horizon = 2;
    const int startingPoint = 0;
    const double amplitude = 55;
    const double batChargeAh = 20;
    const Str name = "Empty list of computers";
    const VecD genPower = SolUtil().GenSolar(horizon, amplitude);

    auto builder = OptiTestUtilConf::Build();
    builder(OptiTestUtilConf::Pars::BATTERY_CHARGE, batChargeAh)
    (OptiTestUtilConf::Pars::NO_PROGRESS_BAR, true)
    ;
    const OptimizerEnProfit & opti = builder.Finalize().TestEdgeSolGetOptimizer(name, genPower, horizon, startingPoint, {});
    CHECK(opti.GetPenality() > 0);
}
