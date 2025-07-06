#pragma once
#include "IConfigProvider.h"
#include <fstream>
#include <filesystem>
#include <sstream>

class JsonConfigProvider : public IConfigProvider {
private:
    std::string configPath_;
    nlohmann::json config_;
    bool loaded_;

public:
    explicit JsonConfigProvider(const std::string& configPath);
    
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
    nlohmann::json getValue(const std::string& key) const;
    std::vector<std::string> splitKey(const std::string& key) const;
}; 