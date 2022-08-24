#include "SolUtil.h"

#include <Ios/Osstream.hpp>
#include <Util/CoutBuf.hpp>
#include <Util/StrColour.hpp>
#include <Util/Time.hpp>
#include <Util/CharManipulations.hpp>
#include <Statistical/Assertions.hpp>


using namespace EnjoLib;

SolUtil::SolUtil(){}
SolUtil::~SolUtil(){}

EnjoLib::Str SolUtil::GetT() const
{
    return  " |- " + StrColour::GenNorm(StrColour::Col::Cyan, Time().CurrentDateTime()) + " -| ";
}
