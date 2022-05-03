#include "OptiEnProfitSubject.h"
#include "OptiSubjectTSUtil.h"
#include "PredictorFactory.h"
#include "IPeriod.h"
#include "OptiVarVec.h"
#include "ConfigMan.h"
#include "ConfigOpti.h"
#include "ConfigDirs.h"
#include "OptiEnProfitDataModel.h"

//#include "GnuplotMan.h"

#include <Util/CoutBuf.hpp>
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

double OptiSubjectEnProfit::Get(const double * inp, int n)
{
    /*
    CorPtr<IPredictor> fun = m_fact.Create(m_period, m_type);
    IPredictor & strat = *(fun.get());
    EnjoLib::Array<OptiVarF *> vopti = fun->GetOptiFloat().Vec();
    const double penality = OptiSubjectTSUtil().UpdateOptiGetPenality(inp, n, m_iterData, fun.get());
    CorPtr<ISimulatorTS> psim = TSUtil().GetSimPred(m_period, fun->GetOptiVec(), m_startEndFrame);
    */
    const double sum = 0; //psim->GetScorePred();
    const double penality = 0;
    const float sumAdjusted = sum - penality;

    //LOGL << sum << ", adj = "  << sumAdjusted << Endl;

    if (sum > m_sumMax || m_sumMax == 0)
    {
        //LOGL << sum << ", adj = "  << sumAdjusted << Endl;
        m_sumMax = sum;

        m_optiFloatResult.clear();
        //for (OptiVarF * v : vopti)
          //  m_optiFloatResult.push_back(*v);

        //if (gcfgMan.cfgOpti->OPTI_VERBOSE && m_isVerbose)
        if (gcfgMan.cfgOpti->OPTI_VERBOSE)
        {
            if (not gcfgMan.cfgOpti->IsXValid())
            {
                //psim->PrintOpti();
                //GnuplotMan().PlotGnuplot(profits.GetProfits(), "/tmp/a", true);
                //PrintCurrentResults();
                LOGL << ": New goal = " << sum << ", after " << 0 << " iterations\n";
            }
        }
    }

    return sumAdjusted;
    //return -sum;
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
