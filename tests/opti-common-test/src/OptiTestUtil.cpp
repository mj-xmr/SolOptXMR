#include "OptiTestUtil.h"
#include "Computer.h"
#include "ConfigSol.h"
#include "SolUtil.h"
#include "OptimizerEnProfit.h"
#include "OptiEnProfitDataModel.h"

#include <Ios/Ofstream.hpp>
#include <Util/StrColour.hpp>
#include <Visual/AsciiMisc.hpp>
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

OptiTestUtilConf::~OptiTestUtilConf(){}
OptiTestUtilConf::OptiTestUtilConf(){}
OptiTestUtilConf::OptiTestUtilConf(const BuilderT & builder)
: m_pars(static_cast<int>(Pars::END))
{
    Add(Pars::NO_GNUPLOT, true);
    Add(Pars::NO_SCHEDULE, true);
    Add(Pars::NO_NEW_SOLUTIONS, true);
    Add(Pars::NUM_SOLUTIONS, 2);
}

OptimizerEnProfit OptiTestUtilConf::TestEdgeSolGetOptimizer(const Str & name, const VecD & genPower, int horizon, int startingPoint,
                                                        const EnjoLib::VecD & compHashMultpliers) const
{
    {
        ELO
        LOG << Nl << AsciiMisc().GenChars("=", 20) << Nl;
        LOG << "Test ID: <== " << StrColour().GenNorm(StrColour::Col::Magenta, name) << " ==> " << Nl;
        LOG << "Simulating: powerMax = " << genPower.Max() << ", horizon = " << horizon << ", compHashMultpliers = " << compHashMultpliers.Print() << Nl;
    }
    ConfigSol cfg;
    cfg.RANDOM_SEED = 1;
    cfg.NO_PROGRESS_BAR = Get(Pars::NO_PROGRESS_BAR);
    cfg.NO_GNUPLOT = Get(Pars::NO_GNUPLOT);
    cfg.NO_SCHEDULE = Get(Pars::NO_SCHEDULE);
    cfg.NO_NEW_SOLUTIONS = Get(Pars::NO_NEW_SOLUTIONS);
    cfg.BATTERY_CHARGE = Get(Pars::BATTERY_CHARGE);
    cfg.NUM_SOLUTIONS = Get(Pars::NUM_SOLUTIONS);
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
    std::vector<Computer> comps;
    for (int i = 0; i < compHashMultpliers.size(); ++i)
    {
        Computer comp = comp0;
        comp.hashPerCore *= compHashMultpliers.at(i);
        comps.push_back(comp);
    }
    dataModel.SetComputers(comps);
    OptimizerEnProfit opti(dataModel);
    opti();

    CHECK(opti.GetGoals().size() > 0);

    const double sum = SolUtil().SumMat(opti.GetScheduleBest());
    if (Get(Pars::EXPECT_EMPTY_SCHEDULE))
    {
        CHECK_EQUAL(0, sum);
    }
    else
    {
        CHECK(sum > 0);
    }
    return opti;
}

void OptiTestUtilConf::Add(const Pars & key, double val)
{
    m_pars.at(static_cast<int>(key)) = val;
}

double OptiTestUtilConf::Get(const Pars & key) const
{
    return m_pars.at(static_cast<int>(key));
}
