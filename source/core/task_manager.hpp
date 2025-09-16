#pragma once

#include <BS_thread_pool.hpp>

#include <functional>

namespace core
{
	class TaskManager
	{
	public:
		[[nodiscard]]static TaskManager& instance()
		{
			static TaskManager taskManager;
			return taskManager;
		}

		void addTask( std::function< void() > task );
		void abort();

	private:
		TaskManager();
		~TaskManager() = default;
		TaskManager( const TaskManager& ) = delete;
		TaskManager& operator = ( const TaskManager& ) = delete;

		BS::thread_pool m_treadPool;
	};
}