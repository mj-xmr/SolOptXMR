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
    const Computer & comp0 = OptiTestUtil().GetCompTestSched();
    const ConfigSol cfg;
    const int horizon = 2;
    const int startingPoint = 0;
    const double apmlidude = 30;
    const Str dataFname = cfg.m_outDir + "/" + OptiEnProfitDataModel::SOLAR_POS_FILE;
    {
        Ofstream ofs(dataFname);
    }
    const VecD genPower = SolUtil().GenSolar(horizon, apmlidude);
    std::vector<Habit> habits;
    System sys;
    BatteryParams batPars;

    OptiEnProfitDataModel dataModel(cfg, habits, sys, batPars, horizon, startingPoint);
    dataModel.SetPowerProduction(genPower);
    dataModel.SetComputers(std::vector<Computer>{comp0});
    OptimizerEnProfit opti(dataModel);
    opti();

    //JsonReader().ReadComputers();
    CHECK(opti.GetGoals().size() > 0);
    CHECK(opti.GetGoal() > 0);
}

TEST(EdgeSol_high)
{
    const Computer & comp0 = OptiTestUtil().GetCompTestSched();
    const ConfigSol cfg;
    const int horizon = 2;
    const int startingPoint = 0;
    const double apmlidude = 150;
    const Str dataFname = cfg.m_outDir + "/" + OptiEnProfitDataModel::SOLAR_POS_FILE;
    {
        Ofstream ofs(dataFname);
    }
    const VecD genPower = SolUtil().GenSolar(horizon, apmlidude);
    std::vector<Habit> habits;
    System sys;
    BatteryParams batPars;

    OptiEnProfitDataModel dataModel(cfg, habits, sys, batPars, horizon, startingPoint);
    dataModel.SetPowerProduction(genPower);
    dataModel.SetComputers(std::vector<Computer>{comp0});
    OptimizerEnProfit opti(dataModel);
    opti();

    //JsonReader().ReadComputers();
    CHECK(opti.GetGoals().size() > 0);
    CHECK(opti.GetGoal() > 0);
}
