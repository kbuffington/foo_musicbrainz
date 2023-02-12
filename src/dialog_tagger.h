#pragma once

#include <sstream>

namespace mb
{
	static const CDialogResizeHelper::Param resize_data[] =
	{
		{ IDOK, 1, 1, 1, 1 },
		{ IDCANCEL, 1, 1, 1, 1 },
		{ IDC_LINK_URL, 0, 1, 1, 1 },
		{ IDC_GROUPBOX_RELEASE, 0, 0, 1, 0},
		{ IDC_LIST_RELEASE, 0, 0, 1, 0},
		{ IDC_GROUPBOX_TRACK, 0, 0, 1, 1},
		{ IDC_LIST_TRACK, 0, 0, 1, 1},
		{ IDC_GROUPBOX_RELEASE_INFO, 1, 0, 1, 0},
		{ IDC_LABEL_ARTIST, 1, 0, 1, 0 },
		{ IDC_EDIT_ARTIST, 1, 0, 1, 0 },
		{ IDC_LABEL_ALBUM, 1, 0, 1, 0 },
		{ IDC_EDIT_ALBUM, 1, 0, 1, 0 },
		{ IDC_LABEL_DATE, 1, 0, 1, 0 },
		{ IDC_EDIT_DATE, 1, 0, 1, 0 },
		{ IDC_LABEL_ORIGINAL_RELEASE_DATE, 1, 0, 1, 0 },
		{ IDC_EDIT_ORIGINAL_RELEASE_DATE, 1, 0, 1, 0 },
		{ IDC_LABEL_LABEL, 1, 0, 1, 0 },
		{ IDC_EDIT_LABEL, 1, 0, 1, 0 },
		{ IDC_LABEL_CATALOG, 1, 0, 1, 0 },
		{ IDC_EDIT_CATALOG, 1, 0, 1, 0 },
		{ IDC_LABEL_BARCODE, 1, 0, 1, 0 },
		{ IDC_EDIT_BARCODE, 1, 0, 1, 0 },
		{ IDC_LABEL_TYPE, 1, 0, 1, 0 },
		{ IDC_COMBO_TYPE, 1, 0, 1, 0 },
		{ IDC_LABEL_STATUS, 1, 0, 1, 0 },
		{ IDC_COMBO_STATUS, 1, 0, 1, 0 },
	};

	static const CRect resize_min_max(660, 425, 0, 0);

	class dialog_tagger : public CDialogImpl<dialog_tagger>, private IListControlOwnerDataSource
	{
	public:
		dialog_tagger(const std::vector<Release>& release_list, metadb_handle_list_cref handles) :
			m_release_list(release_list),
			m_handles(handles),
			m_resizer(resize_data, resize_min_max),
			track_list(this),
			current_release(0),
			current_disc(0),
			handle_count(handles.get_count()),
			release_count(release_list.size())
		{}

		BEGIN_MSG_MAP_EX(dialog_tagger)
			CHAIN_MSG_MAP_MEMBER(m_resizer)
			MSG_WM_INITDIALOG(OnInitDialog)
			MSG_WM_CLOSE(OnClose)
			COMMAND_ID_HANDLER_EX(IDOK, OnOk)
			COMMAND_ID_HANDLER_EX(IDCANCEL, OnCancel)
			NOTIFY_HANDLER_EX(IDC_LINK_URL, NM_CLICK, OnLink)
			NOTIFY_HANDLER_EX(IDC_LINK_URL, NM_RETURN, OnLink)

			COMMAND_HANDLER_EX(IDC_COMBO_TYPE, CBN_SELENDOK, OnTypeChange)
			COMMAND_HANDLER_EX(IDC_COMBO_STATUS, CBN_SELENDOK, OnStatusChange)
			COMMAND_HANDLER_EX(IDC_COMBO_DISC, CBN_SELENDOK, OnDiscChange)

			COMMAND_HANDLER_EX(IDC_EDIT_ARTIST, EN_UPDATE, OnArtistUpdate)
			COMMAND_HANDLER_EX(IDC_EDIT_ALBUM, EN_UPDATE, OnAlbumUpdate)
			COMMAND_HANDLER_EX(IDC_EDIT_DATE, EN_UPDATE, OnDateUpdate)
			COMMAND_HANDLER_EX(IDC_EDIT_ORIGINAL_RELEASE_DATE, EN_UPDATE, OnOriginalDateUpdate)
			COMMAND_HANDLER_EX(IDC_EDIT_LABEL, EN_UPDATE, OnLabelUpdate)
			COMMAND_HANDLER_EX(IDC_EDIT_CATALOG, EN_UPDATE, OnCatalogUpdate)
			COMMAND_HANDLER_EX(IDC_EDIT_BARCODE, EN_UPDATE, OnBarcodeUpdate)
		END_MSG_MAP()

		enum { IDD = IDD_TAGGER };

		enum columns
		{
			artist_column,
			release_column,
			date_column,
			label_column,
			media_column,
			discs_column
		};

		BOOL OnInitDialog(CWindow, LPARAM)
		{
			CenterWindow();

			release_list.CreateInDialog(*this, IDC_LIST_RELEASE);
			release_list.SetSelectionModeSingle();
			release_list.onSelChange = [&]()
			{
				size_t idx = release_list.GetFocusItem();
				if (idx != current_release && idx < release_count)
				{
					current_release = idx;
					UpdateRelease();
				}
			};

			track_list.CreateInDialog(*this, IDC_LIST_TRACK);

			type_combo = GetDlgItem(IDC_COMBO_TYPE);
			status_combo = GetDlgItem(IDC_COMBO_STATUS);
			disc_combo = GetDlgItem(IDC_COMBO_DISC);

			album_artist_edit = GetDlgItem(IDC_EDIT_ARTIST);
			album_edit = GetDlgItem(IDC_EDIT_ALBUM);
			date_edit = GetDlgItem(IDC_EDIT_DATE);
			original_release_date_edit = GetDlgItem(IDC_EDIT_ORIGINAL_RELEASE_DATE);
			label_edit = GetDlgItem(IDC_EDIT_LABEL);
			catalog_edit = GetDlgItem(IDC_EDIT_CATALOG);
			barcode_edit = GetDlgItem(IDC_EDIT_BARCODE);

			url = GetDlgItem(IDC_LINK_URL);

			auto DPI = release_list.GetDPI();

			// Add release list columns
			release_list.AddColumnAutoWidth("Artist");
			release_list.AddColumnAutoWidth("Release");
			release_list.AddColumn("Date/Country", MulDiv(90, DPI.cx, 96));
			release_list.AddColumnAutoWidth("Label(s)/Cat#");
			release_list.AddColumn("Media", MulDiv(85, DPI.cx, 96));
			release_list.AddColumn("Discs", MulDiv(40, DPI.cx, 96));

			// Add release list rows
			release_list.SetItemCount(release_count);
			for (size_t i = 0; i < release_count; ++i)
			{
				release_list.SetItemText(i, artist_column, m_release_list[i].album_artist);
				release_list.SetItemText(i, release_column, m_release_list[i].title);
				release_list.SetItemText(i, date_column, slasher(m_release_list[i].date, m_release_list[i].country));
				str8 label = getMultiValueString(m_release_list[i].label);
				release_list.SetItemText(i, label_column, slasher(label, m_release_list[i].catalog));
				release_list.SetItemText(i, media_column, media_thingy(m_release_list[i].tracks));
				release_list.SetItemText(i, discs_column, std::to_string(m_release_list[i].tracks[0].totaldiscs).c_str());
			}

			// Adding track list columns
			track_list.AddColumn("Filename", MulDiv(230, DPI.cx, 96));
			track_list.AddColumn("#", MulDiv(40, DPI.cx, 96), HDF_RIGHT);
			track_list.AddColumn("Disc Subtitle", MulDiv(120, DPI.cx, 96));
			track_list.AddColumnAutoWidth("Title");

			// Fixed combo boxes
			for (pfc::stringp i : release_group_types)
			{
				type_combo.AddString(string_wide_from_utf8_fast(i));
			}

			for (pfc::stringp i : release_statuses)
			{
				status_combo.AddString(string_wide_from_utf8_fast(i));
			}

			UpdateRelease();

			// Don't allow editing if disabled in Preferences
			const bool label_enabled = prefs::check::write_label_info.get_value();
			label_edit.EnableWindow(label_enabled);
			catalog_edit.EnableWindow(label_enabled);
			barcode_edit.EnableWindow(label_enabled);
			const bool use_orig_date = prefs::check::use_orig_date.get_value();
			original_release_date_edit.EnableWindow(!use_orig_date);
			type_combo.EnableWindow(prefs::check::write_albumtype.get_value());
			status_combo.EnableWindow(prefs::check::write_albumstatus.get_value());

			ShowWindow(SW_SHOW);
			return TRUE;
		}

		LRESULT OnLink(LPNMHDR pnmh)
		{
			ShellExecute(nullptr, L"open", ((PNMLINK)pnmh)->item.szUrl, nullptr, nullptr, SW_SHOW);
			return 0;
		}

		void OnAlbumUpdate(UINT, int, CWindow)
		{
			uGetWindowText(album_edit, m_release_list[current_release].title);
			release_list.SetItemText(current_release, release_column, m_release_list[current_release].title);
		}

		void OnArtistUpdate(UINT, int, CWindow)
		{
			uGetWindowText(album_artist_edit, m_release_list[current_release].album_artist);
			release_list.SetItemText(current_release, artist_column, m_release_list[current_release].album_artist);
		}

		void OnBarcodeUpdate(UINT, int, CWindow)
		{
			uGetWindowText(barcode_edit, m_release_list[current_release].barcode);
		}

		void OnCancel(UINT, int, CWindow)
		{
			DestroyWindow();
		}

		void OnCatalogUpdate(UINT, int, CWindow)
		{
			uGetWindowText(catalog_edit, m_release_list[current_release].catalog);
			str8 label = getMultiValueString(m_release_list[current_release].label);
			release_list.SetItemText(current_release, label_column, slasher(label, m_release_list[current_release].catalog));
		}

		void OnClose()
		{
			DestroyWindow();
		}

		void OnDateUpdate(UINT, int, CWindow)
		{
			if (!prefs::check::use_orig_date.get_value()) {
				uGetWindowText(date_edit, m_release_list[current_release].date);
				release_list.SetItemText(current_release, date_column, slasher(m_release_list[current_release].date, m_release_list[current_release].country));
			}
			else {
				uGetWindowText(date_edit, m_release_list[current_release].original_release_date);
			}
		}

		void OnDiscChange(UINT, int, CWindow)
		{
			current_disc = disc_combo.GetCurSel();
			UpdateDisc();
		}

		void OnOriginalDateUpdate(UINT, int, CWindow)
		{
			uGetWindowText(original_release_date_edit, m_release_list[current_release].original_release_date);
		}

		void OnLabelUpdate(UINT, int, CWindow)
		{
			str8 labelStr;
			uGetWindowText(label_edit, labelStr);
			
			std::vector<std::string> labels = parseMultiValueString(labelStr);
			m_release_list[current_release].label.remove_all();
			for (auto& label : labels) {
				m_release_list[current_release].label.add_item(label.c_str());
			}
			release_list.SetItemText(current_release, label_column, slasher(labelStr, m_release_list[current_release].catalog));
		}

		/** Takes a string with 1 or more semi-colon separated values and returns a vector containing each separated value */
		std::vector<std::string> parseMultiValueString(str8 mvString) {
			std::stringstream ss(mvString.get_ptr());       // Insert the string into a stream
			std::string buf;
			std::vector<std::string> tokens; // Create vector to hold our values
			regex trim("^\\s+|\\s+$");

			while (getline(ss, buf, ';')) {
				std::string trimmedVal = regex_replace(buf, trim, "");
				tokens.push_back(trimmedVal);
			}

			return tokens;
		}

		/** Separates multi-value field into semi-colon delimited string */
		str8 getMultiValueString(pfc::string_list_impl field) {
			str8 buf = "";
			for (size_t i = 0; i < field.get_count(); ++i) {
				if (i > 0) {
					buf += "; ";
				}
				buf += field[i];
			}
			return buf;
		}

		void OnOk(UINT, int, CWindow)
		{
			tagger(m_handles, m_release_list[current_release], current_disc);
			DestroyWindow();
		}

		void OnStatusChange(UINT, int, CWindow)
		{
			m_release_list[current_release].status = get_status_str(status_combo.GetCurSel());
		}

		void OnTypeChange(UINT, int, CWindow)
		{
			m_release_list[current_release].primary_type = get_type_str(type_combo.GetCurSel());
		}

		void UpdateDisc()
		{
			if (track_list.TableEdit_IsActive())
			{
				track_list.TableEdit_Abort(false);
			}
			track_list.ReloadData();
		}

		void UpdateRelease()
		{
			uSetWindowText(album_artist_edit, m_release_list[current_release].album_artist);
			uSetWindowText(album_edit, m_release_list[current_release].title);
			if (prefs::check::use_orig_date.get_value()) {
				uSetWindowText(date_edit, m_release_list[current_release].original_release_date);
			}
			else {
				uSetWindowText(date_edit, m_release_list[current_release].date);
				uSetWindowText(original_release_date_edit, m_release_list[current_release].original_release_date);
			}
			uSetWindowText(label_edit, getMultiValueString(m_release_list[current_release].label));
			uSetWindowText(catalog_edit, m_release_list[current_release].catalog);
			uSetWindowText(barcode_edit, m_release_list[current_release].barcode);

			type_combo.SetCurSel((int) get_type_index(m_release_list[current_release].primary_type));
			status_combo.SetCurSel((int) get_status_index(m_release_list[current_release].status));

			auto DPI = track_list.GetDPI();
			track_list.ResizeColumn(2, m_release_list[current_release].tracks[0].totaldiscs > 1 ? MulDiv(120, DPI.cx, 96) : 0);

			if (m_release_list[current_release].is_various)
			{
				if (track_list.GetColumnCount() == 4)
				{
					track_list.AddColumn("Track Artist", MulDiv(250, DPI.cx, 96));
				}
			}
			else if (track_list.GetColumnCount() == 5)
			{
				track_list.DeleteColumn(4, false);
			}

			disc_combo.ResetContent();

			size_t partial_lookup_matches = m_release_list[current_release].partial_lookup_matches;
			if (partial_lookup_matches > 0)
			{
				for (size_t i = 0; i < partial_lookup_matches; ++i)
				{
					disc_combo.AddString(string_wide_from_utf8_fast(PFC_string_formatter() << "Disc " << m_release_list[current_release].tracks[i * handle_count].discnumber << " of " << m_release_list[current_release].tracks[i * handle_count].totaldiscs));
				}
			}
			else
			{
				size_t totaldiscs = m_release_list[current_release].tracks[0].totaldiscs;
				if (totaldiscs == 1)
				{
					disc_combo.AddString(L"Showing the only disc");
				}
				else
				{
					disc_combo.AddString(string_wide_from_utf8_fast(PFC_string_formatter() << "Showing all " << totaldiscs << " discs"));
				}
			}

			current_disc = 0;
			disc_combo.SetCurSel((int) current_disc);
			UpdateDisc();

			str8 url_str = PFC_string_formatter() << "<a href=\"" << prefs::get_server() << "/release/" << m_release_list[current_release].albumid << "\">MusicBrainz release page</a>";
			uSetWindowText(url, url_str);
		}

	private:
		bool listCanSelectItem(ctx_t, size_t) override
		{
			return false;
		}

		bool listIsColumnEditable(ctx_t, size_t sub_item) override
		{
			return sub_item > 1;
		}

		str8 listGetSubItemText(ctx_t, size_t item, size_t sub_item) override
		{
			size_t track_idx = item + (current_disc * handle_count);
			switch (sub_item)
			{
			case 0:
				return pfc::string_filename_ext(m_handles[item]->get_path());
			case 1:
				return PFC_string_formatter() << m_release_list[current_release].tracks[track_idx].discnumber << "." << m_release_list[current_release].tracks[track_idx].tracknumber;
			case 2:
				if (m_release_list[current_release].tracks[track_idx].tracknumber == 1 && m_release_list[current_release].tracks[track_idx].totaldiscs > 1)
				{
					return m_release_list[current_release].tracks[track_idx].subtitle;
				}
				return "";
			case 3:
				return m_release_list[current_release].tracks[track_idx].title;
			case 4:
				return m_release_list[current_release].tracks[track_idx].artist;
			default:
				return "";
			}
		}

		size_t listGetItemCount(ctx_t) override
		{
			return handle_count;
		}

		void listSetEditField(ctx_t, size_t item, size_t sub_item, const char* value) override
		{
			size_t track_idx = item + (current_disc * handle_count);
			switch (sub_item)
			{
			case 2:
				m_release_list[current_release].tracks[track_idx].subtitle = value;
				break;
			case 3:
				m_release_list[current_release].tracks[track_idx].title = value;
				break;
			case 4:
				m_release_list[current_release].tracks[track_idx].artist = value;
				break;
			}
		}

		void listSubItemClicked(ctx_t, size_t item, size_t sub_item) override
		{
			size_t track_idx = item + (current_disc * handle_count);
			if ((sub_item == 2 && m_release_list[current_release].tracks[track_idx].tracknumber == 1 && m_release_list[current_release].tracks[track_idx].totaldiscs > 1) || sub_item > 2)
			{
				track_list.TableEdit_Start(item, sub_item);
			}
		}

		CComboBox disc_combo;
		CComboBox type_combo;
		CComboBox status_combo;
		CDialogResizeHelper m_resizer;
		CEdit album_artist_edit;
		CEdit album_edit;
		CEdit date_edit;
		CEdit original_release_date_edit;
		CEdit label_edit;
		CEdit catalog_edit;
		CEdit barcode_edit;
		CListControlSimple release_list;
		CListControlOwnerData track_list;
		CWindow url;
		metadb_handle_list m_handles;
		std::vector<Release> m_release_list;
		size_t current_release;
		size_t current_disc;
		size_t handle_count;
		size_t release_count;
	};
}
