#include "Math.h"

namespace Nero { namespace Util
{
	float Lerp(float a, float b, float t)
	{
		return a + t * (b - a);
	}
} }