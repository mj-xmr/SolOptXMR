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
        long int RANDOM_SEED = 0;  constexpr static const char * DESCR_RANDOM_SEED  = "Random seed. Use any integer number to 'fixate' the random solutions";
        
        //long int SYSTEM_VOLTAGE = 12;   constexpr static const char * DESCR_SYSTEM_VOLTAGE   = "System voltage: 12 / 24";
        float BATTERY_CHARGE = 0;  constexpr static const char * DESCR_BATTERY_CHARGE = "Initial battery charge in Ah";
        float HASHRATE_BONUS = 0;  constexpr static const char * DESCR_HASHRATE_BONUS = "Hashrate bonus for the current hour";
        
        //EnjoLib::Str SYSTEM_TYPE = "island"; constexpr static const char * DESCR_SYSTEM_TYPE = "System type: island / gen+buy / gen+buy+sell / gen+sell";
        

    protected:
        EnjoLib::Str GetFileNameProt() const override;

    private:
};

#endif // CONFIGTS_H
