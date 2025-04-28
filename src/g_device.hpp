#ifndef GRAPHX_DEVICE
#include "graphx_namespace.hpp"
#define GRAPHX_DEVICE

struct Device
{
public:
    std::string name = "Untitled Device";

    // The constructor that should be used 99% of the time
    explicit Device(const std::string& Name);

    // The constructor that the Interpreter uses when creating Devices
    explicit Device(Theatre* ParentTheatre, const int UID, const graphx::gSettings& Settings = graphx::gSettings());

    virtual ~Device();

    int getUID() const;
    void setUID(const int);
    graphx::gSettings getSettings() const;
    void setSettings(const graphx::gSettings&);

    virtual void loadSettings();

protected:
    Theatre* parent_theatre = nullptr;
    graphx::gSettings settings = graphx::gSettings();

private:
    int UID = -1;
};
#endif