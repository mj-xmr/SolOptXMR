#ifndef APPSOL_H
#define APPSOL_H

#include <Util/Str.hpp>

class ConfigTS;
class ConfigSym;
class ISymbol;
class IPeriod;
class CLIResultSol;
class ISimulatorTS;

class App
{
    public:
        App();
        virtual ~App(){}

        void Run(const CLIResultSol & cliResult) const;
        //void Optim(const ISymbol & sym, const IPeriod & per) const;
        void Optim(const CLIResultSol & cliResult) const;
        void ReadJson() const;

    protected:

    private:
};

#endif // APP_H
