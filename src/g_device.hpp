#ifndef GRAPHX_DEVICE
#include "t_settings.hpp"
#define GRAPHX_DEVICE

#ifdef COMPILER_FORWARD_DECLARATIONS // This is to keep forward declarations from causing issues when including header files
struct Theatre;
#endif

struct Device
{
    std::string name = "Untitled Device";

    // The constructor that should be used 99% of the time
    Device(const std::string& Name);

    // The constructor that the Interpreter uses when creating Devices
    Device(Theatre* ParentTheatre, const int UID, const graphx::gSettings& Settings = empty_settings);

    virtual ~Device();

    int getUID() const;
    void setUID(int NewUID);
    graphx::gSettings getSettings() const;
    void setSettings(const graphx::gSettings&);

    virtual void initialize();
    virtual void loadSettings(graphx::gSettings new_settings = empty_settings);
    virtual void prepForDestruction();

protected:
    Theatre* parent_theatre = nullptr;
    graphx::gSettings settings = empty_settings;
    bool ready_to_destroy = false;

private:
    int device_uid = -1; // A UID of -1 means it's not been set yet
};
#endif