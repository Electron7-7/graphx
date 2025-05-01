struct gSetting
{
public:
    static constexpr unsigned int UNDEFINED          = 0;
    static constexpr unsigned int RAW_DATA           = 1; //< Identifies a vector of strings, using the typedef `gRawData`.
    static constexpr unsigned int CPP_REFERENCE      = 2; //< Identifies a C++ reference (see `cpp_definitions` in `t_interpreter.cpp`).
    static constexpr unsigned int EXTERNAL_REFERENCE = 3; //< Identifies an external file's pathname, passed via string.
    static constexpr unsigned int THEATRE_REFERENCE  = 4; //< Identifies a pointer to a pre-existing Actor/Device in the current Theatre.
    static constexpr unsigned int SANDWICH           = 5; //< Identifies a unique copy of a pre-existing Actor/Device in the current Theatre.

    gSetting();
    gSetting(std::any CppReference, const bool SettingIsCppReference, const unsigned int CategoryOverride = CPP_REFERENCE);
    gSetting(const gRawData& RawData, const unsigned int CategoryOverride = RAW_DATA);
    gSetting(std::shared_ptr<Actor> TheatreReference, const unsigned int TheatreReferenceOrSandwichBun);
    gSetting(std::shared_ptr<Device> TheatreReference, const unsigned int TheatreReferenceOrSandwichBun);
    gSetting(const std::string& ExternalReference, const unsigned int CategoryOverride = EXTERNAL_REFERENCE);

    unsigned int getCategory() const;

    // Default specialization: arithmatic scalar or glm vector/quaternion
    template<typename T> int getRawData(T& variable) const
    {
        if constexpr(std::is_same_v<T, glm::vec2> || std::is_same_v<T, glm::vec3> || std::is_same_v<T, glm::vec4> || std::is_same_v<T, glm::quat> || std::is_arithmetic_v<T>)
        {
            if constexpr(std::is_arithmetic_v<T>)
            { variable = tryConvertStringToNumber(raw_data[0], true); return 0; } // Get double instead of float to avoid errors (that's the idea, at least)

            else if constexpr(std::is_same_v<T, glm::vec2>)
            { getRawDataGLM(variable, 2); return 0; }

            else if constexpr(std::is_same_v<T, glm::vec3>)
            { getRawDataGLM(variable, 3); return 0; }

            else if constexpr(std::is_same_v<T, glm::vec4> || std::is_same_v<T, glm::quat>)
            { getRawDataGLM(variable, 4); return 0; }

            PRINTERR("in gSetting::getRawData: unknown error occured! (default specialization, end of if-else statements)")
            return GRAB_SETTING_ERR_RAW_DATA;
        }

        PRINTERR("gSetting::getRawData(T& variable) - T is not a valid type!")
        return GRAB_SETTING_ERR_RAW_DATA;
    }

    // gSetting::getRawData - Specializations

    template<> int getRawData(bool& variable) const
    {
        // Converting raw_data to all lowercase
        std::string raw_data_lower = raw_data[0];
        std::transform(raw_data_lower.begin(), raw_data_lower.end(), raw_data_lower.begin(), [](unsigned char c)
        { return std::tolower(c); });
        // Potential shorter way of doing this (only try after everything's working):
        // for(char& character : raw_data_lower)
            // character = std::tolower(character);

        variable = raw_data_lower.compare("false");
        return 0;
    }

    template<> int getRawData(std::string& variable) const
    {
        std::string raw_data_out = "";
        for(int i = 0; i < raw_data.size(); i++)
            raw_data_out.append(raw_data[i].c_str());
        variable = raw_data_out;
        return 0;
    }

    // This one's a bit iffy...
    template<> int getRawData(const char*& variable) const
    {
        std::string raw_data_out = "";
        for(int i = 0; i < raw_data.size(); i++)
            raw_data_out.append(raw_data[i].c_str());
        variable = raw_data_out.c_str();
        return 0;
    }

    // idk when, where, or why you'd need this but here you go i guess...
    template<> int getRawData(char& variable) const
    {
        variable = (!raw_data.empty()) ? raw_data[0][0] : '0'; // just being cautious...
        return 0;
    }

    template<typename T> int getCppReference(T& variable) const
    {
        try
        {
            std::any_cast<T>(cpp_reference);
        }

        catch(std::bad_any_cast const& exception)
        {
            PRINTERR("in gSetting::getCppRefernce: std::bad_any_cast: " << exception.what())
            return GRAB_SETTING_ERR_CPP_REFERENCE;
        }

        variable = std::any_cast<T>(cpp_reference);
        return 0;
    }

    template<typename T> int getTheatreReference(T& variable) const
    {}

    template<typename T> int getTheatreReference(std::shared_ptr<T>& variable) const
    {
        static_assert(
            std::is_base_of_v<Actor, T> ||
            std::is_base_of_v<Device, T>,
            "in gSetting::getTheatreReference: invalid type! Object type must be derived from either Actor or Device!"
        );

        if constexpr(std::is_base_of_v<Actor, T>)
        {
            if(theatre_reference_actor == nullptr)
            {
                PRINTERR("in gSetting::getTheatreReference (variable type is derived from Actor): the Theatre reference is nullptr (most likely not set)!")
                return GRAB_SETTING_ERR_ACTOR_POINTER;
            }

            variable = dynamic_pointer_cast<T>(theatre_reference_actor);
        }

        else if constexpr(std::is_base_of_v<Device, T>)
        {
            if(theatre_reference_device == nullptr)
            {
                PRINTERR("in gSetting::getTheatreReference (variable type is derived from Device): the Theatre reference is nullptr (most likely not set)!")
                return GRAB_SETTING_ERR_DEVICE_POINTER;
            }

            variable = dynamic_pointer_cast<T>(theatre_reference_device);
        }
    }

    template<typename T> int getExternalReference(T& variable) const
    {
        static_assert(
            std::is_same_v<T, std::string>           ||
            std::is_same_v<T, std::filesystem::path>,
            "in gSetting::getExternalReference: variable type is not valid! (must be std::filesystem::path or std::string)"
        );

        variable = T(external_reference_path);
    }

private:
    unsigned int category = UNDEFINED;
    gRawData raw_data = {""};
    std::any cpp_reference = std::any();
    std::shared_ptr<Actor> theatre_reference_actor = nullptr;
    std::shared_ptr<Device> theatre_reference_device = nullptr;
    std::string external_reference_path = "";

    double tryConvertStringToNumber(const std::string&, const bool = false) const;
    template<typename T> int getRawDataGLM(T& variable, const unsigned int size) const;
};

typedef std::unordered_map<std::string, gSetting> gSettings;

int getSetting(auto& variable, const gSetting& setting)
{
    switch(setting.getCategory())
    {
    case gSetting::RAW_DATA:
        setting.getRawData(variable);
        return 0;
    case gSetting::CPP_REFERENCE:
        setting.getCppReference(variable);
        return 0;
    case gSetting::THEATRE_REFERENCE:
        setting.getTheatreReference(variable);
        return 0;
    case gSetting::SANDWICH:
        setting.getTheatreReference(variable);
        return 0;
    }

    PRINTERR("in getSetting: unknown/undefined setting type!")
    return -1;
}