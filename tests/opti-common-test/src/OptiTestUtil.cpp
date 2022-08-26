#include "OptiTestUtil.h"
#include "Computer.h"

using namespace EnjoLib;

OptiTestUtil::OptiTestUtil(){}
OptiTestUtil::~OptiTestUtil(){}

Str OptiTestUtil::compSched_macAddr = "DE:AD:BE:EF:AA:BB";
Str OptiTestUtil::compSched_hostname = "Miner_2049er";

Computer OptiTestUtil::GetCompTestSched() const
{
    Computer compTest;
    compTest.macAddr = compSched_macAddr;
    compTest.hostname = compSched_hostname;

    return compTest;
}

    
