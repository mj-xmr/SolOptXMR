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
#include "BatteryParams.h"

#include "GnuplotIOSWrap.h"

#include <Ios/Ofstream.hpp>
#include <Math/GeneralMath.hpp>
#include <Util/CoutBuf.hpp>
#include <Util/ToolsMixed.hpp>
#include <Template/Array.hpp>

using namespace EnjoLib;

OptiSubjectEnProfit::OptiSubjectEnProfit(const OptiEnProfitDataModel & dataModel)
    : m_dataModel(dataModel)
{
}

OptiSubjectEnProfit::~OptiSubjectEnProfit()
{
}

struct BatterySimulation
{
    BatterySimulation(const ConfigSol & confSol, const BatteryParams & batPars, const System & sys);
    const BatteryParams & pars;
    const System & m_sys;
    //state:
    //double load = MIN_LOAD * 0.95;
    double load = 0;
    static constexpr double T_DELTA_HOURS = 1;

    double initial_load = true;
    double num_undervolted = 0;
    double num_undervolted_initial = 0;
    double num_overvolted = 0;
    double num_overused = 0;

    double iter_get_load(double inp, double out, double hours=T_DELTA_HOURS);
};

BatterySimulation::BatterySimulation(const ConfigSol & confSol, const BatteryParams & batPars, const System & sys)
: pars(batPars)
, m_sys(sys)
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
    if (initial_load)
    {
        //out = 0; // dangerous
    }
    double discharge = hours * pars.DISCHARGE_PER_HOUR_PERCENT / 100.0 * load;
    if (inp < 0)
    {
        inp = 0;
    }
    double balance = inp - out - discharge;
    double change = balance * pars.GetMulPowerToCapacity(m_sys.voltage);
    if (change > pars.MAX_DISCHARGE_AMP)
    {
        //if out > pars.MAX_CAPACITY_AMPH: # A valid possibility
        num_overused += 1;
        change = pars.MAX_DISCHARGE_AMP;
    }
    //#print(change)
    load += change;
    //LOGL << "Cap " <<  pars.MAX_CAPACITY_AMPH << Nl;
    if (load > pars.MAX_CAPACITY_AMPH)
    {
        load = pars.MAX_CAPACITY_AMPH;
        num_overvolted += 1;
    }
    if (load < pars.MIN_LOAD_AMPH)
    {
        //if (initial_load)
        //  num_undervolted_initial += 1;
        //else
        num_undervolted += 1;

    }
    //if (load < 0)
    //  load = 0;

    if (initial_load)
        if (load > pars.MIN_LOAD_AMPH)
            initial_load = false;

    return load;

}

double OptiSubjectEnProfit::Get(const double * inp, int n)
{
    return 0;
}
double OptiSubjectEnProfit::GetVerbose(const EnjoLib::Matrix & dataMat, bool verbose)
{
    //ELO
    const size_t n = dataMat.at(0).size();
    const EnjoLib::Array<Computer> & comps = m_dataModel.GetComputers();

    BatterySimulation battery(m_dataModel.GetConf(), m_dataModel.GetBatPars(), m_dataModel.GetSystem());
    double penalitySum = 0;

    SimResult simResult{};
    for (int i = 0; i < n; ++i)
    {
        const double bonusMul = HashrateBonus(i % 24);
        //LOG << "i = " << i << ", val = " << inp[i] << Nl;
        //if (not battery.initial_load)
        //if (false)
        const SimResult & resLocal = Simulate(i, dataMat, bonusMul, m_dataModel.GetConf().HASHRATE_BONUS);
        simResult.Add(resLocal);
        const double load = battery.iter_get_load(m_dataModel.GetPowerProduction(i), resLocal.sumPowerUsage);
        //const double pentalityUndervolted = load < 0 ? GMat().Fabs(load * load * load) : 0;
        const double pentalityUndervolted = battery.num_undervolted;
        const double pentalityOvervolted = battery.num_overvolted;
        //penalityUnder.Add(pentalityUndervolted);
        penalitySum += pentalityUndervolted;
        penalitySum += pentalityOvervolted;
    }
    //const double pentalityUndervolted = m_battery.num_undervolted * m_battery.num_undervolted;
    //const double pentalityUndervolted = penalitySum * 10000;
    //const double pentalityOvervolted = battery.num_overvolted;
    const double penality = penalitySum * 10000; /// TODO: Penalize overvoltage differently than undervoltage
    /// TODO: The undervoltage / overvoltage penality should be non-linear.
    const double positive = simResult.sumHashes;
    double sumAdjusted = positive - penality;
    if (penality > 0 && sumAdjusted > 0)
    {
        sumAdjusted -= positive;
    }

    //LOGL << sum << ", adj = "  << sumAdjusted << Endl;

    //if (GMat().round(sumAdjusted) > GMat().round(m_sumMax) || m_sumMax == 0)
    if (verbose)
    {
        //LOGL << sum << ", adj = "  << sumAdjusted << Endl;
        m_sumMax = sumAdjusted;

        //if (gcfgMan.cfgOpti->OPTI_VERBOSE && m_isVerbose)
        if (gcfgMan.cfgOpti->OPTI_VERBOSE)
            //if (false)
        {
            if (not gcfgMan.cfgOpti->IsXValid())
            {
                LOGL << ": New goal = " << sumAdjusted << ", m_sumMax = " << m_sumMax << ", penality = " << penality << ", after " << 0 << " iterations\n";

                SimResult resVisual{};
                BatterySimulation batteryCopy(m_dataModel.GetConf(), m_dataModel.GetBatPars(), m_dataModel.GetSystem());
                VecD hashes, loads, penalityUnder, input, prod, hashrateBonus, usages;
                for (int i = 0; i < n; ++i)
                {
                    const double bonusMul = HashrateBonus(i % 24);
                    //LOG << "i = " << i << ", val = " << inp[i] << Nl;
                    //if (not battery.initial_load)
                    //if (false)
                    const SimResult & resLocal = Simulate(i, dataMat, bonusMul, m_dataModel.GetConf().HASHRATE_BONUS);
                    resVisual.Add(resLocal);
                    const double load = batteryCopy.iter_get_load(m_dataModel.GetPowerProduction(i), resLocal.sumPowerUsage);
                    usages.Add(resLocal.sumPowerUsage * batteryCopy.pars.GetMulPowerToCapacity(m_dataModel.GetSystem().voltage));
                    //input.Add(val);
                    loads.Add(load);
                    prod.Add(m_dataModel.GetPowerProduction(i));
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
                OutputVar(loads, "battery");
                OutputVar(usages, "usage", false);

                //GnuplotPlotTerminal1d(input, "input", 1, 0.5);
                GnuplotPlotTerminal1d(prod, "prod", 1, 0.5);
                GnuplotPlotTerminal1d(hashrateBonus, "hashrateBonus", 1, 0.5);
            }
        }
    }

    return sumAdjusted;
    //return -sum;
}

OptiSubjectEnProfit::SimResult OptiSubjectEnProfit::Simulate(int i, const EnjoLib::Matrix & dataMat, double bonusMul, double bonusMulMA) const
{
    SimResult res{};

    const EnjoLib::Array<Computer> & comps = m_dataModel.GetComputers();
    for (int ic = 0; ic < comps.size(); ++ic)
    {
        const Computer & comp = comps.at(ic);
        const VecD & inp = dataMat.at(ic);
        const double val = inp[i];
        const double hashe = comp.GetHashRate(val) * bonusMul;
        if (ic < comp.minRunHours + 1)
        {
            res.sumHashes += hashe * (1 + bonusMulMA);
        }
        else
        {
            res.sumHashes += hashe;
        }
        res.sumPowerUsage += comp.GetUsage(val);
    }
    return res;
}

void OptiSubjectEnProfit::OutputVar(const EnjoLib::VecD & data, const EnjoLib::Str & descr, bool plot) const
{
    if (plot)
    {
        GnuplotPlotTerminal1d(data, descr, 1, 0.5);
    }
    Ofstream fout("/tmp/soloptout-" + descr + ".txt");
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
    EnjoLib::Array<EnjoLib::OptiMultiSubject::Bounds> ret;
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
        return 0.97;
    }
    else if (hour > 18)
    {
        return 1.03;
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
