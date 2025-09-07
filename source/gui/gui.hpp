#pragma once

#include "widgets/title_bar.hpp"

namespace gui
{
	class Gui
	{
	public:
		Gui();
		~Gui();

		void render();

	private:
		Widget* m_titleBar = nullptr;
	};
}