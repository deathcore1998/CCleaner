#pragma once

#include <string>

namespace gui
{
	class Widget
	{
	public:
		Widget( std::string title ) : m_title( std::move( title ) ) {}
		~Widget() = default;

		virtual void draw() = 0;

	protected:
		std::string m_title;
	};
}