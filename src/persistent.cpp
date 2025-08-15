/*
    LightBaker3000 Frontend project,
    (c) 2022 CrazyRussian
*/

#include "common.h"
#include "application.h"
#include "persistent.h"

#include "fs_core.h"
#include "variant.h"

#include "ui_options_pages.h"

PersistentStorage::PersistentStorage(Application *appInstance)
{
    extern void RegisterOptions();
    extern ProgramOptions::uiOptionPage_t g_OptionsPages[(int)ProgramOptions::OptionsPage::Total];

    RegisterOptions();


    // TODO: calc this
    m_ApplicationProps.reserve(32);

    for (auto &it : g_OptionsPages)
    {
        for (auto &opt : it.items)
        {
            if (opt->ItemType() == ProgramOptions::OptionPageItemType::Value)
            {
                auto val = (ProgramOptions::OptionsValue *)opt;

                val->GetValue()->ValidateValue();
                m_ApplicationProps.push_back(val->GetValue());
            }
        }
    }



    SetDefaultValues();
    LoadFromFile(appInstance);
       
    // To make default game configuration avaible    
}

void PersistentStorage::SetDefaultValues()
{
    auto setting = GetSetting(ApplicationSettings::BackgroundColor1);
    setting->SetColorRGBA(glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));

    setting = GetSetting(ApplicationSettings::CameraFov);
    setting->SetFloat(110);

    setting = GetSetting(ApplicationSettings::CameraZNear);
    setting->SetFloat(1.f);

    setting = GetSetting(ApplicationSettings::CameraZFar);
    setting->SetFloat(4096.f);

    setting = GetSetting(ApplicationSettings::CameraMovementSpeed);
    setting->SetFloat(400);

    setting = GetSetting(ApplicationSettings::CameraAccel);
    setting->SetFloat(1000);

    setting = GetSetting(ApplicationSettings::CameraDecel);
    setting->SetFloat(1000);

    setting = GetSetting(ApplicationSettings::GridAxisColor);
    setting->SetColorRGB(glm::vec3{1,0,0});

    setting = GetSetting(ApplicationSettings::GridMainColor);
    setting->SetColorRGB(glm::vec3{.2f, .2f, .2f});

    setting = GetSetting(ApplicationSettings::Grid64thLineColor);
    setting->SetColorRGB(glm::vec3{.2f, .2f, .2f});

    setting = GetSetting(ApplicationSettings::Grid1024thLineColor);
    setting->SetColorRGB(glm::vec3{1, 1, 0.4f});

    setting = GetSetting(ApplicationSettings::GridCustomColor);
    setting->SetColorRGB(glm::vec3{0, 0, 0});

    setting = GetSetting(ApplicationSettings::GridCustomStep);
    setting->SetInt(0);

    setting = GetSetting(ApplicationSettings::CameraMouseSensivityRotating);
    setting->SetFloat(0.5f);

    setting = GetSetting(ApplicationSettings::CameraMouseSensivityPaning);
    setting->SetInt(1.f);

    setting = GetSetting(ApplicationSettings::CameraMouseSensivityZooming);
    setting->SetInt(1.f);
}

void PersistentStorage::LoadFromFile(Application *appInstance)
{
    char persistent_file[1024];
    char *p = SDL_GetPrefPath(SDL_ORGANIZATION, SDL_APP_NAME);
    snprintf(persistent_file, sizeof(persistent_file), "%s/persistent.json", p);

    FileData *fd = Application::Instance()->GetFileSystem()->LoadFile(persistent_file);

    if (!fd)
    {
        m_bFreshFile = true;
        return;
    }

    m_bFreshFile = false;

    try
    {
        std::string json_data = std::string(std::string_view((char *)fd->Data(), fd->Length()));
        nlohmann::json j      = nlohmann::json::parse(json_data);

        if (j.contains("MRU"))
        {
            auto &MRU = j["MRU"];

            for (auto items : MRU)
            {
                m_lstMRUFiles.push_back(mruFile_t(items[0].get<std::string>(), items[1].get<std::time_t>()));
            }

            SortMRU();
        }

        if (j.contains("ApplicationSettings"))
            ParseApplicationSettings(j["ApplicationSettings"]);

        

        // if (j.contains("PanelsState"))
        // {
        //     auto &items = j["PanelsState"];

        //     for (auto &it : items)
        //     {
        //         ToolUIPanelID id      = ToolUIPanelID::_from_string(it["Id"].get<std::string>().c_str());
        //         bool isValid = it["Valid"];

        //         m_bPanelsValid[id._value] = isValid;
        //     }
        // }

        // if (j.contains("Viewports"))
        // {
        //     nlohmann::json block = j["Viewports"];
        //     ViewportsOrchestrator::Instance()->Init(std::move(block));
        // }

    }
    catch (std::exception &e)
    {
        e;
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Exception while parsing persistent data", e.what(), 0);
        // Application::EPICFAIL(e.what());
    }

    delete fd;
}

PersistentStorage::~PersistentStorage()
{
    SaveToFile();
}

void PersistentStorage::PushMRUFile(const char *fileName)
{
    auto p = std::filesystem::path(fileName);
    auto f = std::filesystem::canonical(p);

    std::string canonicalFileName = {f.string()};

    for (auto &it : m_lstMRUFiles)
    {
        if (!strcasecmp(it.first.c_str(), canonicalFileName.c_str()))
        {
            it.second = std::time(nullptr);
            SortMRU();
            return;
        }
    }

    mruFile_t desc(fileName, std::time(nullptr));

    m_lstMRUFiles.push_front(desc);
    SortMRU();

    while (m_lstMRUFiles.size() > 10)
        m_lstMRUFiles.pop_back();
}

void PersistentStorage::SortMRU()
{
    m_lstMRUFiles.sort([](mruFile_t &a, mruFile_t &b) -> bool { return (a.second > b.second); });
}

std::list<mruFile_t> &PersistentStorage::GetMRUFiles()
{
    return m_lstMRUFiles;
}

VariantValue *PersistentStorage::GetSetting(ApplicationSettings id)
{
    for (auto &it : m_ApplicationProps)
    {
        if (it->GetId() == (int)id)
            return it;
    }

    return nullptr;
}

bool PersistentStorage::GetSettingBool(ApplicationSettings id)
{
    VariantValue *p = GetSetting(id);
    assert(p);

    return p->GetAsBool();
}

nlohmann::json PersistentStorage::SerializeApplicationProperty(nlohmann::json &j, VariantValue *it)
{
    nlohmann::json prop_descriptor;

    prop_descriptor["id"]           = it->GetId();
    prop_descriptor["display_name"] = it->DisplayName();
    prop_descriptor["type"]         = it->GetType();

    switch (it->GetType())
    {
    case PropertiesTypes::Enum:
        prop_descriptor["value"] = it->GetEnumValue();
        break;
    case PropertiesTypes::Flags:
        prop_descriptor["value"] = it->GetFlags();
        break;
    case PropertiesTypes::Position: {
        nlohmann::json field;

        auto pos = it->GetPosition();

        field["x"] = pos[0];
        field["y"] = pos[1];
        field["z"] = pos[2];

        prop_descriptor["value"] = field;
    }
    break;
    case PropertiesTypes::ColorRGB: {
        nlohmann::json field;

        auto val = it->GetColorRGB();

        field["r"] = val[0];
        field["g"] = val[1];
        field["b"] = val[2];

        prop_descriptor["value"] = field;
    }
    break;
    case PropertiesTypes::ColorRGBA: {
        nlohmann::json field;

        auto val = it->GetColorRGBA();

        field["r"] = val[0];
        field["g"] = val[1];
        field["b"] = val[2];
        field["a"] = val[3];

        prop_descriptor["value"] = field;
    }
    break;
    case PropertiesTypes::Float:
        prop_descriptor["value"] = it->GetFloat();
        break;
    case PropertiesTypes::Angles: {
        nlohmann::json field;

        auto val = it->GetAngles();

        field["x"] = val[0];
        field["y"] = val[1];
        field["z"] = val[2];

        prop_descriptor["value"] = field;
    }
    break;
    case PropertiesTypes::Int:
        prop_descriptor["value"] = it->GetInt();
        break;
    case PropertiesTypes::SizeX:
        prop_descriptor["value"] = it->GetFloat();
        break;
    case PropertiesTypes::Bool:
        prop_descriptor["value"] = it->GetAsBool();
        break;
    case PropertiesTypes::String:
        prop_descriptor["value"] = it->GetString();
        break;
    default:
        break;
    }

    auto v = prop_descriptor["value"];

    return prop_descriptor;
}

void PersistentStorage::ParseApplicationSettings(nlohmann::json j)
{
    for (auto &it : j)
    {
        int id = it["id"];

        VariantValue *descriptor = GetSetting((ApplicationSettings)id);

        if (!descriptor)
            continue;

        if (it["type"] != descriptor->GetType())
            continue;

        switch (descriptor->GetType())
        {
        case PropertiesTypes::Enum:
            // prop_descriptor["value"] = it.value.asEnum;
            descriptor->SetEnumValue(it["value"]);
            break;
        case PropertiesTypes::Flags:
            descriptor->SetFloat(it["value"]);
            break;
        case PropertiesTypes::Position: {
            nlohmann::json value = it["value"];

            glm::vec3 val;

            val[0] = value["x"];
            val[1] = value["y"];
            val[2] = value["z"];

            descriptor->SetPosition(val);
        }
        break;
        case PropertiesTypes::ColorRGB: {
            nlohmann::json value = it["value"];

            glm::vec3 val;

            val[0] = value["r"];
            val[1] = value["g"];
            val[2] = value["b"];

            descriptor->SetColorRGB(val);
        }
        break;
        case PropertiesTypes::ColorRGBA: {
            nlohmann::json value = it["value"];

            glm::vec4 val;

            val[0] = value["r"];
            val[1] = value["g"];
            val[2] = value["b"];
            val[3] = value["a"];

            descriptor->SetColorRGBA(val);
        }
        break;
        case PropertiesTypes::Float:
            // prop_descriptor["value"] = it.value.asFloat;
            descriptor->SetFloat(it["value"]);
            break;
        case PropertiesTypes::Angles: {
            nlohmann::json value = it["value"];

            glm::vec3 val;

            val[0] = value["x"];
            val[1] = value["y"];
            val[2] = value["z"];

            descriptor->SetAngles(val);
        }
        break;
        case PropertiesTypes::Int:
            descriptor->SetInt(it["value"]);
            break;
        case PropertiesTypes::SizeX:
            descriptor->SetFloat(it["value"]);
            break;
        case PropertiesTypes::Bool:
            descriptor->SetBool(it["value"]);
            break;
        case PropertiesTypes::String:
            descriptor->SetString(it["value"]);
            break;
        default:
            break;
        }

        // prop_descriptor["display_name"] = it.display_name;
        // prop_descriptor["type"] = it.type;
    }
}

// bool PersistentStorage::IsPanelAtValidPosition(ToolUIPanelID id)
// {
//     return m_bPanelsValid[id._value];
// }

// void PersistentStorage::FlagPanelIsAtValidPosition(ToolUIPanelID id)
// {
//     m_bPanelsValid[id._value] = true;
// }

bool PersistentStorage::IsFreshFile()
{
    // return true;

    return m_bFreshFile;
}

PersistentStorage *PersistentStorage::Instance()
{
    static PersistentStorage *sInstance = new PersistentStorage(Application::Instance());
    return sInstance;
}

void PersistentStorage::RemoveMRUItem(std::string &filePath)
{
    m_lstMRUFiles.remove_if([&](mruFile_t &a) { return a.first == filePath; });
    SortMRU();
}

void PersistentStorage::SaveToFile()
{
    char persistent_file[1024];
    char *p = SDL_GetPrefPath(SDL_ORGANIZATION, SDL_APP_NAME);
    snprintf(persistent_file, sizeof(persistent_file), "%s/persistent.json", p);

    try
    {
        nlohmann::json j;
        j["MRU"] = m_lstMRUFiles;



        std::list<nlohmann::json> props;

        for (auto &it : m_ApplicationProps)
        {
            props.push_back(SerializeApplicationProperty(j, it));
        }

        j["ApplicationSettings"] = props;

        std::list<nlohmann::json> panels_state;

        // for (auto &it : ToolUIPanelID::_values())
        // {
        //     if (it._value == ToolUIPanelID::None)
        //         continue;

        //     if (it._value == ToolUIPanelID::MaxPanels)
        //         continue;

        //     nlohmann::json panel_desc;
        //     panel_desc["Valid"] = m_bPanelsValid[it._value];
        //     panel_desc["Id"]    = it._to_string();

        //     panels_state.push_back(panel_desc);
        // }

        j["PanelsState"] = panels_state;
        //j["BakerSettings"] = Application::Instance()->GetLightBakerApplication()->Settings()->ToJSON();

        //ViewportsOrchestrator::Instance()->SaveViewports(j);

        std::string data = j.dump(4);

        FILE *fp = 0;

#ifdef WINDOWS
        fopen_s(&fp, persistent_file, "wb");
#else
       fp = fopen(persistent_file, "wb");
#endif

        if (!fp)
            return;

        fprintf(fp, "%s", data.c_str());
        fflush(fp);
        fclose(fp);
    }
    catch (std::exception &e)
    {
        e;
    }
}
