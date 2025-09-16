#include "task_manager.hpp"

core::TaskManager::TaskManager() : m_treadPool( std::thread::hardware_concurrency() )
{
}

void core::TaskManager::addTask( std::function<void()> task )
{
	m_treadPool.submit_task( std::move( task ) );
}

void core::TaskManager::abort()
{

}
