#include <imgui.h>

#include <string>

namespace ImGui
{
	struct StyleGuard
	{
		StyleGuard( ImGuiCol idx, ImU32 col )
		{
			ImGui::PushStyleColor( ImGuiCol_ChildBg, IM_COL32( 100, 100, 100, 255 ) );
		}

		~StyleGuard()
		{
			ImGui::PopStyleColor();
		}
	};

	struct IDGuard
	{
		IDGuard( std::string id )
		{
			ImGui::PushID( id.c_str() );
		}

		~IDGuard()
		{
			ImGui::PopID();
		}
	};
}