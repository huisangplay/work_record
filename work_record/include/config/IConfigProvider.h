#pragma once
#include <string>
#include <memory>
#include <vector>
#include <nlohmann/json.hpp>

class IConfigProvider {
public:
    virtual ~IConfigProvider() = default;
    virtual bool load() = 0;
    virtual bool save() = 0;
    virtual nlohmann::json getConfig() const = 0;
    virtual std::string getString(const std::string& key, const std::string& defaultValue = "") const = 0;
    virtual int getInt(const std::string& key, int defaultValue = 0) const = 0;
    virtual bool getBool(const std::string& key, bool defaultValue = false) const = 0;
    virtual double getDouble(const std::string& key, double defaultValue = 0.0) const = 0;
    virtual nlohmann::json getObject(const std::string& key) const = 0;
    virtual std::vector<std::string> getStringArray(const std::string& key) const = 0;
};

using IConfigProviderPtr = std::shared_ptr<IConfigProvider>; 