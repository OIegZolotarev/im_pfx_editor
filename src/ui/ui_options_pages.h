/*
	LightBaker3000 Frontend project,
	(c) 2023 CrazyRussian
*/


#pragma once
#include "../persistent.h"

namespace ProgramOptions
{

enum class OptionsPage
{
	General = 0,
	Camera,
	//Keyboard,
	Appearence,
	Total
};

enum class OptionPageItemType
{
	Value = 0,
	GroupHeader,
	None
};

class OptionPageItem
{
protected:
	OptionPageItemType m_Type = OptionPageItemType::None;
public:
	OptionPageItem() = default;
	virtual ~OptionPageItem() = default;

	OptionPageItemType  ItemType();
	virtual void RenderImGUI() {};
};

class OptionsHeader : public OptionPageItem
{
	std::string m_Description;
public:

	OptionsHeader(const char* descritpion);
	void RenderImGUI() override;
};

class OptionsValue : public OptionPageItem
{
	VariantValue* m_Value = nullptr;
	
	void RenderFlagsProperty();
	void RenderEnumProperty();

	void UpdateProperty();
public:
	OptionsValue(VariantValue* val);
	~OptionsValue()
	{
		if (m_Value) delete m_Value;
	}

	VariantValue* GetValue();
	void RenderImGUI() override;
};


typedef struct uiOptionPage_s
{
	OptionsPage id;

	std::string pageDescription;
	bool selected;

	std::vector<OptionPageItem*> items;
}uiOptionPage_t;

void BeginOptionPage(ProgramOptions::OptionsPage id, const char* pageDescription);
VariantValue* AddOption(ApplicationSettings setting, const char* description, PropertiesTypes type);
void AddGroup(const char* descr);

}// namespace ProgramOptions

