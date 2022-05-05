#include "App.h"
#include <ConfigMan.h>
#include <ISymbol.h>
#include <IPeriod.h>
#include <IMainTester.h>
#include <SymbolFactoryClean.h>
#include "SimulatorTSFactory.h"
//#include "SymbolFactory.h"
#include "IMainTester.h"
#include "TSFunType.h"
#include "TSFunFactory.h"
#include "TSInput.h"
#include "ConfigTS.h"
#include "ConfigTF.h"
#include "ConfigTF2.h"
#include "ConfigSym.h"
#include "ConfigOpti.h"
#include "OptiType.h"
#include "OptimizerTS.h"
#include "OptimizerPred.h"
#include "PredictorFactory.h"
#include "TSUtil.h"
#include "OptiHigh.h"
#include "SimulatorTSFactoryImpl.h"
#include "MatplotLine.h"
#include "MatplotACF.h"
#include "PredictorOutputType.h"
#include "CLIResult.h"
#include "CLIResultSol.h"
#include "OptimizerEnProfit.h"
#include "OptiEnProfitDataModel.h"

#include <Ios/Cin.hpp>
#include <Util/Trim.hpp>
#include <Util/CoutBuf.hpp>
#include <Template/UniquePtrVecFwd.hpp>
#include <Template/UniquePtr.hpp>

using namespace EnjoLib;

App::App(){}

void App::Run(const CLIResultSol & cliResultCmdLine) const
{
    //const ConfigSol  & confSol    = *gcfgMan.cfgTF.get();
    //do
    {
        Optim(cliResultCmdLine);
    } //while (confTF.REPEAT);
    //LOGL << "Plugin name = " << plugin << Nl;
}

void App::Optim(const CLIResultSol & cliSol) const
//void App::Optim(const ISymbol & sym, const IPeriod & per) const
{
    {LOGL << "Optim\n"
        << "Horizon = " << cliSol.m_confSol.DAYS_HORIZON << Nl
        << "Start day = "  << cliSol.m_confSol.DAYS_START << Nl
        ;}

    int horizon = cliSol.m_confSol.DAYS_HORIZON;

    if (horizon <= 0)
    {
        horizon = 3;
    }

    OptiEnProfitDataModel dataModel(horizon, 24 * cliSol.m_confSol.DAYS_START);

    OptimizerEnProfit optimizer(dataModel);
    optimizer();
}
