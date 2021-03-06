#include "OptimizerEnProfit.h"
#include "IOptimizable.h"
#include "IPeriod.h"
#include "ConfigGlob.h"
#include "ConfigMan.h"
#include "ConfigOpti.h"
#include "ConfigSol.h"

#include "TradeUtil.h"
#include "OptiVarVec.h"
#include "OptiType.h"
//#include "OptiGoalFactory.h"
#include "OptiEnProfitSubject.h"
#include "OptiEnProfitDataModel.h"
#include "GnuplotIOSWrap.h"
#include "SolUtil.h"
#include "TimeUtil.h"

#include <Math/GeneralMath.hpp>
#include <Util/ProgressMonit.hpp>
#include <Util/ProgressMonitHigh.hpp>
#include <Util/CharManipulations.hpp>
#include <Util/ToolsMixed.hpp>
#include <Util/Except.hpp>
#include <Util/CoutBuf.hpp>
#include <Util/StrColour.hpp>
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

const int OptimizerEnProfit::HOURS_IN_DAY = 24;
const int OptimizerEnProfit::MAX_NUM_COMBINATIONS = 1e7;
const double OptimizerEnProfit::MAX_FAILED_COMBINATIONS = 0.40;
const double OptimizerEnProfit::MIN_POS_2_NEG_CHANGE_RATIO = 0.01;

OptimizerEnProfit::OptimizerEnProfit(const OptiEnProfitDataModel & dataModel)
    : m_dataModel(dataModel)
{
}
OptimizerEnProfit::~OptimizerEnProfit() {}

bool OptimizerEnProfit::IsUseHash() const
{
    return true;
    //return false;

    const int MIN_HOURS_HASHMAP = 2 * HOURS_IN_DAY;
    const int hours = m_dataModel.GetHorizonHours();
    return hours > MIN_HOURS_HASHMAP;
}

/// TODO: The basic multi-dim-iter interaction should go to upper library
void OptimizerEnProfit::operator()()
{
    ELO
    float goal = 0;
    //const bool randomSearch = false;
    const bool randomSearch = true;
    if (randomSearch)
    {
        RandomSearch();
    }
    else
    {
        //multiDimIter.StartIteration(data, *this);
    }
}

void OptimizerEnProfit::RandomSearch()
{
    SOL_LOG("Random search");
    const int horizonHours = m_dataModel.GetHorizonHours();
    const EnjoLib::Array<Computer> & comps = m_dataModel.GetComputers();
    const int numComputers = comps.size();
    const RandomMath rmath;
    rmath.RandSeed(m_dataModel.GetConf().RANDOM_SEED);
    const VecD binaryZero(horizonHours);
    const std::string hashStrZero(horizonHours * numComputers, '0');
    std::string hashStr = hashStrZero;
    Matrix binaryMat;
    VecT<int> minHoursTogetherHalfVec;
    for (const Computer & comp : comps)
    {
        binaryMat.Add(binaryZero);
        const int minHoursTogetherHalf = GMat().round(comp.minRunHours/2.0);
        minHoursTogetherHalfVec.push_back(minHoursTogetherHalf);
    }
    Matrix binarBest = binaryMat;

    bool foundFirstSolution = false;
    const bool useHash = IsUseHash();
    const int maxCombisFailed = MAX_NUM_COMBINATIONS * MAX_FAILED_COMBINATIONS;
    short bit = 1;
    char bitC = '1';
    std::set<std::string> usedCombinations;
    int alreadyCombined = 0;
    const GMat gmat;
    ProgressMonitHigh progressMonitor;
    for (int i = 0; i < MAX_NUM_COMBINATIONS; ++i)
    {
        if (i % 100000 == 0)
        {
            progressMonitor.PrintProgressBarTime(i, MAX_NUM_COMBINATIONS);
        }
        const int icomp = gmat.round(rmath.Rand(0, numComputers-1));
        //for (int icomp = 0; icomp < numComputers; ++icomp)
        {
            //LOGL << "icomp = " << icomp << Nl;
            VecD & binary = binaryMat.at(icomp);
            const int minHoursTogetherHalf = minHoursTogetherHalfVec.at(icomp);
            const int compIdxMul = 1 + icomp;
            const int index = gmat.round(rmath.Rand(0, horizonHours-1));
            //if (bit == 1)
            {
                for (int j = index - minHoursTogetherHalf; j <= index + minHoursTogetherHalf; ++j)
                {
                    if (j < 0 || j >= horizonHours)
                    {
                        continue;
                    }
                    binary[j] = bit; /// Each computer gets its own binary.
                    hashStr.at(j * compIdxMul) = bitC; /// TODO: j * (1 + computerIDX)
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
                for (int j = horizonHours * icomp; j < horizonHours * (icomp + 1); ++j)
                {
                    hashStr.at(j) = '0'; /// TODO: Something is wrong here
                }
                //ELO
                //LOG << "Hash pre: " << hashStr << Nl;
                //hashStr = hashStrZero;
                //bit = 1;
                //LOG << "\nHash post: " << hashStr << Nl;
            }
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
            if (Consume2(binaryMat))
            {
                SOL_LOG("Consume success: " + binaryMat.Print());
                //LOGL << "Consume success: " << binaryMat.Print() << '\n';
                m_numFailed = 0;
                binarBest = binaryMat;
                m_uniqueSolutionsPrev = m_uniqueSolutions;
                m_uniqueSolutions = usedCombinations.size();
                foundFirstSolution = true;
            }
            else
            {
                //SOL_LOG("Failed: " + binaryMat.Print());
                ++m_numFailed;
            }
            RecalcComputationCosts();
        }
        const bool changeLargeEnough = m_relPos2Neg == 0 || m_relPos2Neg > MIN_POS_2_NEG_CHANGE_RATIO;
        const bool exceededNumFailed = m_numFailed >= maxCombisFailed;
        //if (exceededNumFailed && not changeLargeEnough)
        if (exceededNumFailed && foundFirstSolution)
        {
            LOGL << "Early stop after " << m_numFailed << " last failed attempts "
                 // << and last change of " << m_relPos2Neg << " < " << MIN_POS_2_NEG_CHANGE_RATIO << Nl
                 << Nl
                 << "Repeated combinations = " << alreadyCombined << " of " << maxCombisFailed << ": " << GMat().round(alreadyCombined/double(maxCombisFailed) * 100) << "%" << Nl
                 << "Unique   combinations = " << usedCombinations.size() << " of " << maxCombisFailed << ": " << GMat().round(usedCombinations.size()/double(maxCombisFailed) * 100) << "%" << Nl;
            break;
        }
    }
    if (not foundFirstSolution)
    {
        // TODO: Unit test it.
        //Assertions::Throw("Couldn't find a solution!", "OptimizerEnProfit::RandomSearch");
        LOGL << StrColour::GenWarn("Couldn't find a solution!") << "\n... in OptimizerEnProfit::RandomSearch()";
    }

    PrintSolution(binarBest);
}

void OptimizerEnProfit::PrintSolution(const EnjoLib::Matrix & bestMat) const
{
    const int currHour = TimeUtil().GetCurrentHour();
    OptiSubjectEnProfit osub(m_dataModel);
    osub.GetVerbose(bestMat, true);
    for (int i = 0; i < bestMat.size(); ++i)
    {
        //GnuplotPlotTerminal1d(bestMat.at(i), "Best solution = " + CharManipulations().ToStr(m_goal), 1, 0.5);
    }
    const Distrib distr;
    const DistribData & distribDat = distr.GetDistrib(m_goals);
    if (distribDat.IsValid())
    {
        //GnuplotPlotTerminal2d(distribDat.data, "Solution distribution", 1, 0.5);
    }

    ELO
    LOG << "\nComputer start schedule:\n";
    for (int i = 0; i < bestMat.size(); ++i)
    {
        LOG << m_dataModel.GetComputers().at(i).name << Nl;
        const VecD & best = bestMat.at(i);
        LOG << best.Print() << Nl;

        int lastHourOn = -1;
        int lastDayOn = -1;
        const int horizonHours = m_dataModel.GetHorizonHours();
        for (int i = 1; i < horizonHours; ++i)
        {
            const int ihour = i + currHour;
            const int hour = ihour % HOURS_IN_DAY;
            const int day  = GMat().round(ihour / static_cast<double>(HOURS_IN_DAY));
            const int dayPrev  = GMat().round((ihour-1) / static_cast<double>(HOURS_IN_DAY));
            if (day != dayPrev)
            {
                //LOG << Nl;
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
                    const int hourPrev = (ihour - 1) % HOURS_IN_DAY;
                    LOG << "day " << lastDayOn << ", hour " << lastHourOn << "-" << hourPrev<< Nl;
                    lastHourOn = -1;
                }
                else if (i == horizonHours - 1)
                {
                    LOG << "day " << lastDayOn << ", hour " << lastHourOn << "-.." << Nl;
                }
            }

        }
        LOG << Nl;
    }
}

void OptimizerEnProfit::AddSpace(const EnjoLib::VecD & data)
{
    m_data.push_back(data);
}

void OptimizerEnProfit::Consume(const EnjoLib::VecD & data)
{

}

static double GMatRatio(double val, double valRef)
{
    if (valRef == 0)
    {
        return 0;
    }
    return val / valRef;
}

bool OptimizerEnProfit::Consume2(const EnjoLib::Matrix & dataMat)
{
    OptiSubjectEnProfit osub(m_dataModel);
    const float goal = osub.GetVerbose(dataMat, false);
    //LOGL << "goal = " << goal << Nl;
    m_goals.Add(goal);
    if (goal > m_goal)
    {
        const double relChangePositive = GMat().RelativeChange(goal, m_goal);
        m_relChangePositive = relChangePositive;

        RecalcComputationCosts();

        LOGL << "\nNew score = " << goal << " ->\t"
        << GMat().round(relChangePositive * 100) << "%" << " costing: "
        << GMat().round(m_relChangeNegative * 100) << "%" << ", pos2neg: "
        << GMat().round(m_relPos2Neg * 100) << "%" << Nl;
//        << GMat().round(relNeg2Pos * 100) << "%" << Nl;

        //osub.GetVerbose(dataMat, true);
        osub.GetVerbose(dataMat, false); /// TODO: Why would we recalculate it in a non-verbose mode? To store the final result?
        m_goal = goal;

        return true;
    }
    else
    {
        return false;
    }
}

void OptimizerEnProfit::RecalcComputationCosts()
{
    m_relChangeNegative = GMat().RelativeChange(m_uniqueSolutions, m_uniqueSolutionsPrev);
    m_relPos2Neg = GMatRatio(m_relChangePositive, m_relChangeNegative);
}
