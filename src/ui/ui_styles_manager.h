/*
	LightBaker3000 Frontend project,
	(c) 2023 CrazyRussian
*/

#pragma once
#include <vector>

namespace UIStyles
{
class StyleDescriptor
{
protected:
	std::string m_Description;
public:
	StyleDescriptor() = default;

	const char* Description();
	virtual void Apply();
};

class BuiltInStyle : public StyleDescriptor
{
	int m_id;
public:
	BuiltInStyle(int id, const char* descr)
	{
		m_id = id;
		m_Description = descr;
	}

	void Apply() override;
};

class UserDefinedStyle : public StyleDescriptor
{
	typedef std::pair<ImGuiCol_, ImVec4> styleColorDescriptor;
	std::vector<styleColorDescriptor> m_StyleData;
	
	ImGuiCol_ ColorIdFromString(const char* value);

	bool m_bValid = false;

public:
	UserDefinedStyle(const char* fileName);
	~UserDefinedStyle();

	void Apply() override;

};

class Manager
{
	Manager();
	std::vector<StyleDescriptor*> m_Styles;
public:
	~Manager();

	static Manager* Instance();
	void SelectNewStyle(size_t index);

	void PopulateStylesOption(VariantValue* pOption);	
	void ApplyCurrentStyle();
    void ScheduleStyleUpdateForAllWindows();
};
}
