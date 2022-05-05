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
#include "OptimizerEnProfit.h"
#include "OptiEnProfitDataModel.h"

#include <Ios/Cin.hpp>
#include <Util/Trim.hpp>
#include <Util/CoutBuf.hpp>
#include <Template/UniquePtrVecFwd.hpp>
#include <Template/UniquePtr.hpp>

using namespace EnjoLib;

App::App(){}

void App::Run(const CLIResult & cliResultCmdLine) const
{
    const ConfigTF  & confTF    = *gcfgMan.cfgTF.get();
    do
    {
        gcfgMan.Read();

        ConfigTS & confTS     = *gcfgMan.cfgTS.get();
        const ConfigTF2 & confTF2   = *gcfgMan.cfgTF2.get();
        const ConfigOpti & confOpti = *gcfgMan.cfgOpti.get();
        ConfigSym & confSym         = *gcfgMan.cfgSym.get();

        confSym.UpdateFromOther(cliResultCmdLine.m_confSym);
        confTS.UpdateFromOther(cliResultCmdLine.m_confTS);

        Optim();

        if (confTF.REPEAT)
        {
            LOGL << "q to quit" << Endl;
            EnjoLib::Str line;
            EnjoLib::Cin cinn;
            EnjoLib::GetLine(cinn, line);
            line = EnjoLib::Trim().trim(line);
            if (line == "q")
                break;
        }

    } while (confTF.REPEAT);
    //LOGL << "Plugin name = " << plugin << Nl;
}

void App::Optim() const
//void App::Optim(const ISymbol & sym, const IPeriod & per) const
{
    {LOGL << "Optim\n"; }
    const TSFunFactory tsFunFact;
    const TSFunType tsFunType = TSFunType::TXT; /// TODO: make user's choice

    OptiEnProfitDataModel dataModel(3, 24 * 10);

    OptimizerEnProfit optimizer(dataModel);
    optimizer();
}
