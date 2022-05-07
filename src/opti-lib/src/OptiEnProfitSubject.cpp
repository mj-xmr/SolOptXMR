#include "OptiEnProfitSubject.h"
#include "OptiSubjectTSUtil.h"
#include "PredictorFactory.h"
#include "Computer.h"
#include "IPeriod.h"
#include "OptiVarVec.h"
#include "ConfigMan.h"
#include "ConfigOpti.h"
#include "ConfigDirs.h"
#include "OptiEnProfitDataModel.h"

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
    //dtor
}

double BatterySimulation::iter_get_load(double inp, double out, double hours)
       {
        if (initial_load)
        {
            //out = 0; // dangerous
        }
        double discharge = hours * DISCHARGE_PER_HOUR; /// TODO: Percentual
        double balance = inp - out - discharge;
        double change = balance * MUL_POWER_2_CAPACITY;
        if (change > MAX_USAGE)
        {
        //if out > MAX_USAGE: # A valid possibility
            num_overused += 1;
            change = MAX_USAGE;
            }
        //#print(change)
        load += change;

        if (load > MAX_CAPACITY){
            load = MAX_CAPACITY;
            num_overvolted += 1;
				}
        if (load < MIN_LOAD) {
            //if (initial_load)
              //  num_undervolted_initial += 1;
            //else
                num_undervolted += 1;

                }
        //if (load < 0)
          //  load = 0;

        if (initial_load)
            if (load > MIN_LOAD)
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
    //const VecD & inp = dataMat.at(0);
    const size_t n = dataMat.at(0).size();
    /*
    CorPtr<IPredictor> fun = m_fact.Create(m_period, m_type);
    IPredictor & strat = *(fun.get());
    EnjoLib::Array<OptiVarF *> vopti = fun->GetOptiFloat().Vec();
    const double penality = OptiSubjectTSUtil().UpdateOptiGetPenality(inp, n, m_iterData, fun.get());
    CorPtr<ISimulatorTS> psim = TSUtil().GetSimPred(m_period, fun->GetOptiVec(), m_startEndFrame);
    */
    //LOG << n << Nl;
    const EnjoLib::Array<Computer> & comps = m_dataModel.GetComputers();

    BatterySimulation battery;
    double sum = 0;
    double penalitySum = 0;

    //const Computer & comp = comps.at(0); /// TODO: This has to become an array of computers, configurable by the User.

    //const VecD & powerProd = m_dataModel.GetPowerProduction();
    //const int START_AT = m_dataModel.GetStartingPoint();
    //Assertions::SizesEqual(powerProd.size(), (size_t)n, "powr prod");
    //VecD hashes, loads, penalityUnder, input, prod;
    //VecD input, prod, loads;
    for (int i = 0; i < n; ++i)
    {
        const double bonusMul = HashrateBonus(i % 24);
        double usage = 0;
       //LOG << "i = " << i << ", val = " << inp[i] << Nl;
       //if (not battery.initial_load)
       //if (false)
       for (int ic = 0; ic < comps.size(); ++ic)
       {
           const Computer & comp = comps.at(ic);
           const VecD & inp = dataMat.at(ic);
           const double val = inp[i];
           const double hashe = comp.GetHashRate(val) * bonusMul;
           sum += hashe;
           usage += comp.GetUsage(val);
        }
       const double load = battery.iter_get_load(m_dataModel.GetPowerProduction(i), usage);
       //const double pentalityUndervolted = load < 0 ? GMat().Fabs(load * load * load) : 0;
       const double pentalityUndervolted = battery.num_undervolted;
       const double pentalityOvervolted = battery.num_overvolted;
       //penalityUnder.Add(pentalityUndervolted);
       penalitySum += pentalityUndervolted;
       penalitySum += pentalityOvervolted;

        //input.Add(val);
        if (false)
        {
       //input.Add(val);
       //loads.Add(load);
        //prod.Add(powerProd[i]);
        //hashes.Add(sum);
        }


    }
    //LOGL << input.Print() << Nl;
    //EnjoLib::Array<const VecD *> data;
    //data.push_back(&hashes);
    //data.push_back(&loads);
    //GnuplotPlotTerminal1d(hashes, "hashes", 1, 0.5);
    //GnuplotPlotTerminal1dSubplots(data, "Hashes, loads", 1, 0.5); /// TODO: segfaults
		 //psim->GetScorePred();
    //const double pentalityUndervolted = m_battery.num_undervolted * m_battery.num_undervolted;
    //const double pentalityUndervolted = penalitySum * 10000;
    //const double pentalityOvervolted = battery.num_overvolted;
    const double penality = penalitySum * 10000; /// TODO: Penalize overvoltage differently than undervoltage
    /// TODO: The undervoltage / overvoltage penality should be non-linear.
    const double bonusNetworkDiff = 0;
    //const double sumAdjusted = sum - penality + bonusNetworkDiff;
    double sumAdjusted = sum + bonusNetworkDiff - penality;
    if (penality > 0 && sumAdjusted > 0)
    {
        //sumAdjusted = 0;
    }

            //LOGL << sum << ", adj = "  << sumAdjusted << Endl;

    //if (GMat().round(sumAdjusted) > GMat().round(m_sumMax) || m_sumMax == 0)
    if (verbose)
    {
        //LOGL << sum << ", adj = "  << sumAdjusted << Endl;
        m_sumMax = sumAdjusted;

        m_optiFloatResult.clear();
        //for (OptiVarF * v : vopti)
          //  m_optiFloatResult.push_back(*v);

        //if (gcfgMan.cfgOpti->OPTI_VERBOSE && m_isVerbose)
        if (gcfgMan.cfgOpti->OPTI_VERBOSE)
        //if (false)
        {
            if (not gcfgMan.cfgOpti->IsXValid())
            {

//psim->PrintOpti();
                //GnuplotMan().PlotGnuplot(profits.GetProfits(), "/tmp/a", true);
                //PrintCurrentResults();
                LOGL << ": New goal = " << sumAdjusted << ", m_sumMax = " << m_sumMax << ", penality = " << penality << ", after " << 0 << " iterations\n";

                BatterySimulation batteryCopy;
                VecD hashes, loads, penalityUnder, input, prod, hashrateBonus, usages;
                for (int i = 0; i < n; ++i)
                {
                    /// TODO: Remove duplication
                    const double bonusMul = HashrateBonus(i % 24);
                    double usage = 0;
                   //LOG << "i = " << i << ", val = " << inp[i] << Nl;
                   //if (not battery.initial_load)
                   //if (false)
                   for (int ic = 0; ic < comps.size(); ++ic)
                   {
                       const Computer & comp = comps.at(ic);
                       const VecD & inp = dataMat.at(ic);
                       const double val = inp[i];
                       const double hashe = comp.GetHashRate(val) * bonusMul;
                       sum += hashe;
                       usage += comp.GetUsage(val);
                    }
                   const double load = batteryCopy.iter_get_load(m_dataModel.GetPowerProduction(i), usage);

                    usages.Add(usage);
                   //input.Add(val);
       loads.Add(load);
        prod.Add(m_dataModel.GetPowerProduction(i));
        hashes.Add(sum);
        hashrateBonus.Add(HashrateBonus(i % 24));
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

void OptiSubjectEnProfit::OutputVar(const EnjoLib::VecD & data, const EnjoLib::Str & descr, bool plot) const
{
    if (plot)
    {

    GnuplotPlotTerminal1d(data, descr, 1, 0.5);
    }
    Ofstream fout("/tmp/soloptout-" + descr + ".txt");
    fout << data.Print() << Nl;
}

/*
void OptiSubjectEnProfit::UpdateOutput()
{

}
*/
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

double OptiSubjectEnProfit::HashrateBonus(int hour) const
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
