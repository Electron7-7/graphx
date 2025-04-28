#ifndef GRAPHX_DEVICE
#include "graphx_namespace.hpp"
#define GRAPHX_DEVICE

#ifdef COMPILER_FORWARD_DECLARATIONS // This is to keep forward declarations from causing issues when including header files
struct Theatre;
#endif

struct Device
{
public:
    std::string name = "Untitled Device";

    // The constructor that should be used 99% of the time
    Device(const std::string& Name);

    // The constructor that the Interpreter uses when creating Devices
    Device(Theatre* ParentTheatre, const int UID, const graphx::gSettings& Settings = graphx::gSettings());

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

namespace graphx
{
    namespace safety
    {
        extern Device device;
    }
}
#endif