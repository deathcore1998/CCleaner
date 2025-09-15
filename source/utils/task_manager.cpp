#include "task_manager.hpp"

utils::TaskManager::TaskManager() : m_treadPool( std::thread::hardware_concurrency() )
{
}

void utils::TaskManager::addTask( std::function<void()> task )
{
	m_treadPool.submit_task( std::move( task ) );
}

void utils::TaskManager::abort()
{

}
