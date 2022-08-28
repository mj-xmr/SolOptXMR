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
#include <Math/RandomMath.hpp>
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
#include <Visual/AsciiPlot.hpp>
#include <Visual/AsciiMisc.hpp>

#include "OptiEnProfitSubject.h" /// TODO: Remove

#include <STD/VectorCpp.hpp>
#include <STD/Set.hpp>
#include <STD/String.hpp>
#include <STD/Algorithm.hpp>

#include <limits>

using namespace std;
using namespace EnjoLib;

const int OptimizerEnProfit::HOURS_IN_DAY = 24;
const double OptimizerEnProfit::GOAL_INITIAL = std::numeric_limits<double>::min();

const OptimizerEnProfit::BigInt OptimizerEnProfit::MAX_NUM_COMBINATIONS = std::numeric_limits<BigInt>::max(); //1e7;
//const OptimizerEnProfit::BigInt OptimizerEnProfit::MAX_NUM_COMBINATIONS = 3e6;
const double OptimizerEnProfit::MAX_FAILED_COMBINATIONS = 0.60;
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

EnjoLib::Str OptimizerEnProfit::GetT() const
{
    return SolUtil().GetT();
}

void OptimizerEnProfit::RandomSearch()
{
    const int MAX_NUM_SOLUTIONS = 3;

    const int horizonHours = m_dataModel.GetHorizonHours();
    const EnjoLib::Array<Computer> & comps = m_dataModel.GetComputers();
    const int numComputers = comps.size();

    const GMat gmat;

    // A heuristic to get the number of possible combinations.
    // TODO: Should check the variance changes
    const BigInt maxIter = gmat.Pow(gmat.Pow(horizonHours, 3), gmat.Pow(numComputers, 1/2.5));

    {LOGL << GetT() << "Random search of " << maxIter << " solutions\n"
    << "Hours = " << horizonHours << ", computers = " << numComputers << Nl;}

    const RandomMath rmath;
    rmath.RandSeed(m_dataModel.GetConf().RANDOM_SEED);
    const VecD binaryZero(horizonHours);
    const std::string hashStrZero(horizonHours * numComputers, '0');
    std::string hashStr = hashStrZero;
    Matrix binaryMat;
    struct Sol0Penality
    {
        Solution sol;
        Matrix  dat;

        bool operator < (const Sol0Penality & other) const
        {
            return sol.hashes < other.sol.hashes;
        }
    };
    std::vector<Sol0Penality> solutions0Penality;
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
    const BigInt maxCombisFailed = maxIter * MAX_FAILED_COMBINATIONS;
    short bit = 1;
    char bitC = '1';
    std::set<std::string> usedCombinations;
    int alreadyCombined = 0;
    //const Distrib distr;
    const bool animateProgressBar = m_dataModel.IsAnimateProgressBar();
    ProgressMonitHigh progressMonitor(20);
    bool needNewLine = false;
    for (BigInt i = 0; i < maxIter; ++i)
    {
        if (animateProgressBar)
        {
            if (i % 100000 == 0)
            {
                progressMonitor.PrintProgressBarTime(i, maxIter, "Solutions");
                //if (i > 0)
                {
                //const DistribData & data = distr.GetDistrib(m_goals, 20); const Str & dstr = distr.PlotLine(data, true, true, true);
                //progressMonitor.PrintProgressBarTime(i, maxIter, dstr);
                }
                needNewLine = true;
            }
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
            if (Consume2(binaryMat, needNewLine))
            {
                SOL_LOG(GetT() + "Consume success: " + binaryMat.Print());
                //LOGL << "Consume success: " << binaryMat.Print() << '\n';
                m_numFailed = 0;
                binarBest = binaryMat;
                m_uniqueSolutionsPrev = m_uniqueSolutions;
                m_uniqueSolutions = usedCombinations.size();
                foundFirstSolution = true;
                needNewLine = false;

                if (m_penality == 0)
                {
                    Sol0Penality soldat;
                    soldat.sol = m_currSolution;
                    soldat.dat = binaryMat;
                    solutions0Penality.push_back(soldat);
                }
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
    {LOGL << Nl << GetT() << "Finished." << Nl;}
    const Str notFoundSolutionWarn = StrColour::GenWarn("Couldn't find a solution!\n"
                                                        "The usual remedy is to increase the number of batteries, "
                                                        "or reduce the load in 'habits' configuration file.\n"
                                                        "If the batteries are overcharged already, "
                                                        "the best thing to do is to start your machines now."
                                                        );
    if (not foundFirstSolution)
    {
        // TODO: Unit test it.
        //Assertions::Throw("Couldn't find a solution!", "OptimizerEnProfit::RandomSearch");
        LOGL << Nl << notFoundSolutionWarn << Nl;
    }

    if (solutions0Penality.empty())
    {
        PrintSolution(binarBest);
    }
    else
    {
        std::sort(solutions0Penality.begin(), solutions0Penality.end());
        std::reverse(solutions0Penality.begin(), solutions0Penality.end());
        std::vector<Sol0Penality> solutions0PenalitySelected;
        for (int i = 0; i < solutions0Penality.size() && i < MAX_NUM_SOLUTIONS; ++i)
        {
            const Sol0Penality & soldat = solutions0Penality.at(i);
            solutions0PenalitySelected.push_back(soldat);

            //PrintSolution(soldat.dat);
        }
        const Sol0Penality & soldatBest = solutions0Penality.at(0);
        std::reverse(solutions0PenalitySelected.begin(), solutions0PenalitySelected.end());
        for (int i = 0; i < solutions0PenalitySelected.size(); ++i)
        {
            const Sol0Penality & soldat = solutions0PenalitySelected.at(i);
            {
                ELO
                const int len = 20;
                LOG << AsciiMisc().GenChars("-", len) << Nl;
                LOG << "Solution " << i+1 << " of " << MAX_NUM_SOLUTIONS << Nl;
                LOG << AsciiMisc().GenChars("-", len) << Nl;
            }
            const double hashes = soldatBest.sol.hashes;
            PrintSolution(soldat.dat, hashes);
        }
        //const Sol0Penality & soldatBest = solutions0Penality.at(solutions0Penality.size() - 1);
        //PrintSolution(soldatBest.dat);
    }

    if (not foundFirstSolution)
    {
        LOGL << Nl << notFoundSolutionWarn << Nl;
    }
}

bool OptimizerEnProfit::Consume2(const EnjoLib::Matrix & dataMat, bool needNewline)
{
    OptiSubjectEnProfit osub(m_dataModel);
    const Solution & goal = osub.GetVerbose(dataMat, false);
    //LOGL << "goal = " << goal << Nl;
    m_goals.Add(goal.penality);
    if (goal.penality < m_goal || m_goal == GOAL_INITIAL || goal.penality == 0)
    {
        const double relChangePositive = GMat().RelativeChange(goal.penality, m_goal);
        m_relChangePositive = relChangePositive;
        ELO
        RecalcComputationCosts();
        if (goal.penality < m_goal)
        {
            if (needNewline)
            {
                LOG << Nl; // Need an extra space to clear the progress bar
            }
            LOG << GetT() << "New score = " << goal.hashes << " ->\t"
            << GMat().round(relChangePositive   * 100) << "%" << " costing: "
            << GMat().round(m_relChangeNegative * 100) << "%" << ", pos2neg: "
            << GMat().round(m_relPos2Neg        * 100) << "%" << Nl;
    //        << GMat().round(relNeg2Pos * 100) << "%" << Nl;
        }


        //osub.GetVerbose(dataMat, true);
        osub.GetVerbose(dataMat, false); /// TODO: Why would we recalculate it in a non-verbose mode? To store the final result?
        m_goal = goal.penality;
        m_penality = osub.GetPenality();

        if (goal.penality == 0)
        {
            m_currSolution = goal;
            //LOG << GetT() << "0 = Penality!" << Nl;
        }

        m_hashes = goal.hashes;

        return true;
    }
    else
    {
        return false;
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


void OptimizerEnProfit::RecalcComputationCosts()
{
    m_relChangeNegative = GMat().RelativeChange(m_uniqueSolutions, m_uniqueSolutionsPrev);
    m_relPos2Neg = GMatRatio(m_relChangePositive, m_relChangeNegative);
}
