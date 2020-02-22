#pragma once

class simple_thread_task
{
public:
	virtual void run() = 0;
};

class simple_thread_worker : public pfc::thread
{
public:
	simple_thread_worker();
	virtual ~simple_thread_worker();

	void threadProc() override;

private:
	PFC_CLASS_NOT_COPYABLE_EX(simple_thread_worker)
};

class simple_thread_pool
{
public:
	simple_thread_pool();
	~simple_thread_pool();

	static simple_thread_pool& instance();

	bool enqueue(simple_thread_task* task);
	bool is_queue_empty();
	simple_thread_task* acquire_task();
	void exit();
	void track(simple_thread_task* task);
	void untrack(simple_thread_task* task);
	void untrack_all();

private:
	void add_worker_(simple_thread_worker* worker);
	void remove_worker_(simple_thread_worker* worker);

	using t_task_list = pfc::chain_list_v2_t<simple_thread_task*>;

	static simple_thread_pool instance_;

	HANDLE empty_worker_;
	HANDLE exiting_;
	HANDLE have_task_;
	critical_section cs_;
	t_task_list task_list_;
	volatile LONG num_workers_;

	friend class simple_thread_worker;

	PFC_CLASS_NOT_COPYABLE_EX(simple_thread_pool)
};
