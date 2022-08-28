//#include "pch_test.h"
#include <Util/CoutBuf.hpp>
#include <UnitTest++/UnitTest++.h>

using namespace std;

int main()
{
    LOGL << "Running tests!" << EnjoLib::Nl;
    return UnitTest::RunAllTests();
}
