#include "OptiEnProfitSubject.h"
#include "OptiSubjectTSUtil.h"
#include "PredictorFactory.h"
#include "Computer.h"
#include "IPeriod.h"
#include "OptiVarVec.h"
#include "ConfigMan.h"
#include "ConfigOpti.h"
#include "ConfigSol.h"
#include "ConfigDirs.h"
#include "OptiEnProfitDataModel.h"
#include "OptimizerEnProfit.h" /// TODO: Remove
#include "BatteryParams.h"
#include "TimeUtil.h"
#include "SolUtil.h"
#include "BatterySimulation.h"
#include "PowerUsageSimulation.h"

#include "GnuplotIOSWrap.h"

#include <Ios/Ofstream.hpp>
#include <Math/GeneralMath.hpp>
#include <Util/CoutBuf.hpp>
#include <Util/StrColour.hpp>
#include <Template/Array.hpp>
#include <Visual/AsciiPlot.hpp>
#include <Visual/AsciiMisc.hpp>

#include <STD/Vector.hpp>

using namespace EnjoLib;

OptiSubjectEnProfit::OptiSubjectEnProfit(const OptiEnProfitDataModel & dataModel)
    : m_dataModel(dataModel)
    , m_currHour(dataModel.GetCurrHour())
{
}

OptiSubjectEnProfit::~OptiSubjectEnProfit()
{
}

double OptiSubjectEnProfit::Get(const double * inp, int n)
{
    return 0;
}
Solution OptiSubjectEnProfit::GetVerbose(const EnjoLib::Matrix & dataMat, bool verbose, double maxHashes)
{
    //ELO
    Solution sol;
    const int PENALITY_SUM_MUL = 1;
    const GeneralMath gmat;
    const size_t n = dataMat.at(0).size();
    const double matSum = SolUtil().SumMat(dataMat);
    const EnjoLib::Array<Computer> & comps = m_dataModel.GetComputers();
    const bool LOG_UNACCEPTABLE_SOLUTIONS = false;
    const System & sys = m_dataModel.GetSystem();
    const ConfigSol & conf = m_dataModel.GetConf();
    BatterySimulation battery(conf, m_dataModel.GetBatPars(), sys);
    double penalitySum = 0;
    PowerUsageSimulation powSim(m_dataModel);
    PowerUsageSimulation::SimResult simResult{};
    const size_t compSize = m_dataModel.GetComputers().size();
    //Assertions::SizesEqual(compSize, dataMat.size(), "OptiSubjectEnProfit::GetVerbose");
    for (int i = 0; i < n; ++i)
    {
        const double bonusMul = HashrateBonus(i % 24);
        const double powerProd = m_dataModel.GetPowerProduction(i);
        //LOG << "i = " << i << ", val = " << inp[i] << Nl;
        //if (not battery.initial_load)
        //if (false)
        const PowerUsageSimulation::SimResult & resLocal = powSim.Simulate(i, m_currHour, dataMat, bonusMul, battery.initial_load); // TODO: currently the function assumes a hardcoded 1h time window, needs to be configurable
        simResult.Add(resLocal);
        battery.iter_get_load(powerProd, resLocal.sumPowerUsage);
        //const double pentalityUndervolted = load < 0 ? GMat().Fabs(load * load * load) : 0;
        const double pentalityUndervolted = battery.num_undervolted * 10;// * resLocal.sumHashes * 2; //  9.0;
        const double pentalityOvervolted  = battery.num_overvolted;// * compSize;// 100.0;

        if (not sys.buying)
        {
            if (pentalityUndervolted > 0)
            {
                if (battery.initial_load)
                {
                    if (matSum != 0)
                    {
                        //unacceptableSolution = true;
                    }

                }
            }
        }
        if (not sys.selling)
        {
            if (pentalityOvervolted > 0)
            {
                // Should stay acceptable, or it screws up many solutions.
                //unacceptableSolution = true;
            }
        }
        if (i > n-24)
        {
            // last day - don't mine
            // Since the algo considers the last day in the horizon as "the end of world", if typically decides to drain the battery to the minimum at the horizon.
            //penalitySum += resLocal.sumHashes;
        }
        //penalityUnder.Add(pentalityUndervolted);
        penalitySum += pentalityUndervolted;
        penalitySum += pentalityOvervolted;
        //LOGL << "acceptable solution. Penality undervolt = " << pentalityUndervolted << " Overvolt: " << pentalityOvervolted << "\n";
    }
    const double penality = penalitySum * PENALITY_SUM_MUL; /// TODO: Penalize overvoltage differently than undervoltage
    m_penalitySum = penality;
    /// TODO: The undervoltage / overvoltage penality should be non-linear.
    const double positive = simResult.sumHashes;
    if (verbose)
    {
        LOGL << m_sumMax << ", adj = "  << m_penalitySum << Nl;
        m_sumMax = m_penalitySum;

        //if (gcfgMan.cfgOpti->OPTI_VERBOSE && m_isVerbose)
        if (gcfgMan.cfgOpti->OPTI_VERBOSE)
            //if (false)
        {
            if (not gcfgMan.cfgOpti->IsXValid())
            {
                LOGL << SolUtil().GetT() << Nl <<
                ": New goal = " << penality << ", hashes = " << simResult.sumHashes << Nl;

                PowerUsageSimulation::SimResult resVisual{};
                BatterySimulation batteryCopy(conf, m_dataModel.GetBatPars(), m_dataModel.GetSystem());
                VecD hashes, loads, penalityUnder, input, prod, hashrateBonus, usages;
                //Assertions::SizesEqual(m_dataModel.GetComputers().size(), dataMat.size(), "OptiSubjectEnProfit::GetVerbose");
                for (int i = 0; i < n; ++i)
                {
                    const double bonusMul = HashrateBonus(i % 24);
                    const double powerProd = m_dataModel.GetPowerProduction(i);
                    //LOG << "i = " << i << ", val = " << inp[i] << Nl;
                    //if (not battery.initial_load)
                    //if (false)
                    const PowerUsageSimulation::SimResult & resLocal = powSim.Simulate(i, m_currHour, dataMat, bonusMul, batteryCopy.initial_load);
                    resVisual.Add(resLocal);
                    const double load = batteryCopy.iter_get_load(powerProd, resLocal.sumPowerUsage);
                    usages.Add(resLocal.sumPowerUsage * batteryCopy.pars.GetMulPowerToCapacity(m_dataModel.GetSystem().voltage));
                    //input.Add(val);
                    loads.Add(load);
                    prod.Add(powerProd);
                    hashes.Add(resVisual.sumHashes);
                    hashrateBonus.Add(bonusMul);
                }
                m_usages = usages;
                m_input = input;
                m_loads = loads;
                m_prod = prod;
                m_hashes = hashes;
                m_hashrateBonus = hashrateBonus;
                //ToolsMixed().SystemCallWarn("clear", __PRETTY_FUNCTION__);
                OutputVar(hashes, "hashrates");
                OutputVar(usages, "usage", false);
                if (not m_dataModel.GetConf().NO_GNUPLOT)
                {
                    //GnuplotPlotTerminal1d(input, "input", 1, 0.5);
                    GnuplotPlotTerminal1d(prod, "Energy production [W]", 1, 0.5);
                    //GnuplotPlotTerminal1d(hashrateBonus, "Bashrate bonus seasonal", 1, 0.5);
                }

                OutputVar(loads, "battery");

                {
                    using Par = AsciiPlot::Pars;
                    const SolUtil sut;
                    ELO
                    const double maxHashes2display = maxHashes > 0 ? maxHashes : m_hashes.Max();
                    LOG << "Hashes cumul. [H]: (max = " << sut.round(m_hashes.Max(), 1) << ")\n";
                    LOG << AsciiMisc().GenChars("▁", m_hashes.size()) << Nl;
                    LOG << StrColour::GenNorm(StrColour::Col::Magenta, AsciiPlot::Build()(Par::MAXIMUM, maxHashes2display).Finalize().Plot(m_hashes)) << Nl;
                    LOG << "Energy input  [W] : (max = " << sut.round(m_prod.Max(), 1) << ")\n";
                    const float maxSol = std::max(conf.MAX_RAW_SOLAR_INPUT, static_cast<float>(m_prod.Max()));
                    LOG << StrColour::GenNorm(StrColour::Col::Yellow,  AsciiPlot::Build()(Par::MAXIMUM, maxSol).Finalize().Plot(m_prod)) << Nl;
                    LOG << "Bat charge    [Ah]: (max = " << sut.round(m_loads.Max(), 1) << ")\n";
                    LOG << AsciiPlot::Build()(Par::MAXIMUM, batteryCopy.m_maxCapacityAmph)
                    (Par::MINIMUM, m_dataModel.GetBatPars().MIN_LOAD_AMPH)(Par::COLORS, true)
                    (Par::LINE_TRIPLE, true)
                    .Finalize().Plot(m_loads) << Nl;
                    LOG << "Total usage   [A] : (max = " << sut.round(m_usages.Max(), 1) << ")\n";
                    LOG << AsciiPlot::Build()(Par::MAXIMUM, batteryCopy.pars.MAX_DISCHARGE_AMP)
                    (Par::COLORS, true)
                    .Finalize().Plot(m_usages) << Nl;
                    //LOG << SolUtil().GetT() << Nl;
                }
            }
        }
    }
    sol.hashes   = positive;
    sol.penality = m_penalitySum;

    return sol;
    //return sumAdjusted;
    //return -sum;
}

void OptiSubjectEnProfit::OutputVar(const EnjoLib::VecD & data, const EnjoLib::Str & descr, bool plot) const
{
    if (plot && not m_dataModel.GetConf().NO_GNUPLOT)
    {
        GnuplotPlotTerminal1d(data, descr, 1, 0.5);
    }
    Ofstream fout(m_dataModel.GetConf().m_outDir + "/soloptout-" + descr + ".txt");
    fout << data.Print() << Nl;
}

double OptiSubjectEnProfit::GetGoal() const
{
    return m_sumMax;
}

EnjoLib::VecD OptiSubjectEnProfit::GetStart() const
{
    return OptiSubjectTSUtil().GetStartProt(m_optiFloat);
}
EnjoLib::VecD OptiSubjectEnProfit::GetStep() const
{
    return OptiSubjectTSUtil().GetStepProt(m_optiFloat);
}
EnjoLib::Array<EnjoLib::OptiMultiSubject::Bounds> OptiSubjectEnProfit::GetBounds() const
{
    const Matrix mat = m_dataModel.GetData();
    std::vector<EnjoLib::OptiMultiSubject::Bounds> ret;
    for (const VecD & dim : mat)
    {
        EnjoLib::OptiMultiSubject::Bounds bound(dim.First(), dim.Last());
        ret.push_back(bound);
    }
    return ret;
}

double OptiSubjectEnProfit::HashrateBonusNonCached(int hour) const
{
    /// TODO: This is meant to be dynamically read from tsqsim
    if (hour > 10 && hour < 16)
    {
        return 0.99;
    }
    else if (hour > 18)
    {
        return 1.01;
    }
    else
    {
        return 1;
    }
}

double OptiSubjectEnProfit::HashrateBonus(int hour) const
{
    static VecD cache(24);

    const double val = cache.at(hour);

    if (val != 0)
    {
        return val;
    }
    cache.at(hour) = HashrateBonusNonCached(hour);
    return cache.at(hour);
}
