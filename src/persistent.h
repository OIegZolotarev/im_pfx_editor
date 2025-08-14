
#pragma once

#include "common.h"
#include <nlohmann/json.hpp>


// To preserve old settings add new values to the end only!
// Otherwise old config files will brake!
enum class ApplicationSettings
{
    ShowGround = 0,
    BackgroundColor1,
    BackgroundColor2,
    UseGradientBackground,    
    CameraControlScheme,
    CameraMovementSpeed,
    CameraZNear,
    CameraZFar,
    CameraFov,
    CameraAccel,
    CameraDecel,
    GUIColorScheme,
    GridAxisColor,
    GridMainColor,
    Grid64thLineColor,
    Grid1024thLineColor,
    GridCustomColor,
    GridCustomStep,
    CameraMouseSensivityRotating,
    CameraMouseSensivityPaning,
    CameraMouseSensivityZooming    
};

class VariantValue;

typedef std::pair<std::string, std::time_t> mruFile_t;

class PersistentStorage
{
    nlohmann::json SerializeApplicationProperty(nlohmann::json &j, VariantValue *it);
    void ParseApplicationSettings(nlohmann::json j);

    std::list<mruFile_t> m_lstMRUFiles;
    void SaveToFile();
    void LoadFromFile(class Application *appInstance);

    std::vector<VariantValue *> m_ApplicationProps;

    // bool m_bPanelsValid[ToolUIPanelID::MaxPanels] = {false};

    bool m_bFreshFile;

  public:
    PersistentStorage(class Application *appInstance);
    ~PersistentStorage();
    
    void SetDefaultValues();      
    void PushMRUFile(const char *fileName);

    void SortMRU();

    std::list<mruFile_t> &GetMRUFiles();

    VariantValue *GetSetting(ApplicationSettings id);

    bool GetSettingBool(ApplicationSettings id);

    // bool IsPanelAtValidPosition(ToolUIPanelID id);
    // void FlagPanelIsAtValidPosition(ToolUIPanelID id);
    bool IsFreshFile();

    static PersistentStorage *Instance();
    void                      RemoveMRUItem(std::string &filePath);
};