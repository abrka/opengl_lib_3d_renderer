#pragma once

#include "render_context.h"

namespace Engine {
	class IRenderable {
	public:
		virtual void render(const RenderContext& ctx) = 0;
	};
}