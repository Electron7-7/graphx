#ifndef GRAPHX_ACTOR
#define GRAPHX_ACTOR
#include "graphx_namespace.hpp"
#include "r_common.hpp"
#include "g_devices.hpp"
#include "t_settings.hpp"
#include <glfw_fwd.hpp>
#include <mutex>
#include <vector>

#ifdef COMPILER_FORWARD_DECLARATIONS // This is to keep forward declarations from causing issues when including header files
struct Theatre;
#endif

extern glm::vec3 vector3_up;
extern glm::vec3 vector3_front;
extern glm::vec3 vector3_right;

class Actor
{
public:
    std::string name = "Untitled Actor";
    bool visible = true;

    Mesh* mesh = nullptr; // replace with std::vector<Mesh *> meshes later(?)

    glm::vec3 scale = glm::vec3(1.0f);

    glm::vec3 orientation_front = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 orientation_grounded_front;
    glm::vec3 orientation_up;
    glm::vec3 orientation_right;
    glm::vec3 world_orientation_up = glm::vec3(0.0f, 1.0f, 0.0f);

    // Actor(std::string = "Untitled Actor", Mesh* = nullptr, glm::vec3 = glm::vec3(0.0f), glm::vec3 = glm::vec3(0.0f), glm::vec3 = glm::vec3(1.0f));
    Actor(const std::string& Name);
    Actor(Theatre* ParentTheatre, const int UID, const gSettings& Settings = gSettings()); // Note: ParentTheatre is unused for now

    virtual ~Actor();

    long getUID() const;
    void setUID(const int NewUID);
    gSettings getSettings() const;
    void setSettings(const gSettings& NewSettings);
    void updateStates(std::mutex&);

    void highlightMe();
    void unHighlightMe();

    template<typename T> T getPosition();
    template<typename T> T getRotation();
    template<typename T> T getRotationDegrees();

    // Todo: convert these to function templates
    virtual void setGlobalPosition(glm::vec3);
    virtual void setGlobalPosition(JPH::Vec3);
    virtual void setGlobalRotation(glm::vec3);
    virtual void setGlobalRotation(glm::quat);
    virtual void setGlobalRotation(JPH::Vec3);
    virtual void setGlobalRotation(JPH::Quat);
    virtual void setLocalPosition(glm::vec3);
    virtual void setLocalPosition(JPH::Vec3);
    virtual void setLocalRotation(glm::vec3);
    virtual void setLocalRotation(glm::quat);
    virtual void setLocalRotation(JPH::Vec3);
    virtual void setLocalRotation(JPH::Quat);

    virtual RenderCommands getRenderCommands();
    virtual bool isPhysicsActor();
    virtual void youGotACallBack(); // Loads settings
    virtual void callToStage(Theatre*);
    virtual void takeABow();
    virtual void processMouse(GLFWwindow*, double, double);
    virtual void processInput(GLFWwindow*);
    virtual void processKey(GLFWwindow*, int, int, int, int);
    virtual void tick(int);

protected:
    friend gSettings;
    Theatre* parent_theatre;
    gSettings settings = gSettings();

    std::vector<RenderState> current_state_buffer =  { RenderState(), RenderState() };
    std::vector<RenderState> previous_state_buffer = { RenderState(), RenderState() };
    int state_index = 0;

    glm::vec3 position_global = glm::vec3(0.0f);
    glm::vec3 position_local = glm::vec3(0.0f);
    glm::quat quaternion = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    glm::quat local_quaternion = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    glm::vec4 debug_highlight_color = glm::vec4(0.0f);

    virtual void updateVectors();

private:
    int actor_uid = -1;
};

#endif