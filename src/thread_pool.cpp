#include "stdafx.h"
#include "thread_pool.h"

simple_thread_worker::simple_thread_worker() {}

simple_thread_worker::~simple_thread_worker()
{
	waitTillDone();
}

void simple_thread_worker::threadProc()
{
	pfc::tickcount_t last_tick = pfc::getTickCount();

	while (WaitForSingleObject(simple_thread_pool::instance().exiting_, 0) == WAIT_TIMEOUT)
	{
		if (WaitForSingleObject(simple_thread_pool::instance().have_task_, 1000) == WAIT_OBJECT_0)
		{
			simple_thread_task* task = simple_thread_pool::instance().acquire_task();

			if (task)
			{
				task->run();
				simple_thread_pool::instance().untrack(task);
				last_tick = pfc::getTickCount();
				continue;
			}
		}

		if (pfc::getTickCount() - last_tick >= 10000)
		{
			insync(simple_thread_pool::instance().cs_);

			if (simple_thread_pool::instance().is_queue_empty())
			{
				simple_thread_pool::instance().remove_worker_(this);
				return;
			}
		}
	}

	simple_thread_pool::instance().remove_worker_(this);
}

simple_thread_pool simple_thread_pool::instance_;

simple_thread_pool::simple_thread_pool() : num_workers_(0)
{
	empty_worker_ = CreateEvent(nullptr, TRUE, TRUE, nullptr);
	exiting_ = CreateEvent(nullptr, TRUE, FALSE, nullptr);
	have_task_ = CreateEvent(nullptr, TRUE, FALSE, nullptr);

	pfc::dynamic_assert(empty_worker_ != INVALID_HANDLE_VALUE);
	pfc::dynamic_assert(exiting_ != INVALID_HANDLE_VALUE);
	pfc::dynamic_assert(have_task_ != INVALID_HANDLE_VALUE);
}

simple_thread_pool::~simple_thread_pool()
{
	CloseHandle(empty_worker_);
	CloseHandle(exiting_);
	CloseHandle(have_task_);
}

bool simple_thread_pool::enqueue(simple_thread_task* task)
{
	if (WaitForSingleObject(exiting_, 0) == WAIT_OBJECT_0)
		return false;

	insync(cs_);
	int max_count = pfc::getOptimalWorkerThreadCount();
	track(task);

	if (num_workers_ < max_count)
	{
		simple_thread_worker* worker = new simple_thread_worker;
		add_worker_(worker);
		worker->start();
	}

	return true;
}

bool simple_thread_pool::is_queue_empty()
{
	insync(cs_);
	return task_list_.get_count() == 0;
}

simple_thread_task* simple_thread_pool::acquire_task()
{
	insync(cs_);

	t_task_list::iterator iter = task_list_.first();

	if (iter.is_valid())
	{
		task_list_.remove(iter);
	}

	if (is_queue_empty())
		ResetEvent(have_task_);

	return iter.is_valid() ? *iter : nullptr;
}

simple_thread_pool& simple_thread_pool::instance()
{
	return instance_;
}

void simple_thread_pool::add_worker_(simple_thread_worker* worker)
{
	insync(cs_);
	InterlockedIncrement(&num_workers_);
	ResetEvent(empty_worker_);
}

void simple_thread_pool::exit()
{
	core_api::ensure_main_thread();

	SetEvent(exiting_);

	// Because the tasks may use blocking SendMessage() function, it should be avoid using
	// infinite wait here, or both main thread and worker thread will block, and it's also
	// important to dispatch windows messages here.
	while (WaitForSingleObject(empty_worker_, 0) == WAIT_TIMEOUT)
	{
		MSG msg;

		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	untrack_all();
}

void simple_thread_pool::remove_worker_(simple_thread_worker* worker)
{
	InterlockedDecrement(&num_workers_);
	insync(cs_);

	if (num_workers_ == 0)
		SetEvent(empty_worker_);

	fb2k::inMainThread([=] {
		delete worker;
		});
}

void simple_thread_pool::track(simple_thread_task* task)
{
	insync(cs_);
	bool empty = is_queue_empty();
	task_list_.add_item(task);

	if (empty)
		SetEvent(have_task_);
}

void simple_thread_pool::untrack(simple_thread_task* task)
{
	insync(cs_);
	task_list_.remove_item(task);
	delete task;

	if (is_queue_empty())
		ResetEvent(have_task_);
}

void simple_thread_pool::untrack_all()
{
	insync(cs_);
	for (t_task_list::iterator iter = task_list_.first(); iter.is_valid(); ++iter)
	{
		task_list_.remove(iter);
		delete* iter;
	}

	ResetEvent(have_task_);
}
