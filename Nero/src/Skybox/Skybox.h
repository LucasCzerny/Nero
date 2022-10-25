#pragma once

#include "Dependencies.h"
#include "Core.h"

#include "OpenGL/Texture/Cubemap.h"

namespace Nero
{
	class Skybox
	{
	private:
		Ref<Cubemap> m_Sky;

		Ref<Cubemap> m_IrradianceMap;
		Ref<Cubemap> m_PrefilteredEnvironmentMap;

		fs::path m_Directory;
		std::string m_FileExtension;


	public:
		static Ref<Skybox> Create(const fs::path& directory, const std::string& extension = "jpg");

		Skybox(const fs::path& directory, const std::string& extension = "jpg");

		void Bind(uint32_t slot = 0) const;

		Ref<Cubemap> GetPrefilteredEnvironmentMap() const { return m_PrefilteredEnvironmentMap; }
		Ref<Cubemap> GetIrradianceMap() const { return m_IrradianceMap; }

		fs::path GetDirectory() const { return m_Directory; }
		std::string GetFileExtension() const { return m_FileExtension; }
	};
}


