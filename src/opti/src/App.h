#ifndef APP_H
#define APP_H

#include <Util/Str.hpp>

class ConfigTS;
class ConfigSym;
class ISymbol;
class IPeriod;
class CLIResult;
class ISimulatorTS;

class App
{
    public:
        App();
        virtual ~App(){}

        void Run(const CLIResult & cliResult) const;
        void Optim(const ISymbol & sym, const IPeriod & per) const;
        void XValid(const ISymbol & sym, const IPeriod & per) const;
        void PlotPython(const ConfigSym & confSym, const ConfigTS & confTS, const ISimulatorTS & sim) const;
        void PlotPythonACF(const ConfigSym & confSym, const ConfigTS & confTS, const ISimulatorTS & sim) const;

        EnjoLib::Str GetTitle(const ConfigSym & confSym) const;

    protected:

    private:
};

#endif // APP_H
