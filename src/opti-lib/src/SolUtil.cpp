#include "SolUtil.h"

#include <Ios/Osstream.hpp>
#include <Util/CoutBuf.hpp>
#include <Util/CharManipulations.hpp>
#include <Util/CharManipulations.hpp>
#include <Statistical/Assertions.hpp>


using namespace EnjoLib;

SolUtil::SolUtil(){}
SolUtil::~SolUtil(){}

EnjoLib::Str SolUtil::GetPercentToAscii(double val, double minimum, double maximum)
{
    Assertions::IsTrue(maximum > minimum, "maximum < minimum GetPercentToAscii");
    const double diff = maximum - minimum;
    Assertions::IsNonZero(diff, "diff GetPercentToAscii");
    const double pro = (val - minimum) / diff;
    //LOGL << pro << Nl;
    EnjoLib::Str ret = "1";
    
    if (pro < 0)
    {
        ret = " ";
    }
    else if (0 <= pro && pro < 0.1)
    {
        ret = "_";
    }
    else if (0.1 <= pro && pro < 0.3)
    {
        ret = ".";
    }
    else if (0.3 <= pro && pro < 0.5)
    {
        ret = "░";
    }
    else if (0.5 <= pro && pro < 0.7)
    {
        ret = "▒";
    }
    else if (0.7 <= pro && pro < 1)
    {
        ret = "▓";
    }
    else 
    {
        ret = "█";
    }
    return ret;
}
EnjoLib::Str SolUtil::GetPercentToAscii(const EnjoLib::VecD & vals, double minimum, double maximum, bool decoration)
{
    EnjoLib::Osstream oss;
    if (decoration)
    {
        oss << "├ ";
    }
    for (const double val : vals)
    {
        oss << GetPercentToAscii(val, minimum, maximum);
    }
    if (decoration)
    {
        oss << " ┤";
    }
    return oss.str();
}