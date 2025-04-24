// Note about Theatres:
// I abstracted getting Actor and Device pointers to functions, because directly grabbing them from their maps
// might return null (if using []) or crash the engine (if using .at()). This crash will appear to happen for no
// reason, so abstracting to these functions lets me put a PRINTERR in them that will print a detailed warning
// message if the function can't find the Actor/Device (before returning a nullptr). This means that getting
// Actor or Device pointers won't (directly) crash the engine, but will print a warning if it returns a nullptr.

struct Theatre
{
    Mesh *stage_mesh = nullptr;
    Material *stage_material = nullptr;
    Actor stage;
    glm::vec3 stage_scale = glm::vec3(0.0f);
    glm::vec3 stage_position = glm::vec3(0.0f);
    glm::quat stage_quaternion = glm::quat();

    std::string name = "Untitled Theatre";
    int point_lights_count = 0;
    int spot_lights_count = 0;
    int directional_lights_count = 0;
    bool dropping_curtains = false;

    // This abomination is what a "gStringSettings" typedef actually is...
    std::vector<std::vector<std::pair<std::string, std::pair<int, std::string>>>> graphx_theatre_settings;
    std::string theatre_file_data_printout = "";

    Theatre(std::string init_name = "Untitled Theatre", long new_uid = -1);

    std::vector<long> dumpActorIDs();
    std::vector<Actor*> getTroupe();

    std::set<std::string> getMeshDataNames();
    std::set<std::string> getTextureNames();
    void probeActorsForRenderCommands();
    void loadStageSettings(graphx::gSettings stage_settings);
    void raiseCurtains();
    void dropCurtains();
    long getUID();
    void setUID(long new_uid);
    void delegateKeyInput(GLFWwindow *window, int key, int scancode, int action, int mods);
    void delegateMouseInput(GLFWwindow *window, double x_position_in, double y_position_in);
    void troupeEnter(std::vector<std::pair<Actor *, long>> new_troupe);
    void actorEnter(Actor *new_actor, long uid, graphx::gSettings new_settings = empty_settings);
    void actorLeave(Actor *old_actor);
    void actorLeave(long uid);
    void placeDevice(Device *new_device, long uid, graphx::gSettings new_settings = empty_settings);
    void removeDevice(Device *old_device);
    void removeDevice(long uid);

    void createActor(graphx::gClass actor_type, long uid, graphx::gSettings new_settings = empty_settings);
    void createDevice(graphx::gClass device_type, long uid, graphx::gSettings new_settings  = empty_settings);

    std::vector<Actor *> getAllActorsOfType(graphx::gClass type_name);
    std::vector<Device *> getAllDevicesOfType(graphx::gClass type_name);

    Actor *getFirstActorOfType(graphx::gClass type_name);
    Device *getFirstDeviceOfType(graphx::gClass type_name);
    // WARNING!! THIS FUNCTION WILL RETURN A nullptr IF NO ACTOR MATCHING type_name IS FOUND!!
    Actor *unsafeGetFirstActorOfType(graphx::gClass type_name);
    // WARNING!! THIS FUNCTION WILL RETURN A nullptr IF NO DEVICE MATCHING type_name IS FOUND!!
    Device *unsafeGetFirstDeviceOfType(graphx::gClass type_name);

    Actor *getActor(long uid);
    Actor *getActor(std::string actor_name);
    Device *getDevice(long uid);
    Device *getDevice(std::string device_name);

    GraphXPlayer *getPlayer();
    Environment *getEnvironment();

private:
    std::unordered_map<long, Actor *> objects = {};
    std::unordered_map<long, Device *> devices = {};
    long UID = -1;
    long environment_uid = -1;
    long player_uid = -1;
};

// extern Theatre graphx::current::theatre;
// extern std::map<int, Actor*(*)()> actor_map;

// Use with CAUTION!!
// Wants to return static_cast<T>(graphx::current::theatre.getActor(identifier)) but if that fails, returns new std::remove_pointer_t<T>.
// Useful for getting a down-casted pointer to a known object. Performs NO safety checks, so only use this if you know both the UID/Name AND the specific sub-class of the object you're getting.
template<typename T> T iKnowWhatActorIWant(auto identifier)
{
    if(graphx::current::theatre.getUID() == -1 || graphx::current::theatre.getActor(identifier) == nullptr)
        return new std::remove_pointer_t<T>;

    return static_cast<T>(graphx::current::theatre.getActor(identifier));
}

// Use with CAUTION!!
// Wants to return static_cast<T>(graphx::current::theatre.getDevice(identifier)) but if that fails, returns new std::remove_pointer_t<T>.
// Useful for getting a down-casted pointer to a known object. Performs NO safety checks, so only use this if you know both the UID/Name AND the specific sub-class of the object you're getting.
template<typename T> T iKnowWhatDeviceIWant(auto identifier)
{
    if(graphx::current::theatre.getUID() == -1 || graphx::current::theatre.getDevice(identifier) == nullptr)
        return new std::remove_pointer_t<T>;
    return static_cast<T>(graphx::current::theatre.getDevice(identifier));
}

// template<typename T> Actor *createNewActor() { return new T; }

Theatre *getCurrentTheatre(bool print_note = true);
Environment *getCurrentEnvironment();
GraphXPlayer *getCurrentPlayer();