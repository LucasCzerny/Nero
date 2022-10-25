#pragma once

#include "Core.h"
#include "Dependencies.h"

namespace Nero
{
	class IndexBuffer
	{
	private:
		uint32_t m_RendererId;
		uint32_t m_Count;

	public:
		static Ref<IndexBuffer> Create(uint32_t* data, uint32_t size);

		IndexBuffer(uint32_t* data, uint32_t size);
		~IndexBuffer();

		void Bind() const;
		void Unbind() const;

		uint32_t GetCount() const { return m_Count; }
	};
}