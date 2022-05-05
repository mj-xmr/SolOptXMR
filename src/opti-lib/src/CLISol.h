#ifndef CLISOL_H
#define CLISOL_H

#include "CLI.h"

#include <Util/Str.hpp>
#include <Util/Result.hpp>

struct CLIResultSol;

class CLISol // : public CLI // TODO
{
    public:
        CLISol();
        virtual ~CLISol();

        EnjoLib::Result<CLIResultSol> GetConfigs(int argc, char ** argv) const;
        EnjoLib::Str GetAdditionalHelp() const;

    protected:

    private:
};

#endif // CLISOL_H
