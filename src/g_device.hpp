#ifndef GRAPHX_DEVICE
#include "graphx_namespace.hpp"
#include "graphx_classes.hpp"
#define GRAPHX_DEVICE
struct Device
{
public:
    const graphx::gClass& type = graphx::classes::DEVICE;

    Device(const graphx::gClass&, const graphx::gID& = graphx::gID("Untitled Device"), const graphx::gSettings& = graphx::gSettings());
    virtual ~Device();

    void setUID(const int);
    void setName(const std::string&);
    graphx::gID getID() const;

    virtual void loadSettings();

    graphx::gSettings getSettings() const;
    void setSettings(const graphx::gSettings&);

private:
    graphx::gID name_and_uid = graphx::gID(-1, "Untitled Device"); // A UID of -1 means it's not been set yet

protected:
    graphx::gSettings settings = graphx::gSettings();
};
#endif