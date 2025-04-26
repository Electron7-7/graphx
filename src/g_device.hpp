#ifndef GRAPHX_DEVICE
#include "graphx_namespace.hpp"
#define GRAPHX_DEVICE

struct DevicePointerWrapper
{
    Device* pointer;
    bool owned_by_me;

    DevicePointerWrapper(Device*, const bool);
};

struct Device
{
public:
    std::string name = "Untitled Device";

    // The constructor that should be used 99% of the time
    Device(const std::string& Name = "Untitled Device");

    // The constructor that a Theatre uses when creating Devices
    Device(Theatre* ParentTheatre, const int UID, const graphx::gSettings& Settings = graphx::gSettings());

    virtual ~Device();

    void setUID(const int);
    int getUID() const;

    virtual void loadSettings();

    graphx::gSettings getSettings() const;
    void setSettings(const graphx::gSettings&);

protected:
    graphx::gSettings settings = graphx::gSettings();

private:
    int UID = -1;
    Theatre* parent_theatre = nullptr;
};
#endif