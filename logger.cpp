#include "logger.h"
#include <strsafe.h>

logger logger::_logger;

bool logger::initialize
(
    const wchar_t root[_name_max],
    const wchar_t* types[_type_max],
    unsigned int type_count
) noexcept
{
    if (type_count > _type_max) __debugbreak();

    _type_count = type_count;

    bool name_result = true;

    if (StringCchPrintfW(_root, _name_max, L"%s", root) != S_OK) name_result = false;
    if (CreateDirectoryW(_root, NULL) == ERROR_PATH_NOT_FOUND) __debugbreak();

    for (unsigned int i = 0; i < _type_count; ++i)
        if (StringCchPrintfW(_types[i]._type, _name_max, L"%s", types[i]) != S_OK) name_result = false;

    __time64_t in_time;
    tm in_tm;

    bool path_result = set_path(&in_time, &in_tm);

    if (name_result && path_result) return true;

    return false;
}

void logger::log(unsigned int level, const wchar_t* const type, const wchar_t* format, ...) noexcept
{
    size_t length;
    if (StringCchLengthW(type, _name_max, &length) != S_OK) return;

    unsigned int index = 0;
    for (;;)
    {
        if (wcscmp(type, _types[index]._type) == 0) break;
        ++index;
        if (index >= _type_max) return;
        continue;
    }

    __time64_t in_time;
    tm in_tm;
    bool path_result = set_path(&in_time, &in_tm);

    unsigned int log_count = InterlockedIncrement(&_log_count);

    va_list vl;
    va_start(vl, format);
    AcquireSRWLockExclusive(&_types[index]._lock);

    wchar_t log_message[_log_max];
    StringCchPrintfW(
        log_message, _log_max, _header_format,
        in_tm.tm_year - 2000, in_tm.tm_mon, in_tm.tm_mday,
        in_tm.tm_hour, in_tm.tm_min, in_tm.tm_sec,
        _tag[level], log_count
    );

    if (StringCchVPrintfW(log_message + _header_length, _log_max - _header_length, format, vl) != S_OK)
        ; //todo

#pragma warning(suppress:6031)
    StringCchLengthW(log_message, _log_max, &length);
    log_message[length] = L'\n';

    FILE* file;
    _wfopen_s(&file, _types[index]._path, L"a, ccs=UTF-16LE");
#pragma warning(suppress:6387)
    fwrite(log_message, 1, length * sizeof(wchar_t) + sizeof(wchar_t), file);
#pragma warning(suppress:6387)
    fclose(file);

    ReleaseSRWLockExclusive(&_types[index]._lock);
    va_end(vl);
}

bool logger::set_path(__time64_t* out_time, tm* out_tm) noexcept
{
    _time64(out_time);
    _localtime64_s(out_tm, out_time);

    out_tm->tm_year += 1900;
    out_tm->tm_mon += 1;

    if (_month == out_tm->tm_mon) return true;

    bool result = true;

    if (StringCchPrintfW(_root_path, _path_max, L"%s\\%04d%02d", _root, out_tm->tm_year, out_tm->tm_mon) != S_OK) result = false;
    if (CreateDirectoryW(_root_path, NULL) == ERROR_PATH_NOT_FOUND) __debugbreak();

    for (unsigned int i = 0; i < _type_count; ++i)
        if (StringCchPrintfW(_types[i]._path, _path_max, L"%s\\%04d%02d_%s.log", _root_path, out_tm->tm_year, out_tm->tm_mon, _types[i]._type) != S_OK) result = false;

    InterlockedExchange(&_month, out_tm->tm_mon);

    return result;
}
