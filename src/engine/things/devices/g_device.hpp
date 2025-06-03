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
    Device(Theatre* ParentTheatre, const int UID, const gSettings& Settings = gSettings());

    virtual ~Device();

    int getUID() const;
    void setUID(int NewUID);
    gSettings getSettings() const;
    void setSettings(const gSettings&);

    virtual void initialize();
    virtual void loadSettings();
    virtual void prepForDestruction();

protected:
    Theatre* parent_theatre = nullptr;
    gSettings settings = gSettings();
    bool ready_to_destroy = false;

private:
    int device_uid = -1; // A UID of -1 means it's not been set yet
};
#endif