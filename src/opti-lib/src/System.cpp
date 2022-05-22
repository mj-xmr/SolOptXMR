#include "System.h"

#include <Ios/Osstream.hpp>

using namespace EnjoLib;

System::System(){}
System::~System(){}

EnjoLib::Str System::Print() const
{
    Osstream oss;

    oss << "---------" << Nl;
    //oss << name << Nl;
    oss << voltage << "V " << type << Nl;

    return oss.str();
}
