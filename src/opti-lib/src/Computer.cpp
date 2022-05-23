#include "Computer.h"

#include <Ios/Osstream.hpp>

using namespace EnjoLib;

Computer::Computer(){}
Computer::~Computer(){}

double Computer::GetHashRate(double freqGhz) const
{
    double hashes = hashPerCore * cores * freqGhz; // minus scaling factor
    return hashes;
}
double Computer::GetUsage(double freqGhz) const
{
    const double idle = freqGhz > 0 ? wattIdle : 0;
    return cores * freqGhz * wattPerCore + idle;
}

EnjoLib::Str Computer::Print() const
{
    Osstream oss;

    oss << "---------" << Nl;
    //oss << name << Nl;
    oss << name << " (" << cores << ")" << Nl;
    oss << wattPerCore << "\tWatts  per core -> " << cores * wattPerCore << "\tWatts  max." << Nl;
    oss << hashPerCore << "\tHashes per core -> " << cores * hashPerCore << "\tHashes max." << Nl; /// TODO: Mul by scaling factor!

    return oss.str();
}
