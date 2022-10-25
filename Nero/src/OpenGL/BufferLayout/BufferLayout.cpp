#include "BufferLayout.h"

namespace Nero
{
	BufferLayout::BufferLayout(const std::initializer_list<BufferElement>& elements)
		: m_Elements(elements)
	{
		FUNCTION_TIMER;

		for (BufferElement& element : m_Elements)
		{
			element.Offset = m_Stride;
			m_Stride += element.Size;
		}
	}
}
