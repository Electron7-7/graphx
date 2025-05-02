#ifndef GRAPHX_DEVICE
#include "t_settings.hpp"
#define GRAPHX_DEVICE

#ifdef COMPILER_FORWARD_DECLARATIONS // This is to keep forward declarations from causing issues when including header files
struct Theatre;
#endif

struct Device
{
public:
    // The constructor that should be used 99% of the time
    Device(const std::string& Name);

    // The constructor that the Interpreter uses when creating Devices
    Device(Theatre* ParentTheatre, const int UID, const gSettings& Settings = gSettings());

    virtual ~Device();

    int getUID() const;
    void setUID(const int NewUID);
    std::string getName() const;
    void setName(const std::string& NewName);
    gSettings getSettings() const;
    void setSettings(const gSettings&);

    virtual void loadSettings();

protected:
    Theatre* parent_theatre = nullptr;
    gSettings settings = gSettings();

private:
    int UID = -1;
    std::string name = "Untitled Device";
};

namespace graphx
{
    namespace safety
    {
        extern Device device;
    }
}
#endif