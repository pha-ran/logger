#include "logger.h"
#include <strsafe.h>

logger logger::_logger;

bool logger::initialize
(
    const wchar_t root[_name_max],
    const wchar_t types[_type_max][_name_max],
    unsigned int type_count
) noexcept
{
    if (type_count >= _type_max) __debugbreak();

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
