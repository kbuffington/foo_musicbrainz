#include "stdafx.h"
#include "thread_pool.h"

class mb_initquit : public initquit {
public:
	void on_init() {
	}
	void on_quit() {
		simple_thread_pool::instance().exit();
	}
};

static initquit_factory_t<mb_initquit> g_myinitquit_factory;
