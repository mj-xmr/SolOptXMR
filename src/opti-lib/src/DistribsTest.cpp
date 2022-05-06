#include "DistribsTest.h"

#include <Statistical/StdDev.hpp>
#include <boost/math/distributions/normal.hpp>

using namespace boost::math;

DistribsTest::DistribsTest()
{
}

double DistribsTest::NormalPDF(double mean, const EnjoLib::StdDev & stdDev, double arg) const
{
    auto d = normal_distribution<>(mean, stdDev.GetValue());
    return pdf(d, arg);
}
