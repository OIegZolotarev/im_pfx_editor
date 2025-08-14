/*
    LightBaker3000 Frontend project,
    (c) 2022 CrazyRussian
*/

#pragma once

#include "common.h"
#include "mathlib.h"


enum class PropertiesTypes
{
    Enum,
    Flags,
    Position,
    ColorRGB,
    ColorRGBA,
    Float,
    Angles,
    Int,
    SizeX,
    Bool,
    String,
};

typedef std::pair<std::string, int> enumValuePair_t;

class VariantValue
{
protected:
    PropertiesTypes              m_Type;
    bool                         m_bInitialized = false;
    std::vector<enumValuePair_t> m_EnumOrFlagsValues;

    union data {
        int       asEnum;
        int       asFlags;
        glm::vec3 asPosition;
        glm::vec4 asColorRGBA;
        glm::vec3 asColorRGB;
        float     asFloat;
        glm::vec3 asAngles;
        int       asInt;
        bool      asBool;
    } m_Value;

private:
    std::string m_strHelp = "";
    std::string m_strDisplayName;

    int m_Id;

    float m_Limits[2] = {-FLT_MAX, FLT_MAX};

    std::string m_StringValue;

    char *m_TempLabel = 0;

    virtual int classId()
    {
        return 0;
    }

public:
    VariantValue();
    ~VariantValue();

    VariantValue(int _id, PropertiesTypes _type, std::string _displayName);
    VariantValue(const VariantValue &pOther);

    int  FindEnumValueIndex(int enumValue);
    void SetEnumIndexFromValue(int value);

    const int       GetEnumValue() const;
    const int       GetFlags() const;
    const glm::vec3 GetPosition() const;
    const glm::vec3 GetColorRGB() const;
    const glm::vec4 GetColorRGBA() const;
    const float     GetFloat() const;
    const glm::vec3 GetAngles() const;
    const int       GetInt() const;
    const float     GetSizeX() const;
    const bool      GetAsBool() const;
    const char *    GetString() const;
    std::string     GetStringStd() const;

    void SetEnumValue(int val);
    void SetFlags(int val);
    void SetPosition(glm::vec3 val);
    void SetColorRGB(glm::vec3 val);
    void SetColorRGBA(glm::vec4 val);
    void SetFloat(float val);
    void SetAngles(glm::vec3 val);
    void SetDeltaAngles(glm::vec3 val);
    void SetInt(int val);
    void SetSizeX(float val);
    void SetBool(bool val);
    void SetString(const char *value);
    void SetString(std::string val);

    const int             GetId() const;
    const PropertiesTypes GetType() const;

    void AddEnumValue(const char *descr, int val);

    const char *DisplayName() const;

    void *Data();

    const std::vector<enumValuePair_t> &GetEnumValues() const;
    void                                SetTempLabel(char *param1);
    const char *                        TempLabel();

    bool IsInitialized();

    void         SetNumericalLimits(float min, float max);
    void         ValidateValue();
    float *      GetNumericalLimits();
    std::string *GetStdStringPtr();

    const std::string &Help() const;

    void SetHelp(const std::string &str);

    void SetDisplayName(const std::string &dispName, bool updateIfSet = true);
};

class SceneEntity;

