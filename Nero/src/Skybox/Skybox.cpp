#include "Skybox.h"

#include "Renderer/Renderer.h"

namespace Nero
{
	Ref<Skybox> Skybox::Create(const fs::path& directory, const std::string& extension /* = "jpg" */)
	{
		FUNCTION_TIMER;

		return CreateRef<Skybox>(directory);
	}

	Skybox::Skybox(const fs::path& directory, const std::string& extension /* = "jpg" */)
		: m_Directory(directory), m_FileExtension(extension)
	{
		FUNCTION_TIMER;

		std::array<fs::path, 6> faces = {
			directory / ("posx." + extension),
			directory / ("negx." + extension),
			directory / ("posy." + extension),
			directory / ("negy." + extension),
			directory / ("posz." + extension),
			directory / ("negz." + extension)
		};

		m_Sky = Cubemap::Create(faces);
		m_Sky->GenerateMipMap();

		m_IrradianceMap = Renderer::CreateIrradianceMap(m_Sky);
		m_PrefilteredEnvironmentMap = Renderer::PrefilerEnvironmentMap(m_Sky);
	}

	void Skybox::Bind(uint32_t slot /* = 0 */) const
	{
		FUNCTION_TIMER;

		m_Sky->Bind(slot);
	}
}