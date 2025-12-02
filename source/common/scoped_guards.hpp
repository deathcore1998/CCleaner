#include <imgui.h>

#include <string>

namespace ImGui
{
	struct StyleGuard
	{
		StyleGuard( ImGuiCol idx, ImU32 col )
		{
			ImGui::PushStyleColor( idx, col );
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

	struct DisabledGuard
	{
		DisabledGuard( bool disable )
		{
			ImGui::BeginDisabled( disable );
		}

		~DisabledGuard()
		{
			ImGui::EndDisabled();
		}
	};

	class Table
	{
	public:
		Table( std::string_view strId, int columns, ImGuiTableFlags flags = 0, const ImVec2& size = ImVec2( 0.0f, 0.0f ) )
		{
			m_open = ImGui::BeginTable( strId.data(), columns, flags, size );
		}

		~Table()
		{
			if ( m_open )
			{
				ImGui::EndTable();
			}	
		}

		explicit operator bool() const
		{
			return m_open;
		}

	private:
		bool m_open = true;
	};
}