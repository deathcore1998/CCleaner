#pragma once

namespace gui
{
	class Widget
	{
	public:
		Widget() = default;
		~Widget() = default;

		virtual void draw() = 0;

	private:

	};
}