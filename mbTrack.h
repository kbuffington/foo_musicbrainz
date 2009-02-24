#pragma once

class mbTrack {
public:
	mbTrack(const char *_title, const char *_id);
	~mbTrack();
	char *getTitle();
	char *getId();

private:
	char *id;
	char *title;
};
