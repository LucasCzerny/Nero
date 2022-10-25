#pragma once

#include "EditorDependencies.h"

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#include <commdlg.h>
#include <codecvt>

#define FILE_DIALOG_FILTER(tag, extensions) tag "\0" extensions "\0\0"

namespace ImCustom { namespace FileDialog
{
	fs::path OpenFileDialog(const char* filter, fs::path initialDirectory = "");
	fs::path SaveFileDialog(const char* filter, fs::path initialDirectory = "");
} }
