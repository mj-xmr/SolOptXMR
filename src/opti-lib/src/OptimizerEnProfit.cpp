#include "OptimizerEnProfit.h"
#include "IOptimizable.h"
#include "IPeriod.h"
#include "ConfigGlob.h"
#include "ConfigMan.h"
#include "ConfigOpti.h"

#include "TradeUtil.h"
#include "OptiVarVec.h"
#include "OptiType.h"
//#include "OptiGoalFactory.h"
#include "OptiEnProfitSubject.h"
#include "OptiEnProfitDataModel.h"
#include "GnuplotIOSWrap.h"

#include <Math/MultiDimIter/MultiDimIterTpl.hpp>
#include <Math/Opti/OptiMultiNelderMead.hpp>
#include <Math/Opti/OptiMultiBinSearch.hpp>
//#include "OptiMultiBinSearch01.hpp"
#include <Math/GeneralMath.hpp>
#include <Util/ProgressMonit.hpp>
#include <Util/CharManipulations.hpp>
#include <Util/ToolsMixed.hpp>
#include <Util/Except.hpp>
#include <Util/CoutBuf.hpp>
#include <Statistical/Statistical.hpp>
#include <Statistical/Distrib.hpp>
#include <Template/CorradePointer.h>

#include <Math/RandomMath.hpp>
#include <Math/GeneralMath.hpp>

#include <STD/VectorCpp.hpp>
#include <STD/Set.hpp>
#include <STD/String.hpp>

using namespace std;
using namespace EnjoLib;

OptimizerEnProfit::OptimizerEnProfit(const OptiEnProfitDataModel & dataModel)
    : m_dataModel(dataModel)
{
}
OptimizerEnProfit::~OptimizerEnProfit() {}

bool OptimizerEnProfit::IsUseHash() const
{
    return true;
    //return false;

    const int MIN_HOURS_HASHMAP = 2 * 24;
    const int hours = m_dataModel.GetHorizonHours();
    return hours > MIN_HOURS_HASHMAP;
}

/// TODO: The basic multi-dim-iter interaction should go to upper library
void OptimizerEnProfit::operator()()
{
    ELO
    MultiDimIterTpl multiDimIter;
    //const MultiDimIterTpl::VVt data = m_dataModel.GetData();
    //const MultiDimIterTpl::VVt data;

    float goal = 0;

    //OptiSubjectEnProfit osub(m_dataModel);
    //const Result<VecD> res = OptiMultiBinSearch01().Run01(osub, 3, 1000);
    //LOG << "Res = " << res.isSuccess << ", val = " << res.value.Print() << Nl;
    //GnuplotPlotTerminal1d(res.value, "hashes", 1, 0.5);

    /// TODO: Generalize and upstream this:
    //const long int maxSamples = 200;
    //const bool randomSearch = false;
    const bool randomSearch = true;
    //if (gcfgMan.cfgOpti->IsSearchRandom())
    if (randomSearch)
    {
        RandomSearch();
    }
    else
    {
        //multiDimIter.StartIteration(data, *this);
    }
    //multiDimIter.StartIteration(data, *this);
    /*
    const size_t numVars = GetOptiFloat().size();
    for (unsigned iVar = 0; iVar < numVars; ++iVar)
    {
        const OptiVarF & var = GetOptiFloat().at(iVar);
        if (gcfgMan.cfgOpti->IsSearchFloatingPoint() && var.fp)
            continue;
        data.push_back(var.GetSpace().Data());
    }
    if (gcfgMan.cfgOpti->IsSearchRandom())
    {
        const long int maxSamples = gcfgMan.cfgOpti->OPTI_RANDOM_SAMPLES_NUM;
        const MultiDimIterTpl::VVt & dataT = data.T();
        MultiDimIterTpl::VVt dataNewT; // Shorten the data down to the requested number of samples
        /// TODO: Unit Test. Crucial element
        for (unsigned i = 0; i < maxSamples && i < dataT.size() ; ++i)
        {
            dataNewT.Add(dataT.at(i));
        }
        for (unsigned i = 0; i < dataNewT.size(); ++i)
        {
            Consume(dataNewT.at(i));
            if (gcfgMan.cfgOpti->OPTI_RANDOM_EARLY_STOP && IsEarlyStop())
            {
                LOGL << "Early stop. The recent variance changes were less than " << gcfgMan.cfgOpti->OPTI_RANDOM_MIN_DIFF_PROMILE << " ‰ after " << i << " iterations.\n";
                break;
            }
        }
    }
    else
    {
        multiDimIter.StartIteration(data, *this);
    }

    //Assertions::Throw("maxRows", data.size(), "Empty variable in OptimizerBase");
    //LOGL << "size = " << data.size() << Endl;



    for (OptiVarF v : GetOptiFloatResult())
    {
        for (const IPeriod * period : GetPeriods())
            v.StoreVariable(*period);
    }

    if (gcfgMan.cfgOpti->OPTI_VERBOSE && m_isVerbose)
        if (not gcfgMan.cfgOpti->IsXValid())
        {
            LOGL << Endl;
        }

    //cout << id << endl;
    PrintCurrentResults();
    PrintStatsSummary();
    */
}

void OptimizerEnProfit::RandomSearch()
{
    const int horizonHours = m_dataModel.GetHorizonHours();
    const EnjoLib::Array<Computer> & comps = m_dataModel.GetComputers();
    const int numComputers = 1; /// TODO: Read from comps!
    const RandomMath rmath;
    rmath.RandSeed();
    const VecD binaryZero(horizonHours);
    const std::string hashStrZero(horizonHours * numComputers, '0');
    std::string hashStr = hashStrZero;
    Matrix binaryMat;
    binaryMat.Add(binaryZero);
    VecD & binary = binaryMat.at(0);
    Matrix binarBest = binaryMat;

    const bool useHash = IsUseHash();
    const int maxEl = 10e8;
    short bit = 1;
    char bitC = '1';
    std::set<std::string> usedCombinations;
    int alreadyCombined = 0;
    const GMat gmat;
    const int minHoursTogether = 3; /// TODO: This should be computer's parameter or user's tolerance
    const int minHoursTogetherHalf = gmat.round(minHoursTogether/2.0);
    for (int i = 0; i < maxEl; ++i)
    {
        const int index = gmat.round(rmath.Rand(0, horizonHours-0.999));
        if (bit == 1)
        {
            for (int j = index - minHoursTogetherHalf; j <= index + minHoursTogetherHalf; ++j)
            {
                if (j < 0 || j >= horizonHours)
                {
                    continue;
                }
                binary[j] = bit; /// Each computer gets its own binary.
                hashStr.at(j) = bitC; /// TODO: j * (1 + computerIDX)
            }
        }
        int sum = 0;
        for (int l = 0; l < horizonHours; ++l)
        {
            sum += binary[l];
        }
        if (sum == horizonHours)
        {
            binary = binaryZero;
            hashStr = hashStrZero;
            bit = 1;
        }
        bool found = false;
        if (useHash)
        {
            found = not usedCombinations.insert(hashStr).second;
            //found = usedCombinations.count(hashStr);
        }
        if (found)
        {
            ++alreadyCombined;
            ++m_numFailed;
        }
        else
        {
            if (useHash)
            {
                //usedCombinations.insert(hashStr);
            }
            if (Consume2(binaryMat))
            {
                m_numFailed = 0;
                binarBest = binaryMat;
            }
            else
            {
                ++m_numFailed;
            }
        }

        const int MAX_FAILED = 200000;
        if (m_numFailed >= MAX_FAILED)
        {
            LOGL << "Early stop after " << m_numFailed << " last failed attempts." << Nl
                 << "Repeated combinations = " << alreadyCombined << Nl;
            break;
        }
    }

    PrintSolution(binarBest);
}

void OptimizerEnProfit::PrintSolution(const EnjoLib::Matrix & bestMat) const
{
    for (int i = 0; i < bestMat.size(); ++i)
    {
        GnuplotPlotTerminal1d(bestMat.at(i), "Best solution = " + CharManipulations().ToStr(m_goal), 1, 0.5);
    }
    const Distrib distr;
    const DistribData & distribDat = distr.GetDistrib(m_goals);
    if (distribDat.IsValid())
    {
        GnuplotPlotTerminal2d(distribDat.data, "Solution distribution", 1, 0.5);
    }

    ELO
    LOG << "Computer start schedule:\n";
    for (int i = 0; i < bestMat.size(); ++i)
    {
        const VecD & best = bestMat.at(i);
        LOG << best.Print() << Nl;

        int lastHourOn = -1;
        int lastDayOn = -1;
        const int horizonHours = m_dataModel.GetHorizonHours();
        for (int i = 1; i < horizonHours; ++i)
        {
            const int hour = i % 24;
            const int day  = GMat().round(i / 24.0);
            const int dayPrev  = GMat().round((i-1) / 24.0);
            if (day != dayPrev)
            {
                LOG << Nl;
            }

            const bool onPrev = best.at(i-1);
            const bool onCurr = best.at(i);
            if (not onPrev && onCurr)
            {
                lastHourOn = hour;
                lastDayOn = day;
            }
            if (lastHourOn > 0)
            {
                if (not onCurr) // Switch off
                {
                    const int hourPrev = (i - 1) % 24;
                    LOG << "day " << lastDayOn << ", hour " << lastHourOn << "-" << hourPrev << Nl;
                    lastHourOn = -1;
                }
                else if (i == horizonHours - 1)
                {
                    LOG << "day " << lastDayOn << ", hour " << lastHourOn << "-.." << Nl;
                }
            }

        }
    }
}

void OptimizerEnProfit::AddSpace(const EnjoLib::VecD & data)
{
    m_data.push_back(data);
}

void OptimizerEnProfit::Consume(const EnjoLib::VecD & data)
{

}
bool OptimizerEnProfit::Consume2(const EnjoLib::Matrix & dataMat)
{
    const VecD & data = dataMat.at(0);
    //ELO
    //++m_iter;
    //const EnjoLib::Str & idd = m_period.GetSymbolPeriodId();

    //const OptiGoalType type = OptiGoalType::SHARPE;
    //const OptiGoalType type = gcfgMan.cfgOpti->GetGoalType();
    //const CorPtr<IOptiGoal> pgoal = OptiGoalFactory::Create(type);
    //const IOptiGoal & igoal = *pgoal;
    //ELO
    //LOG << "Data = " << data.Print() << Nl;
    OptiSubjectEnProfit osub(m_dataModel);
    float goal = osub.GetVerbose(dataMat);
    //const Result<VecD> res = OptiMultiBinSearch().Run(osub, 3, 100);
    //const Result<VecD> res = OptiMultiBinSearch01().Run01(osub, 3, 100);
    //LOGL << "Res = " << res.isSuccess << ", val = " << res.value.Print() << Nl;

    //CorPtr<IPosition> pos = IPosition::Create(m_period.GetSymbolName());
    switch (gcfgMan.cfgOpti->GetMethod())
    {
    case OptiMethod::OPTI_METHOD_BISECTION:
    {
        /*
        SafePtrThin<IOptiSubjectTF> osub(IOptiSubjectTF::Create(m_sym, m_period, m_stratType, m_stratFact, igoal, GetOptiFloat(), data));
        //Result<VecD> res = OptiMultiNelderMead().Run(*osub, 0.1, 10, 10);
        Result<VecD> res = OptiMultiBinSearch().Run(*osub, 3, 100); /// TODO: Make multithreaded
        if (not res.isSuccess)
        {
            LOGL << idd << ": Failed Nelder-Mead" << Nl;
            //return;
        }
        //pos->CopyFrom(osub->GetPosition());

        //const ProfitsCalc & profits = pos->GetProfitsCalc();
        //float sum = igoal.GetGoal(profits);
        LOGL << idd << ": Nelder-Mead sum = " << sum << Nl;

        //PrintArgs(os.GetOptiVarsResult());
        //if (IsOptiGoalReached(profits))
        {
            GetOptiFloatResult() = osub->GetOptiVarsResult();
        }
        */
    }
    break;
    case OptiMethod::OPTI_METHOD_MONTECARLO:
    case OptiMethod::OPTI_METHOD_GRID:
    {
        /*
        const TSInput tsin(m_period, *gcfgMan.cfgTS.get());
        CorPtr<ITSFun> fun = m_funFact.Create(tsin, m_funType);
        fun->UpdateOptiVars(data);
        CorPtr<ISimulatorTS> psim = TSUtil().GetSim(m_period, *fun, m_startEndFrame);

        goal = psim->GetScoreStationarity();

        if (IsGoalReached(goal))
        {
            OnGoalReached(fun.get());
        }
        */
        /// TODO: MonteCarlo early stop if goal's variance stops changing
        //ToolsMixed().SystemCallWarn("clear", __PRETTY_FUNCTION__); PrintCurrentResults();
    }
    }

    //const ProfitsCalc & profits = pos->GetProfitsCalc();
    //m_calcs.Add(profits);
    //m_goals.Add(igoal.GetGoal(profits));
    //AddGoal(goal);
    //const bool verbose = gcfgMan.cfgOpti->OPTI_VERBOSE && m_isVerbose;
    //if (IsGoalReached(goal))
    //LOGL << "goal = " << goal << Nl;
    m_goals.Add(goal);
    if (goal > m_goal)
    {
        m_goal = goal;
        m_numFailed = 0;
        LOGL << "New score = " << goal << Nl;

        osub.GetVerbose(dataMat, true);
        //osub.GetVerbose(dataMat, false);
        return true;

    }
    else
    {
        return false;

        //GnuplotMan().PlotGnuplot(pos.GetProfitsCalc().GetProfits(), true); /// test

    }
}


