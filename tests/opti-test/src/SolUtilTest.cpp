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
    CHECK_CLOSE(amplitude, sol.Max(), 0.1);

    LOG << "Generated " << days << " days. Max = " << sol.Max() << "\n";
    LOG << AsciiPlot::Build()
    (AsciiPlot::Pars::MULTILINE, true)
    (AsciiPlot::Pars::MAXIMUM, sol.Max())
    .Finalize().Plot(sol) << Nl;
}

TEST(SolUtil_round)
{
    ELO
    const double exp = 10;
    const double got = sut.round(10.1);
    CHECK_EQUAL(exp, got);
}

TEST(SolUtil_round_2)
{
    ELO
    const double exp = 10.01;
    const double got = sut.round(10.009, 2);
    CHECK_CLOSE(exp, got, 0.1);
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
