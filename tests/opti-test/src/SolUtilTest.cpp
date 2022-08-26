#include "SolUtil.h"
#include <Util/CoutBuf.hpp>
#include <Visual/Ascii.hpp>

#include <UnitTest++/UnitTest++.h>

using namespace EnjoLib;

static const SolUtil sut;

TEST(SolUtil_placeholder)
{

}

TEST(SolUtil_genSolar)
{
    ELO
    const int days = 3;
    const double amplitude = 10;
    const VecD sol = sut.GenSolar(days, amplitude);
    CHECK_EQUAL(days * 24, sol.size());
    CHECK(sol.Min() >= 0);
    CHECK(sol.Max() <= amplitude);
    
    LOG << "Generated " << days << " days:\n";
    LOG << AsciiPlot::Build()(AsciiPlot::Pars::MULTILINE, true).Finalize().Plot(sol) << Nl;
}

/*
TEST(SolUtil_factorial)
{
    const SolUtil sut;
    CHECK_EQUAL(120,    sut.GetFactorial(5));
    CHECK_EQUAL(1,      sut.GetFactorial(1));
    CHECK_EQUAL(1,      sut.GetFactorial(0));
}

TEST(SolUtil_factorial_large)
{
    const SolUtil sut;
    //CHECK_EQUAL(8.065817517e67L,    sut.GetFactorial(52));
}

TEST(SolUtil_combinations)
{
    const SolUtil sut;
    //CHECK_EQUAL(635013559600,    sut.GetCombinations(52, 13));
}
*/
