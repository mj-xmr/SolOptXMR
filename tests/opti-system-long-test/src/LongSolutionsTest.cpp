#include "OptimizerEnProfit.h" /// TODO: Use a smaller header after the extraction
#include "OptiEnProfitDataModel.h"
#include "ConfigSol.h"
#include "OptiTestUtil.h"
#include "SolUtil.h"
#include "Computer.h"
#include "OptimizerEnProfit.h"

#include <Util/CoutBuf.hpp>

#include <UnitTest++/UnitTest++.h>
#include <vector>

using namespace EnjoLib;

TEST(EdgeSol_2computers)
{
    /// TODO: This one should also have a penalty of 0, since the miners are not used enough. Minimize for over- under-voltage first, then for hashrate?
    const int horizon = 2;
    const int startingPoint = 0;
    const VecD computersHashrateMul = {1, 4};
    const double amplitude = 120;
    const double batChargeAh = 70;
    const VecD genPower = SolUtil().GenSolar(horizon, amplitude);


    auto builder = OptiTestUtilConf::Build()
    (OptiTestUtilConf::Pars::BATTERY_CHARGE, batChargeAh)
    (OptiTestUtilConf::Pars::NO_SCHEDULE, false);
    const OptimizerEnProfit & opti = builder.Finalize().TestEdgeSolGetOptimizer(genPower, horizon, startingPoint, computersHashrateMul);
    CHECK(opti.GetHashes() > 0);
    //CHECK_EQUAL(0, opti.GetPenality());
    CHECK(opti.GetPenality() > 0);
    CHECK(opti.GetPenality() < 3500);
}

