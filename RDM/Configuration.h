#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <iostream>
#include <string>
#include <map>

namespace CONFIG
{
    enum ConfigValue
    {
        BROADCAST_IP = 0,
        BROADCAST_PORT,
        METADATA_XML,
        LAYOUT_FILE_NAME
    };

    static const std::string ConfigNames[] =
    {
        "BROADCAST_IP",
        "BROADCAST_PORT",
        "METADATA_XML",
        "LAYOUT_FILE_NAME"
    };
}

class Configuration
{
private:
    static Configuration* instance;
    const std::string file_name;
    std::map<CONFIG::ConfigValue, std::string> config_values;
    static int CONFIG_ELEMENTS;

    Configuration();
    Configuration & operator=(const Configuration& copyVal);
    Configuration(const Configuration& copyVal);
    ~Configuration();

public:
    static Configuration* getInstance();

    std::string getValue(CONFIG::ConfigValue type);
    bool setValue(CONFIG::ConfigValue type,
                  std::string value,
                  bool persistent=true);
    bool loadConfigFile(std::string fileName);
    bool saveConfig();

    // helper function
    static std::string getTagValue(std::string fromStr, std::string tag);
};

#endif // CONFIGURATION_H
