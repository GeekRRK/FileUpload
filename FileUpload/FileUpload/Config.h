#pragma once
class Config
{
public:
	Config(void);
	~Config(void);
public:
	int interval;
	int before;
	int protocol;
	CString ip;
	int port;
	CString username;
	CString passwd;
	CString nearpath;
	CString remotepath;
	CString updatepath;
	CString version;
};