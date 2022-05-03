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

        VecStr symbols = {confSym.symbol};
        VecStr periods = {confSym.period};

        const SymbolFactoryClean symFact;
        //VecUPtr<ISymbol> symbolsVec = IMainTester::Create(symFact, &confTF2, &confSym)->GetSymbolMT(symbols, periods);
        //const IPeriod & per = symbolsVec.at(0)->GetPeriod(periods.at(0));
        CorPtr<ISymbol> symbol = IMainTester::Create(symFact, &confTF2, &confSym)->GetSymbol(confSym.symbol, periods);
        const IPeriod & per = symbol->GetPeriod(periods.at(0));

        switch (confOpti.GetOperationType())
        {
        case OptiType::OPTI_TYPE_FIND:
            {
                Optim(*symbol, per);
                CorPtr<ISimulatorTS> sim = TSUtil().GetSim(per);
            }
        break;
        case OptiType::OPTI_TYPE_NONE:
        case OptiType::OPTI_TYPE_USE:
            {
                CorPtr<ISimulatorTS> sim = TSUtil().GetSim(per);
                if (confTS.PLOT_PYTHON)
                {
                    PlotPython(confSym, confTS, *sim);
                }
                if (confTS.PLOT_PYTHON_ACF)
                {
                    PlotPythonACF(confSym, confTS, *sim);
                }
            }
        break;
        case OptiType::OPTI_TYPE_XVALID:
                XValid(*symbol, per);
        break;
        }
        {LOGL << confSym.GetDateFromToStr(false) << Nl;}

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

void App::XValid(const ISymbol & sym, const IPeriod & per) const
{
    {LOGL << "XValid\n"; }
    const TSFunFactory tsFunFact;
    const TSFunType tsFunType = TSFunType::TXT; /// TODO: make user's choice

    //OptimizerTS optiTS(tsFunType, sym, per, tsFunFact); /// TODO: Opti goal: stationarity statistics
    //optiTS();
    const ConfigTS & confTS   = *gcfgMan.cfgTS.get(); /// TODO: Allow override via console
    const PredictorFactory predFact;
    const PredictorType predType = confTS.GetPredType();

    const TSInput tsin(per, confTS);

     CorPtr<ITSFun> fun = tsFunFact.Create(tsin, tsFunType);

    OptimizerPred optiPred(predType, sym, per, predFact);
    const SimulatorTSFactory simFact;
    CorPtr<ISimulatorTS> sim = simFact.CreateTS(tsin, *fun);
    fun->SetSilent();
    sim->SetSilent();
    SimulatorTSFactoryImpl simFactImpl(tsin, *fun);

    OptiHigh().WalkForwardOptiIndiv(sym, per, 1, 1, optiPred, *sim, simFactImpl);

    sim->PrintOpti();
}

void App::Optim(const ISymbol & sym, const IPeriod & per) const
{
    {LOGL << "Optim\n"; }
    const TSFunFactory tsFunFact;
    const TSFunType tsFunType = TSFunType::TXT; /// TODO: make user's choice

    //OptimizerTS optiTS(tsFunType, sym, per, tsFunFact); /// TODO: Opti goal: stationarity statistics
    //optiTS();
    const ConfigTS & confTS   = *gcfgMan.cfgTS.get(); /// TODO: Allow override via console
    const PredictorFactory predFact;
    const PredictorType predType = confTS.GetPredType();
    OptimizerPred optiPred(predType, sym, per, predFact);
    optiPred();
}

void App::PlotPython(const ConfigSym & confSym, const ConfigTS & confTS, const ISimulatorTS & sim) const
{
    {LOGL << "Plot Python\n"; }

    MatplotLine plot;
    plot.AddLine(sim.GetOutputSeries(PredictorOutputType::RECONSTRUCTION),               "Signal", "Blue");
    if (confTS.PLOT_BASELINE)
    {
        plot.AddLine(sim.GetOutputSeries(PredictorOutputType::RECONSTRUCTION_PRED_BASELINE), "Prediction baseline", "Gray");
    }
    plot.AddLine(sim.GetOutputSeries(PredictorOutputType::RECONSTRUCTION_PRED),          "Prediction modeled", "Green");

    const Str title = GetTitle(confSym);
    plot.Plot(title);
}

void App::PlotPythonACF(const ConfigSym & confSym, const ConfigTS & confTS, const ISimulatorTS & sim) const
{
    {LOGL << "Plot Python ACF\n"; }

    const int lags = confTS.PLOT_LAGS_NUM;
    const int per  = confTS.PLOT_PERIOD_NUM;
    const MatplotACF plot;
    const Str title = ": " + GetTitle(confSym);
    plot.Plot(sim.GetOutputSeries(PredictorOutputType::SERIES), lags, per, title);
}

Str App::GetTitle(const ConfigSym & confSym) const
{
    return confSym.symbol + " " + confSym.period + " " + confSym.GetDateFromToStr(false);
}
