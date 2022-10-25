#pragma once

#include "Core.h"
#include "Dependencies.h"

namespace Nero
{
	class VertexBuffer
	{
	private:
		uint32_t m_RendererId;

	public:
		static Ref<VertexBuffer> Create(const float* data, uint32_t size);

		VertexBuffer(const float* data, uint32_t size);
		~VertexBuffer();

		void Bind() const;
		void Unbind() const;
	};
}