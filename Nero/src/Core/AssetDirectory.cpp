#include "AssetDirectory.h"

namespace Nero { namespace Util
{
	fs::path GetAssetDirectory()
	{
		if (loadingInternalAssets)
			return "../Nero/Assets";

		return assetDirectory;
	}

	void SetAssetDirectory(const fs::path& directory)
	{
		fs::path currentPath = fs::current_path();
		assetDirectory = currentPath / directory;

		Log::Info("Asset directory is now " + assetDirectory.string());
	}

	void LoadInternalAssets()
	{
		loadingInternalAssets = true;
	}

	void StopLoadingInternalAssets()
	{
		loadingInternalAssets = false;
	}
} }
