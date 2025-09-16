#include "system_cleaner.hpp"

#include <windows.h> // temporary
#include <iostream>

#include "core/task_manager.hpp"
#include "utils/filesystem.hpp"

void core::SystemCleaner::cleanTemp()
{
	TaskManager::instance().addTask( []()
	{
		auto tempDir = utils::FileSystem::instance().getAppDataDir();
		OutputDebugStringA( ( tempDir.string() + "\n" ).c_str());
	} );
}
