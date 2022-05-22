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
    oss << voltage << "V\n";
    if (generating)
    {
        oss << "Generating; ";
    }
    if (buying)
    {
        oss << "Buying; ";
    }
    if (selling)
    {
        oss << "Selling; ";
    }
    

    return oss.str();
}
