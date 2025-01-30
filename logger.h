#pragma once

#include <windows.h>
#include <time.h>

#define LOG(level, type, format, ...)\
do\
{\
    if (level >= logger::get_instance().get_level())\
	{\
		logger::get_instance().log(level, type, format, ##__VA_ARGS__);\
	}\
} while(0)

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
	static constexpr unsigned int _type_max = 16;
	static constexpr unsigned int _log_max = 256;

public:
	inline static logger& get_instance(void) noexcept
	{
		return _logger;
	}

public:
	inline void set_level(unsigned int level) noexcept
	{
		_level = level;
	}

	inline unsigned int get_level(void) const noexcept
	{
		return _level;
	}

public:
	bool initialize(
		const wchar_t root[_name_max],
		const wchar_t* types[_type_max],
		unsigned int type_count
	) noexcept;

	void log(unsigned int level, const wchar_t* const type, const wchar_t* format, ...) noexcept;

private:
	static constexpr wchar_t _header[] = L"[YY/MM/DD][HH:MM:SS][TAG][LOGCOUNT]";
	static constexpr wchar_t _header_format[] = L"[%02d/%02d/%02d][%02d:%02d:%02d][%s][%08d]";
	static constexpr unsigned int _header_length = (sizeof(_header) - sizeof(wchar_t)) / 2;

private:
	inline logger(void) noexcept
		: _level(0), _type_count(0), _log_count(0), _month(0)
	{
		_root[0] = L'\0';
		_root_path[0] = L'\0';

		for (unsigned int i = 0; i < _type_max; ++i)
		{
			_types[i]._type[0] = L'\0';
			_types[i]._path[0] = L'\0';
			InitializeSRWLock(&_types[i]._lock);
		}
	}

private:
	bool set_path(__time64_t* out_time, tm* out_tm) noexcept;

private:
	static logger _logger;

private:
	unsigned int _level;
	unsigned int _type_count;

	volatile unsigned int _log_count;
	volatile unsigned int _month;

	wchar_t _root[_name_max];
	wchar_t _root_path[_path_max];

	struct {
		wchar_t _type[_name_max];
		wchar_t _path[_path_max];
		SRWLOCK _lock;
	} _types[_type_max];

};
