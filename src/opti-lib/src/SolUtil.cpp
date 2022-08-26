#include "SolUtil.h"

#include <Ios/Osstream.hpp>
#include <Util/CoutBuf.hpp>
#include <Util/StrColour.hpp>
#include <Util/Time.hpp>
#include <Util/CharManipulations.hpp>
#include <Math/GeneralMath.hpp>
#include <Math/Constants.hpp>
#include <Statistical/Assertions.hpp>


using namespace EnjoLib;

SolUtil::SolUtil(){}
SolUtil::~SolUtil(){}

EnjoLib::Str SolUtil::GetT() const
{
    return  "|< " + StrColour::GenNorm(StrColour::Col::Cyan, Time().CurrentDateTime()) + " >| ";
}

EnjoLib::VecD SolUtil::GenSolar(int days, double aplitude) const
{
    EnjoLib::VecD aplitudes;
    for (int i = 0; i < days; ++i)
    {
        aplitudes.Add(aplitude);
    }
    return GenSolar(aplitudes);
}

EnjoLib::VecD SolUtil::GenSolar(const EnjoLib::VecD & aplitudes) const
{
    EnjoLib::VecD ret;
    const GeneralMath gmat;
    const int period = 24;
    for (const double & apml : aplitudes)
    {
        for (int i = 0; i < 24; ++i)
        {
            double x = i / double(period);
            double y = gmat.Sin(x * 2 * PI - PI * 0.5);
            if (y < 0)
            {
                y = 0;
            }
            ret.Add(y);
        }
    }
    
    
    return ret;
}
	
/*
mpz_class SolUtil::GetFactorial(int inp) const
{
    mpz_class prod = 1;
    for (int i = 1; i <= inp; ++i)
    {
        prod *= i;
    }
    return prod;
}
mpz_class SolUtil::GetPermutations(int available) const
{
    return 0;
}
mpz_class SolUtil::GetCombinations(int available, int choices) const
{
    // n! / (r! (n -- r)!)
    const int diff = available - choices;
    Assertions::IsTrue(diff >= 0, "available < choices in GetCombinations");
    const mpz_class nfact = GetFactorial(available);
    const mpz_class rfact = GetFactorial(choices);
    const mpz_class dfact = GetFactorial(diff);
    const mpz_class sub =  rfact * dfact;
    //LOGL << "diff = " << diff << ", sub = " << sub << ", nfact = " << nfact << ", rfact = " << rfact << ", dfact = " << dfact << Nl;
    Assertions::IsTrue(sub > 0, "sub <= 0 in GetCombinations");
    const mpz_class ret = nfact / rfact / dfact;
    return ret;
}
*/
