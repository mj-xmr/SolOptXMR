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
#include "TimeUtil.h"
#include "SolUtil.h"

#include "GnuplotIOSWrap.h"

#include <Ios/Ofstream.hpp>
#include <Math/GeneralMath.hpp>
#include <Util/CoutBuf.hpp>
#include <Util/StrColour.hpp>
#include <Template/Array.hpp>
#include <Visual/AsciiPlot.hpp>

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
    double num_overvolted_initial = 0;
    double num_overused = 0;
    double m_mulPowerToCapacity = 0;
    double m_dischargePerHour = 0;
    double m_maxCapacityAmph = 0;

    double iter_get_load(double inp, double out, double hours=T_DELTA_HOURS);
};

BatterySimulation::BatterySimulation(const ConfigSol & confSol, const BatteryParams & batPars, const System & sys)
: pars(batPars)
, m_sys(sys)
, m_mulPowerToCapacity(pars.GetMulPowerToCapacity(sys.voltage))
, m_dischargePerHour(pars.DISCHARGE_PER_HOUR_PERCENT / 100.0)
, m_maxCapacityAmph(pars.MAX_CAPACITY_AMPH * (confSol.BATTERY_CHARGE_MAX_PERCENTAGE > 0 ? confSol.BATTERY_CHARGE_MAX_PERCENTAGE : 1))
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
    const double discharge = hours * m_dischargePerHour * load;
    if (inp < 0)
    {
        inp = 0;
    }
    const double balance = inp - out - discharge;
    double change = balance * m_mulPowerToCapacity;
    if (change > pars.MAX_DISCHARGE_AMP)
    {
        //if out > m_maxCapacityAmph: # A valid possibility
        ++num_overused;
        change = pars.MAX_DISCHARGE_AMP;
    }
    //#print(change)
    load += change;
    //LOGL << "Cap " <<  m_maxCapacityAmph << Nl;
    if (load > m_maxCapacityAmph)
    {
        //load = m_maxCapacityAmph; /// TODO: This is quite wrong to assume this.
        if (initial_load)
            ++num_overvolted_initial; /// TODO: Unit test this, as lack of this should cause a crash
        else
            ++num_overvolted;
    }
    if (load < pars.MIN_LOAD_AMPH)
    {
        if (initial_load)
            ++num_undervolted_initial;
        else
            ++num_undervolted;

    }
    //if (load < 0)
    //  load = 0;

    if (initial_load)
        if (pars.MIN_LOAD_AMPH < load && load < m_maxCapacityAmph)
        //if (load > pars.MIN_LOAD_AMPH)
        {
            /// TODO: Unit test this, as lack of this should cause a crash
            //LOGL << "Initial load done.\n";
            initial_load = false;
        }
        else
        {
            //LOGL << "Still loading.\n";
        }


    return load;
}

double OptiSubjectEnProfit::Get(const double * inp, int n)
{
    return 0;
}
double OptiSubjectEnProfit::GetVerbose(const EnjoLib::Matrix & dataMat, bool verbose)
{
    //ELO
    const int PENALITY_SUM_MUL = 10000;
    const size_t n = dataMat.at(0).size();
    const EnjoLib::Array<Computer> & comps = m_dataModel.GetComputers();
    const bool LOG_UNACCEPTABLE_SOLUTIONS = false;
    const System & sys = m_dataModel.GetSystem();
    BatterySimulation battery(m_dataModel.GetConf(), m_dataModel.GetBatPars(), sys);
    double penalitySum = 0;
    SimResult simResult{};
    const size_t compSize = m_dataModel.GetComputers().size();
    Assertions::SizesEqual(compSize, dataMat.size(), "OptiSubjectEnProfit::GetVerbose");
    bool unacceptableSolution = false;
    for (int i = 0; i < n; ++i)
    {
        const double bonusMul = HashrateBonus(i % 24);
        const double powerProd = m_dataModel.GetPowerProduction(i);
        //LOG << "i = " << i << ", val = " << inp[i] << Nl;
        //if (not battery.initial_load)
        //if (false)
        const SimResult & resLocal = Simulate(i, m_currHour, compSize, dataMat, bonusMul, m_dataModel.GetConf().HASHRATE_BONUS, battery.initial_load);
        simResult.Add(resLocal);
        const double load = battery.iter_get_load(powerProd, resLocal.sumPowerUsage);
        //const double pentalityUndervolted = load < 0 ? GMat().Fabs(load * load * load) : 0;
        const double pentalityUndervolted = battery.num_undervolted;
        const double pentalityOvervolted  = battery.num_overvolted;

        if (not sys.buying)
        {
            if (pentalityUndervolted > 0)
            {
                if (not battery.initial_load)
                {
                    unacceptableSolution = true;
                }
            }
        }
        if (not sys.selling)
        {
            if (pentalityOvervolted > 0)
            {
                unacceptableSolution = true;
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

        if (unacceptableSolution)
        {
            if  (LOG_UNACCEPTABLE_SOLUTIONS)
            {
                LOGL << "Unacceptable solution. Penality undervolt = " << pentalityUndervolted << " Overvolt: " << pentalityOvervolted
                 << ", hashes = " << resLocal.sumHashes << "\n";
            }
            const double penality = penalitySum * PENALITY_SUM_MUL;
            const double penalityExtrapolated = penality * (n - i) * 3; // Extrapolate across the remaining simulation steps
            if (not verbose)
            {
                return resLocal.sumHashes -penalityExtrapolated;
            }
        }
        //LOGL << "acceptable solution. Penality undervolt = " << pentalityUndervolted << " Overvolt: " << pentalityOvervolted << "\n";
    }

    //const double pentalityUndervolted = m_battery.num_undervolted * m_battery.num_undervolted;
    //const double pentalityUndervolted = penalitySum * PENALITY_SUM_MUL;
    //const double pentalityOvervolted = battery.num_overvolted;
    const double penality = penalitySum * PENALITY_SUM_MUL; /// TODO: Penalize overvoltage differently than undervoltage
    /// TODO: The undervoltage / overvoltage penality should be non-linear.
    const double positive = simResult.sumHashes;
    double sumAdjusted = positive - penality;
    if (penality > 0 && sumAdjusted > 0)
    {
        sumAdjusted -= positive; /// TODO: This looks like a mistake
    }
    //LOGL << "acceptable solution. Penality sum = " << penalitySum << " positive: " << positive << "\n";
    //LOGL << sum << ", adj = "  << sumAdjusted << Endl;

    //if (GMat().round(sumAdjusted) > GMat().round(m_sumMax) || m_sumMax == 0)
    if (not verbose)
    {
        if (unacceptableSolution)
        {
            Assertions::Throw("Logic error: unacceptableSolution went through", "GetVerbose");
        }
    }
    else
    {
        LOGL << m_sumMax << ", adj = "  << sumAdjusted << Endl;
        m_sumMax = sumAdjusted;

        //if (gcfgMan.cfgOpti->OPTI_VERBOSE && m_isVerbose)
        if (gcfgMan.cfgOpti->OPTI_VERBOSE)
            //if (false)
        {
            if (not gcfgMan.cfgOpti->IsXValid())
            {
                LOGL << SolUtil().GetT() << Nl <<
                ": New goal = " << sumAdjusted << ", m_sumMax = " << m_sumMax << ", penality = " << penality << ", after " << 0 << " iterations\n";

                SimResult resVisual{};
                BatterySimulation batteryCopy(m_dataModel.GetConf(), m_dataModel.GetBatPars(), m_dataModel.GetSystem());
                VecD hashes, loads, penalityUnder, input, prod, hashrateBonus, usages;
                Assertions::SizesEqual(m_dataModel.GetComputers().size(), dataMat.size(), "OptiSubjectEnProfit::GetVerbose");
                for (int i = 0; i < n; ++i)
                {
                    const double bonusMul = HashrateBonus(i % 24);
                    const double powerProd = m_dataModel.GetPowerProduction(i);
                    //LOG << "i = " << i << ", val = " << inp[i] << Nl;
                    //if (not battery.initial_load)
                    //if (false)
                    const SimResult & resLocal = Simulate(i, m_currHour, compSize, dataMat, bonusMul, m_dataModel.GetConf().HASHRATE_BONUS, batteryCopy.initial_load);
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

                //GnuplotPlotTerminal1d(input, "input", 1, 0.5);
                GnuplotPlotTerminal1d(prod, "Energy production", 1, 0.5);
                //GnuplotPlotTerminal1d(hashrateBonus, "Bashrate bonus seasonal", 1, 0.5);
                OutputVar(loads, "battery");

                {
                    using Par = AsciiPlot::Pars;
                    ELO
                    LOG << "Hashes cumul. [Hh]:\n" << StrColour::GenNorm(StrColour::Col::Magenta, AsciiPlot::Build()(Par::MAXIMUM, m_hashes.Max()).Finalize().Plot(m_hashes)) << Nl;
                    LOG << "Energy input  [A] :\n" << StrColour::GenNorm(StrColour::Col::Yellow,  AsciiPlot::Build()(Par::MAXIMUM,   m_prod.Max()).Finalize().Plot(m_prod)) << Nl;
                    LOG << "Bat charge    [Ah]:\n"   << AsciiPlot::Build()(Par::MAXIMUM, batteryCopy.m_maxCapacityAmph)
                    (Par::MINIMUM, m_dataModel.GetBatPars().MIN_LOAD_AMPH)(Par::COLORS, true)
                    (Par::MULTILINE, true)
                    .Finalize().Plot(m_loads) << Nl;
                    LOG << "Total usage   [A] :\n"   << AsciiPlot::Build()(Par::MAXIMUM, batteryCopy.pars.MAX_DISCHARGE_AMP)
                    (Par::COLORS, true)
                    .Finalize().Plot(m_usages) << Nl;
                    //LOG << SolUtil().GetT() << Nl;
                }
            }
        }
    }

    return sumAdjusted;
    //return -sum;
}

OptiSubjectEnProfit::SimResult OptiSubjectEnProfit::Simulate(int i, int currHour, size_t compSize, const EnjoLib::Matrix & dataMat, double bonusMul, double bonusMulMA, bool isInitialLoad) const
{
    SimResult res{};
    //const EnjoLib::Array<Computer> & comps = m_dataModel.GetComputers();
    res.sumPowerUsage += m_dataModel.GetHabitsUsage(i);
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
