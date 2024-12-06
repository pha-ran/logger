#pragma once

#include <windows.h>

class logger
{
public:
	static constexpr unsigned int _level_debug = 0;
	static constexpr unsigned int _level_error = 1;
	static constexpr unsigned int _level_system = 2;
	static constexpr wchar_t _tag[3][4] = { L"DBG", L"ERR", L"SYS" };

public:
	static constexpr unsigned int _name_max = 32;
	static constexpr unsigned int _path_max = 128;
	static constexpr unsigned int _type_count = 16;
	static constexpr unsigned int _log_max = 256;

private:
	static constexpr wchar_t _header[] = L"[YY/MM/DD][HH:MM:SS][TAG][LOGCOUNT]";
	static constexpr unsigned int _header_size = sizeof(_header) - sizeof(wchar_t);
	static constexpr unsigned int _header_count = _header_size / 2;

private:
	inline logger(void) noexcept
		: _level(0), _reserved(0), _count(0), _month(0)
	{
		_root[0] = L'\0';
		_root_path[0] = L'\0';

		for (unsigned int i = 0; i < _type_count; ++i)
		{
			_types[i]._type[0] = L'\0';
			_types[i]._path[0] = L'\0';
			InitializeSRWLock(&_types[i]._lock);
		}
	}

private:
	static logger _logger;

private:
	unsigned int _level;
	unsigned int _reserved;

	volatile unsigned int _count;
	volatile unsigned int _month;

	wchar_t _root[_name_max];
	wchar_t _root_path[_path_max];

	struct {
		wchar_t _type[_name_max];
		wchar_t _path[_path_max];
		SRWLOCK _lock;
	} _types[_type_count];

};
