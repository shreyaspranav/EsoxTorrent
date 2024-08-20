#pragma once

#include <string>

namespace Esox
{
	class UILayer
	{
	public:
		virtual void OnPush() = 0;
		virtual void OnPop() = 0;

		virtual void UIUpdate(float timestep) = 0;
	protected:
		std::string layerName = "UILayer";
	};
}