/*
	LightBaker3000 Frontend project,
	(c) 2023 CrazyRussian
*/

#include "../application.h"
#include "../common.h"
#include "../variant.h"

#include "ui_options_pages.h"
#include "ui_styles_manager.h"


using namespace ProgramOptions;

uiOptionPage_t g_OptionsPages[(int)OptionsPage::Total];
uiOptionPage_t* builderCurrentPage = nullptr;

void ProgramOptions::BeginOptionPage(OptionsPage id, const char* pageDescription)
{
	builderCurrentPage = &g_OptionsPages[(int)id];
	builderCurrentPage->pageDescription = pageDescription;
	builderCurrentPage->items.clear();
	builderCurrentPage->pageDescription = pageDescription;
}


VariantValue* ProgramOptions::AddOption(ApplicationSettings setting, const char* description, PropertiesTypes type)
{
//	VariantValue ;
	OptionsValue* p = new OptionsValue(new VariantValue((int)setting, type, description));
	builderCurrentPage->items.push_back(p);

	return p->GetValue();
}

void ProgramOptions::AddGroup(const char* descr)
{
	OptionsHeader* p = new OptionsHeader(descr);
	builderCurrentPage->items.push_back(p);
}


OptionPageItemType OptionPageItem::ItemType()
{
	return m_Type;
}

OptionsHeader::OptionsHeader(const char* descritpion)
{
	m_Type = OptionPageItemType::GroupHeader;
	m_Description = std::string(descritpion);
}


void OptionsHeader::RenderImGUI()
{
	ImGui::TableSetColumnIndex(0);
	ImGui::AlignTextToFramePadding();
	
	//ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Leaf;
	//ImGui::TreeNodeEx("Field", flags, "%s", m_Description.c_str());

	ImGui::SeparatorText(m_Description.c_str());


	ImGui::TableSetColumnIndex(1);
	ImGui::SetNextItemWidth(-FLT_MIN);


	
}


void OptionsValue::RenderImGUI()
{	
	ImGui::TableSetColumnIndex(0);	
	ImGui::AlignTextToFramePadding();
	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Bullet;
	ImGui::TreeNodeEx("Field", flags, "%s", m_Value->DisplayName());
	

	ImGui::TableSetColumnIndex(1);		
	ImGui::SetNextItemWidth(-FLT_MIN);
	

	switch (m_Value->GetType())
	{
	case PropertiesTypes::Enum:
		RenderEnumProperty();
		break;
	case PropertiesTypes::Flags:
		RenderFlagsProperty();
		break;
	case PropertiesTypes::Position:
		if (ImGui::DragFloat3("Vector", (float*)m_Value->Data()))
			UpdateProperty();
		break;
	case PropertiesTypes::ColorRGB:
		if (ImGui::ColorEdit3("##value", (float*)m_Value->Data()))
			UpdateProperty();
		break;
	case PropertiesTypes::ColorRGBA:
		if (ImGui::ColorEdit4("##value", (float*)m_Value->Data()))
			UpdateProperty();
		break;
	case PropertiesTypes::Float:
	{
		float* limits = m_Value->GetNumericalLimits();

		bool limMinSet = abs(limits[0] - (-FLT_MAX)) > 0.001;
		bool limMaxSet = abs(limits[1] - FLT_MAX) > 0.001;

		
		if (limMinSet && limMaxSet)
		{
			if (ImGui::DragFloat("##value", (float*)m_Value->Data(), 0.f, limits[0], limits[1]))
				UpdateProperty();
		}
		else
		{
			if (ImGui::DragFloat("##value", (float*)m_Value->Data()))
				UpdateProperty();
		}
	}
		break;
	case PropertiesTypes::Angles:
		if (ImGui::DragFloat3("Vector", (float*)m_Value->Data()))
			UpdateProperty();
		break;
	case PropertiesTypes::Int:
		if (ImGui::DragInt("##value", (int*)m_Value->Data(), 0.01f))
			UpdateProperty();
		break;
	case PropertiesTypes::Bool:
		if (ImGui::Checkbox("##value", (bool*)m_Value->Data()))
			UpdateProperty();
		break;
	case PropertiesTypes::SizeX:
		if (ImGui::DragFloat("Vector", (float*)m_Value->Data()))
			UpdateProperty();
		break;
	default:
		break;

	}
}

OptionsValue::OptionsValue(VariantValue* val)
{
	m_Value = val;
	m_Type = OptionPageItemType::Value;
}

VariantValue* OptionsValue::GetValue()
{
	return m_Value;
}

void OptionsValue::RenderFlagsProperty()
{
	if (ImGui::Button("..."))
	{
		ImGui::OpenPopup(ImGui::GetID("Flags"));
		//m_pCurrentFlagProp = &it;
	}

	ImGui::SameLine();

	std::string flagDescription = "[";

	for (auto flagValue : m_Value->GetEnumValues())
	{
		if (m_Value->GetFlags() & flagValue.second)
			flagDescription += flagValue.first + ",";
	}

	if (flagDescription.length() > 1)
		flagDescription[flagDescription.length() - 1] = ']';
	else
		flagDescription += "0]";

	ImGui::Text("%s", flagDescription.c_str());
}

void OptionsValue::RenderEnumProperty()
{
	if (m_Value->GetEnumValues().size() > 0)
	{
		int selectionIndex = m_Value->GetInt();
		m_Value->SetTempLabel((char*)m_Value->GetEnumValues()[selectionIndex].first.c_str());

		if (ImGui::BeginCombo(m_Value->DisplayName(), m_Value->TempLabel()))
		{
			int i = 0;

			for (auto& enumValue : m_Value->GetEnumValues())
			{
				if (ImGui::Selectable(enumValue.first.c_str(), i == selectionIndex))
				{
					m_Value->SetInt(i);
					m_Value->SetTempLabel((char*)enumValue.first.c_str());
					UpdateProperty();
				}

				i++;
			}

			ImGui::EndCombo();
		}
	}
	else
	{
		ImGui::Text("No values defined!");
	}
}

void OptionsValue::UpdateProperty()
{
	if (m_Value->GetId() == (int)ApplicationSettings::GUIColorScheme)
	{
		// UIStyles::Manager::Instance()->SelectNewStyle(m_Value->GetEnumValue());
        UIStyles::Manager::Instance()->ScheduleStyleUpdateForAllWindows();
	}
}
