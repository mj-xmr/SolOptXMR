#ifndef CONFIGSOL_H
#define CONFIGSOL_H

#include "ConfigBase.h"
#include <Util/Str.hpp>

enum class PredictorType;
enum class PriceType;

class ConfigSol : public ConfigBase
{
    public:
        ConfigSol();
        virtual ~ConfigSol();

        void RegisterAndReadBools (EnjoLib::Istream & ifs) override;
        void RegisterAndReadInts  (EnjoLib::Istream & ifs) override;
        void RegisterAndReadFloats(EnjoLib::Istream & ifs) override;
        void RegisterAndReadStrs  (EnjoLib::Istream & ifs) override;

        void FromOptsNumeric(const OptionsNumeric & optsNum) override;


        void UpdateFromOther(const ConfigSol & cfgTSCmdLine);

        long int DAYS_HORIZON = 3; constexpr static const char * DESCR_DAYS_HORIZON = "How many days ahead you'd like to predict";
        long int DAYS_START = 0;   constexpr static const char * DESCR_DAYS_START   = "The starting day";

    protected:
        EnjoLib::Str GetFileNameProt() const override;

    private:
};

#endif // CONFIGTS_H
