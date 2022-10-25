#include "FileDialog.h"

using namespace Nero;

namespace ImCustom { namespace FileDialog
{
	fs::path OpenFileDialog(const char* filter, fs::path initialDirectory /* = "" */)
	{
		OPENFILENAMEA ofn;
		CHAR szFile[MAX_PATH] = { 0 };

		ZeroMemory(&ofn, sizeof(OPENFILENAMEA));
		ofn.lStructSize = sizeof(OPENFILENAMEA);
		ofn.hwndOwner = glfwGetWin32Window(Application::GetGlfwWindow());
		ofn.lpstrFile = szFile;
		ofn.nMaxFile = sizeof(szFile);
		ofn.lpstrFilter = filter;
		ofn.nFilterIndex = 1;
		ofn.lpstrFileTitle = NULL;
		ofn.nMaxFileTitle = 0;
		ofn.lpstrInitialDir = NULL;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

		std::string directoryString = initialDirectory.string();
		ofn.lpstrInitialDir = directoryString.c_str();

		if (GetOpenFileNameA(&ofn) == TRUE)
		{
			return ofn.lpstrFile;
		}

		// Dialog has been cancelled
		return "";
	}

	fs::path SaveFileDialog(const char* filter, fs::path initialDirectory /* = "" */)
	{
		OPENFILENAMEA ofn;
		CHAR szFile[MAX_PATH] = { 0 };

		ZeroMemory(&ofn, sizeof(OPENFILENAMEA));
		ofn.lStructSize = sizeof(OPENFILENAMEA);
		ofn.hwndOwner = glfwGetWin32Window(Application::GetGlfwWindow());
		ofn.lpstrFile = szFile;
		ofn.nMaxFile = sizeof(szFile);
		ofn.lpstrFilter = filter;
		ofn.nFilterIndex = 1;
		ofn.lpstrFileTitle = NULL;
		ofn.nMaxFileTitle = 0;
		ofn.lpstrInitialDir = NULL;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

		std::string directoryString = initialDirectory.string();
		ofn.lpstrInitialDir = directoryString.c_str();

		if (GetSaveFileNameA(&ofn) == TRUE)
		{
			return ofn.lpstrFile;
		}

		// Dialog has been cancelled
		return "";
	}
} }