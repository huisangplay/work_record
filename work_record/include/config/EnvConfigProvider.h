#pragma once
#include "IConfigProvider.h"
#include <map>
#include <cstdlib>

class EnvConfigProvider : public IConfigProvider {
private:
    std::map<std::string, std::string> envVars_;
    std::string prefix_;

public:
    explicit EnvConfigProvider(const std::string& prefix = "WORK_RECORD_");
    
    bool load() override;
    bool save() override;
    nlohmann::json getConfig() const override;
    
    std::string getString(const std::string& key, const std::string& defaultValue = "") const override;
    int getInt(const std::string& key, int defaultValue = 0) const override;
    bool getBool(const std::string& key, bool defaultValue = false) const override;
    double getDouble(const std::string& key, double defaultValue = 0.0) const override;
    nlohmann::json getObject(const std::string& key) const override;
    std::vector<std::string> getStringArray(const std::string& key) const override;

private:
    std::string getEnvKey(const std::string& key) const;
    std::string toUpper(const std::string& str) const;
    std::string toLower(const std::string& str) const;
    std::vector<std::string> splitKey(const std::string& key) const;
}; 