#include "stdafx.h"
#include "dialog_tagger.h"
#include "request_thread.h"
#include "thread_pool.h"
#include "query_cache.h"
#include <atomic>
#include <mutex>

namespace mb
{
	std::mutex adding_release;
	std::atomic_uint32_t thread_counter = 0;

	class get_release_info : public simple_thread_task {
	public:
		get_release_info(pfc::string mb_releaseid, size_t num_tracks, std::vector<Release>* release_list, 
						 bool* failed, abort_callback& abort) 
			: m_id(mb_releaseid)
			, m_num_tracks(num_tracks)
			, m_release_list(release_list)
			, m_failed(failed)
			, m_abort(abort) {}

		void run() override
		{
			try {
				thread_counter++;

				query q("release", m_id);
				q.add_param("inc", "artists+labels+recordings+release-groups+artist-credits+isrcs");

				json j2 = q.lookup(m_abort);
				if (!j2.is_object())
				{
					*m_failed = true;
					thread_counter--;
				}
				else {
					Release r = parser(j2, m_num_tracks);
					if (r.tracks.size())
					{
						std::lock_guard<std::mutex> guard(adding_release);	// lock_guard is destroyed when exiting block
						m_release_list->emplace_back(r);
					}
					else {
						thread_counter--;
					}
				}

			}
			catch (std::exception const& e) {
				m_failMsg = e.what();
				FB2K_console_formatter() << component_title << ": " << m_failMsg;
			}
		}
	private:
		pfc::string8 m_failMsg;
		const pfc::string m_id;
		abort_callback& m_abort;
		size_t m_num_tracks;
		bool* m_failed;
		std::vector<Release> * m_release_list;
	};


	request_thread::request_thread(types type, std::unique_ptr<query> q, metadb_handle_list_cref handles)
		: m_type(type)
		, m_query(std::move(q))
		, m_handles(handles)
		, m_failed(false) {}

	void request_thread::on_done(HWND hwnd, bool was_aborted)
	{
		if (was_aborted || m_failed) return;

		if (m_release_list.size())
		{
			std::sort(m_release_list.begin(), m_release_list.end(), [](const Release& one, const Release& two) -> bool
				{
					return strcmp(one.date, two.date) < 0;
				});
			fb2k::newDialog<dialog_tagger>(m_release_list, m_handles);
		}
		else
		{
			popup_message::g_show("No matching results were found.", component_title);
		}
	}

	void request_thread::run(threaded_process_status& status, abort_callback& abort)
	{
		const size_t handle_count = m_handles.get_count();

		json j = m_query->lookup(abort);
#ifdef DEBUG
		FB2K_console_formatter() << component_title << ": " << m_query->get_url();
#endif
		if (!j.is_object())
		{
			m_failed = true;
			return;
		}

		switch (m_type)
		{
		case types::discid:
			{
				str8 discid = to_str(j["id"]);
				json releases = j.value("releases", json::array());
				for (const auto& release : releases)
				{
					Release r = parser(release, handle_count, discid);
					if (r.tracks.size())
					{
						m_release_list.emplace_back(r);
					}
				}
			}
			break;
		case types::search:
			{
				queryingMB = true;
				json releases = j.value("releases", json::array());
				pfc::string_list_impl ids;
				filter_releases(releases, handle_count, ids);
				const size_t count = ids.get_count();

				thread_counter = 0;

				for (size_t i = 0; i < count; ++i)
				{
					/**
					 * this was pre-existing logic which would abort when we didn't receive anything back 
					 * from musicbrainz. This would prevent hammering if mb is down, but also (silently!)
					 * fails even if we had some number of requests that already completed successfully.
					 * Keeping this for now, but it's subject to change later.
					 **/
					// TOOD: This code is now worthless since all the threads are spawned before queries are
					// made. We'll need to abort elsewhere if we care enough.
					if (m_failed) {
						FB2K_console_formatter() << component_title << ": Musicbrainz query failed.";
						return;
					}

					// async requests
					get_release_info* task = new get_release_info(ids[i], handle_count, &m_release_list, &m_failed, abort);
					if (!simple_thread_pool::instance().enqueue(task)) delete task;
				}
				Sleep(10); // wait for last thread to start
				while (m_release_list.size() < thread_counter && !abort.is_aborting()) {
					// Is there a better way to wait for all the threads to complete?
					status.set_title(PFC_string_formatter() << "Fetching release " << (m_release_list.size() + 1) << " of " << count);
					status.set_progress(1 + m_release_list.size(), count);
					Sleep(10);
				}
				queryingMB = false;
				if (m_failed) {
					FB2K_console_formatter() << component_title << ": Musicbrainz query failed.";
				}
			}
			break;
		case types::albumid:
			{
				Release r = parser(j, handle_count);
				if (r.tracks.size())
				{
					m_release_list.emplace_back(r);
				}
			}
			break;
		}
	}
}
