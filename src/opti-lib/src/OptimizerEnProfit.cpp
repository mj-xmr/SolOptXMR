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


#include <Math/MultiDimIter/MultiDimIterTpl.hpp>
#include <Math/Opti/OptiMultiNelderMead.hpp>
#include <Math/Opti/OptiMultiBinSearch.hpp>
#include <Math/GeneralMath.hpp>
#include <Util/ProgressMonit.hpp>
#include <Util/ToolsMixed.hpp>
#include <Util/Except.hpp>
#include <Util/CoutBuf.hpp>
#include <Statistical/Statistical.hpp>
#include <Template/CorradePointer.h>

#include <STD/VectorCpp.hpp>

using namespace std;
using namespace EnjoLib;

OptimizerEnProfit::OptimizerEnProfit(const OptiEnProfitDataModel & dataModel)
: m_dataModel(dataModel)
{
}
OptimizerEnProfit::~OptimizerEnProfit(){}

/// TODO: The basic multi-dim-iter interaction should go to upper library
void OptimizerEnProfit::operator()()
{
    ELO
    MultiDimIterTpl multiDimIter;
    const MultiDimIterTpl::VVt data = m_dataModel.GetData();

    float goal = 0;

    OptiSubjectEnProfit osub(m_dataModel);
    const Result<VecD> res = OptiMultiBinSearch().Run(osub, 3, 100);
    LOG << "Res = " << res.isSuccess << ", val = " << res.value.Print() << Nl;

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

void OptimizerEnProfit::AddSpace(const EnjoLib::VecD & data)
{
    m_data.push_back(data);
}

void OptimizerEnProfit::Consume(const EnjoLib::VecD & data)
{
    ELO
    //++m_iter;
    //const EnjoLib::Str & idd = m_period.GetSymbolPeriodId();

    //const OptiGoalType type = OptiGoalType::SHARPE;
    //const OptiGoalType type = gcfgMan.cfgOpti->GetGoalType();
    //const CorPtr<IOptiGoal> pgoal = OptiGoalFactory::Create(type);
    //const IOptiGoal & igoal = *pgoal;

    //LOGL << "Data = " << data.Print() << Nl;
    float goal = 0;

    OptiSubjectEnProfit osub(m_dataModel);
    const Result<VecD> res = OptiMultiBinSearch().Run(osub, 3, 100);
    LOG << "Res = " << res.isSuccess << ", val = " << res.value.Print() << Nl;

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
    {
    }
    //else
    {

        //GnuplotMan().PlotGnuplot(pos.GetProfitsCalc().GetProfits(), true); /// test
    }
}


