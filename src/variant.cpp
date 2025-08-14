/*
    LightBaker3000 Frontend project,
    (c) 2022 CrazyRussian
*/


#include "variant.h"

VariantValue::VariantValue()
{
    // memset(this, 0, sizeof(*this));
    memset(&m_Value, 0, sizeof(m_Value));
}

VariantValue::VariantValue(int _id, PropertiesTypes _type, std::string _displayName)
{
    m_Id             = _id;
    m_Type           = _type;
    m_strDisplayName = _displayName;

    memset(&m_Value, 0, sizeof(m_Value));
}

VariantValue::VariantValue(const VariantValue &pOther)
{
    m_Id                = pOther.m_Id;
    m_Type              = pOther.m_Type;
    m_strDisplayName    = pOther.m_strDisplayName;
    m_EnumOrFlagsValues = pOther.m_EnumOrFlagsValues;
    m_bInitialized      = pOther.m_bInitialized;

    m_Limits[0] = pOther.m_Limits[0];
    m_Limits[1] = pOther.m_Limits[1];

    m_StringValue = pOther.m_StringValue;

    m_Value = pOther.m_Value;

    SetHelp(pOther.m_strHelp);
}

VariantValue::~VariantValue()
{
}

int VariantValue::FindEnumValueIndex(int enumValue)
{
    int i = 0;

    for (auto &it : m_EnumOrFlagsValues)
    {
        if (it.second == enumValue)
            return i;

        i++;
    }

    return -1;
}

void VariantValue::SetEnumIndexFromValue(int value)
{
    this->m_Value.asEnum = FindEnumValueIndex(value);
}

const int VariantValue::GetEnumValue() const
{
    if (!m_EnumOrFlagsValues.size())
        return 0;
#pragma warning(disable : 4018)
    if (m_Value.asEnum > (m_EnumOrFlagsValues.size() - 1))
        return 0;

    return m_EnumOrFlagsValues[m_Value.asEnum].second;
}

const int VariantValue::GetFlags() const
{
    return m_Value.asFlags;
}

const glm::vec3 VariantValue::GetPosition() const
{
    return m_Value.asPosition;
}

const glm::vec3 VariantValue::GetColorRGB() const
{
    return m_Value.asColorRGB;
}

const glm::vec4 VariantValue::GetColorRGBA() const
{
    return m_Value.asColorRGBA;
}

const float VariantValue::GetFloat() const
{
    return m_Value.asFloat;
}

const glm::vec3 VariantValue::GetAngles() const
{
    return m_Value.asAngles;
}

const int VariantValue::GetInt() const
{
    return m_Value.asInt;
}

const float VariantValue::GetSizeX() const
{
    return m_Value.asFloat;
}

const bool VariantValue::GetAsBool() const
{
    return m_Value.asBool;
}

const char *VariantValue::GetString() const
{
    return m_StringValue.c_str();
}

std::string VariantValue::GetStringStd() const
{
    return m_StringValue;
}

void VariantValue::SetEnumValue(int val)
{
    m_bInitialized = true;
    int idx        = 0;

    for (auto &it : m_EnumOrFlagsValues)
    {
        if (it.second == val)
        {
            m_Value.asEnum = idx;
        }

        idx++;
    }
}

void VariantValue::SetFlags(int val)
{
    m_bInitialized  = true;
    m_Value.asFlags = val;
}

void VariantValue::SetPosition(glm::vec3 val)
{
    m_bInitialized     = true;
    m_Value.asPosition = val;
}

void VariantValue::SetColorRGB(glm::vec3 val)
{
    m_bInitialized     = true;
    m_Value.asColorRGB = val;
}

void VariantValue::SetColorRGBA(glm::vec4 val)
{
    m_bInitialized      = true;
    m_Value.asColorRGBA = val;
}

void VariantValue::SetFloat(float val)
{
    m_bInitialized  = true;
    m_Value.asFloat = val;
}

void VariantValue::SetAngles(glm::vec3 val)
{
    m_bInitialized   = true;
    m_Value.asAngles = val;
}

void VariantValue::SetDeltaAngles(glm::vec3 val)
{
    assert(m_bInitialized);
    m_Value.asAngles += val;

    for (int i = 0; i < 3; i++)
        m_Value.asAngles[i] = AngleMod(m_Value.asAngles[i]);
}

void VariantValue::SetInt(int val)
{
    m_bInitialized = true;
    m_Value.asInt  = val;
}

void VariantValue::SetSizeX(float val)
{
    m_bInitialized  = true;
    m_Value.asFloat = val;
}

void VariantValue::SetBool(bool val)
{
    m_bInitialized = true;
    m_Value.asBool = val;
}

void VariantValue::SetString(const char *value)
{
    m_bInitialized = true;
    m_StringValue  = value;
}

void VariantValue::SetString(std::string val)
{
    m_bInitialized = true;
    m_StringValue  = val;
}

void VariantValue::AddEnumValue(const char *descr, int val)
{
    m_EnumOrFlagsValues.push_back(enumValuePair_t(descr, val));
}

const int VariantValue::GetId() const
{
    return m_Id;
}

const char *VariantValue::DisplayName() const
{
    return m_strDisplayName.c_str();
}

const PropertiesTypes VariantValue::GetType() const
{
    return m_Type;
}

void *VariantValue::Data()
{
    return (void *)&m_Value.asBool;
}

const std::vector<enumValuePair_t> &VariantValue::GetEnumValues() const
{
    return m_EnumOrFlagsValues;
}

void VariantValue::SetTempLabel(char *param1)
{
    m_TempLabel = param1;
}

const char *VariantValue::TempLabel()
{
    return m_TempLabel;
}

bool VariantValue::IsInitialized()
{
    return m_bInitialized;
}

void VariantValue::SetNumericalLimits(float min, float max)
{
    m_Limits[0] = min;
    m_Limits[1] = max;
}

void VariantValue::ValidateValue()
{
    switch (m_Type)
    {
    case PropertiesTypes::Float:
        m_Value.asFloat = std::clamp(m_Value.asFloat, m_Limits[0], m_Limits[1]);
        break;
    case PropertiesTypes::SizeX:
    case PropertiesTypes::Int:
        m_Value.asInt = std::clamp(m_Value.asInt, (int)m_Limits[0], (int)m_Limits[1]);
        break;
    default:
        break;
    }
}

float *VariantValue::GetNumericalLimits()
{
    return m_Limits;
}

std::string *VariantValue::GetStdStringPtr()
{
    return &m_StringValue;
}

const std::string &VariantValue::Help() const
{
    return m_strHelp;
}

void VariantValue::SetHelp(const std::string &str)
{
    m_strHelp = str;
}

void VariantValue::SetDisplayName(const std::string &dispName, bool updateIfSet /*= true*/)
{
    if (!updateIfSet && !m_strDisplayName.empty())
        return;

    m_strDisplayName = dispName;
}

// void IObjectPropertiesBinding::RenderFooter()
// {
//     return;
// }
// 
// SceneEntity *IObjectPropertiesBinding::GetEntity(int param1)
// {
//     return nullptr;
// }
