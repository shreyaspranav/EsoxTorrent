#pragma once

#include "Base.h"

namespace Esox
{
	class UILayer
	{
	public:
		virtual void OnPush() = 0;
		virtual void OnPop() = 0;

		virtual void UIUpdate(float timestep) = 0;
	protected:
		String layerName = "UILayer";
	};
}