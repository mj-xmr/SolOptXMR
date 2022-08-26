#include "OptimizerEnProfit.h" /// TODO: Use a smaller header after the extraction
//#include "ConfigSol.h"
#include "OptiTestUtil.h"
#include "Computer.h"

#include <Util/CoutBuf.hpp>

#include <UnitTest++/UnitTest++.h>

using namespace EnjoLib;

TEST(EdgeSol_1)
{
    const Computer & comp0 = OptiTestUtil().GetCompTestSched();

    //JsonReader().ReadComputers();
    CHECK(true);
}
