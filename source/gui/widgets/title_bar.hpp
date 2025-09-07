#pragma once

#include "gui/widgets/widget.hpp"

namespace gui
{
	class TitleBar : public Widget
	{
	public:
		TitleBar();
		~TitleBar();

		void draw() override;

	private:

	};
}