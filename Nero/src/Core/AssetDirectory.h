#pragma once

#include "Core.h"
#include "Dependencies.h"

namespace Nero { namespace Util
{
	static bool loadingInternalAssets = false;
	static fs::path assetDirectory = "";

	fs::path GetAssetDirectory();
	void SetAssetDirectory(const fs::path& directory);

	void LoadInternalAssets();
	void StopLoadingInternalAssets();
} }
