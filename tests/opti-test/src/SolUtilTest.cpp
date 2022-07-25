#include "SolUtil.h"
#include <Util/CoutBuf.hpp>

#include <UnitTest++/UnitTest++.h>

using namespace EnjoLib;

TEST(SolUtil_values_to_ascii1)
{
    const double minimum = 0;
    const double maximum = 10;
    VecD inp;
    for (int i = 1; i >= -2; --i)
    {
        inp.Add(i);
    }
    for (int i = 0; i <= 12; ++i)
    {
        inp.Add(i);
    }
    SolUtil().GetPercentToAscii(inp, minimum, maximum);
    LOGL << "values_to_ascii1:" << Nl << SolUtil().GetPercentToAscii(inp, minimum, maximum) << Nl;
}


