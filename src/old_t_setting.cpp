//---------
// gSetting
//---------
gSetting::gSetting()
: category(UNDEFINED)
{}

gSetting::gSetting(const gRawData& raw_data_setting, const unsigned int new_category)
: category(new_category), raw_data(raw_data_setting)
{}

gSetting::gSetting(std::any cpp_reference_setting, const bool setting_is_cpp_reference, const unsigned int new_category)
: category(new_category), cpp_reference(cpp_reference_setting)
{}

gSetting::gSetting(std::shared_ptr<Actor> actor_reference_setting, const unsigned int new_category)
: category(new_category), theatre_reference_actor(actor_reference_setting)
{}

gSetting::gSetting(std::shared_ptr<Device> device_reference_setting, const unsigned int new_category)
: category(new_category), theatre_reference_device(device_reference_setting)
{}

gSetting::gSetting(const std::string& external_reference_setting, const unsigned int new_category)
: category(new_category), external_reference_path(external_reference_setting)
{}

unsigned int gSetting::getCategory() const
{ return category; }

int gSetting::getRawDataGLM(auto& variable, const unsigned int size) const
{
    gRawData raw_data_copy = raw_data; // because function is "const"
    raw_data_copy.resize(size);

    if(raw_data.size() == 1)
        raw_data_copy.assign(size, raw_data[0]); // The most basic form of swizzling; this is the extent of my swizzling support

    for(int i = 0; i < size; i++)
    { variable[i] = tryConvertStringToNumber(raw_data_copy[i]); }
}

double gSetting::tryConvertStringToNumber(const std::string& str, const bool use_double_instead) const
{
    if(use_double_instead)
    {
        double valid_double = 0.0;

        try
        {
            valid_double = std::stod(str);
        }

        catch(std::invalid_argument const& exception)
        {
            PRINTERR("in gSetting::getRawDataGLM at std::stod: std::invalid_argument " << exception.what())
            valid_double = 0.0; // Just to be safe...
        }

        return valid_double;
    }

    float valid_float = 0.0f;

    try
    {
        valid_float = std::stof(str);
    }

    catch(std::invalid_argument const& exception)
    {
        PRINTERR("in gSetting::getRawDataGLM at std::stof: std::invalid_argument: " << exception.what())
        valid_float = 0.0f; // Just to be safe...
    }

    return valid_float;
}