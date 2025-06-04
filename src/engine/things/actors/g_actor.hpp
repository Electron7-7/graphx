#ifndef GRAPHX_ACTOR
#define GRAPHX_ACTOR
#include "engine/common.hpp"
#include "engine/rendering/r_common.hpp"
#include "engine/things/devices/g_devices.hpp"
#include "engine/things/settings.hpp"
#include "glfw_fwd.hpp"
#include <glm/glm.hpp>
#include <mutex>
#include <array>

#ifdef COMPILER_FORWARD_DECLARATIONS // This is to keep forward declarations from causing issues when including header files
struct Theatre;
#endif

class Actor
{
public:
    std::string name = "Untitled Actor";

    bool visible = true;
    bool debug_highlight_enabled = false;

    Mesh* mesh = &graphx::missing::mesh; // replace with std::vector<Mesh *> meshes later(?)

    Actor(const std::string& Name);
    Actor(Theatre* ParentTheatre, const int UID, const gSettings& Settings = gSettings()); // Note: ParentTheatre is unused for now

    virtual ~Actor();

    int getUID() const;
    void setUID(const int NewUID);
    gSettings getSettings() const;
    void setSettings(const gSettings& NewSettings);
    void updateStates(std::mutex&);

    glm::vec3 getAbsolutePosition() const;
    glm::quat getAbsoluteQuaternion() const;
    glm::vec3 getAbsoluteEulerAngles(const bool AsDegrees = false) const;
    glm::vec3 getAbsoluteScale() const;

    void setGlobalPosition(const glm::vec3& Position);
    void setGlobalQuaternion(const glm::quat& Quaternion);
    void setGlobalEulerAngles(const glm::vec3& EulerAngles, const bool UseDegrees = false);
    void setGlobalPitch(const float Pitch, const bool UseDegrees = false);
    void setGlobalYaw(const float Yaw, const bool UseDegrees = false);
    void setGlobalRoll(const float Roll, const bool UseDegrees = false);
    void setGlobalScale(const glm::vec3& Scale);

    void setLocalPosition(const glm::vec3& Position);
    void setLocalQuaternion(const glm::quat& Quaternion);
    void setLocalEulerAngles(const glm::vec3& EulerAngles, const bool UseDegrees = false);
    void setLocalPitch(const float Pitch, const bool UseDegrees = false);
    void setLocalYaw(const float Yaw, const bool UseDegrees = false);
    void setLocalRoll(const float Roll, const bool UseDegrees = false);
    void setLocalScale(const glm::vec3& Scale);

    glm::vec3 getOrientationUp(const bool Global = true) const;
    glm::vec3 getOrientationFront(const bool Global = true) const;
    glm::vec3 getOrientationRight(const bool Global = true) const;

    glm::vec3 getGlobalPosition() const;
    glm::quat getGlobalQuaternion() const;
    glm::vec3 getGlobalEulerAngles(const bool AsDegrees = false) const;
    glm::vec3 getGlobalScale() const;

    glm::vec3 getLocalPosition() const;
    glm::quat getLocalQuaternion() const;
    glm::vec3 getLocalEulerAngles(const bool AsDegrees = false) const;
    glm::vec3 getLocalScale() const;

    virtual void tick(const int CurrentTick = -1);
    virtual void loadSettings();
    virtual RenderCommands getRenderCommands();
    virtual void processMouse(GLFWwindow*, double, double);
    virtual void processKey(GLFWwindow*, int, int, int, int);
    virtual void checkForInput(GLFWwindow* window);
    virtual bool isPhysicsActor() const;
    virtual void callToStage(Theatre*);
    virtual void takeABow();

    // Only used for debugging purposes; will return "Actor" if not implemented
    virtual std::string getTypeName() const;

protected:
    Theatre* parent_theatre;
    gSettings settings = gSettings();

    std::array<RenderState, 2> current_state_buffer =  { RenderState(), RenderState() };
    std::array<RenderState, 2> previous_state_buffer = { RenderState(), RenderState() };
    int state_index = 0;

    glm::vec4 debug_highlight_color = glm::vec4(0.0f);

    // void selfOverrideColliderTransform(const bool IgnoreScale = true);
    // void colliderOverrideSelfTransform(const bool IgnoreScale = true);
    virtual bool canBeRendered() const;

private:
    int actor_uid = -1;

    glm::vec3 position_global = glm::vec3(0.0f);
    glm::vec3 position_local = glm::vec3(0.0f);
    glm::quat quaternion_global = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    glm::quat quaternion_local = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    glm::vec3 scale_global = glm::vec3(1.0f);
    glm::vec3 scale_local = glm::vec3(1.0f);
};

#endif