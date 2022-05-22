#include "SolUtil.h"

#include <Ios/Osstream.hpp>
#include <Util/CharManipulations.hpp>

#include <iostream>
#include <iomanip>
#include <chrono>
#include <ctime>

SolUtil::SolUtil(){}
SolUtil::~SolUtil(){}

int SolUtil::GetCurrentHour() const
{
    std::time_t t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::tm ltime;
    localtime_r(&t, &ltime);
    EnjoLib::Osstream oss;
    oss.OStr() << std::put_time(&ltime, "%H");
    return EnjoLib::CharManipulations().ToInt(oss.str());
}
