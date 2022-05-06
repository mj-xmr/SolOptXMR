#include "App.h"

#include "CLISol.h"
#include "CLIResultSol.h"
#include "RInitDeinit.h"

#include <Util/CoutBuf.hpp>
#include <Util/StrColour.hpp>

#include <exception>

using namespace std;
using namespace EnjoLib;

int main(int argc, char ** argv)
{
    try
    {
        RInitDeinit rinit;
        const CLISol cli;
        const EnjoLib::Result<CLIResultSol> & result = cli.GetConfigs(argc, argv);

        if (not result.isSuccess)
        {
            return 0;
        }
        App().Run(result.value);
    }
    catch (exception & ex)
    {
        LOGL << StrColour::GenErr(ex.what()) << Nl;
        return 1;
    }
    return 0;
}
