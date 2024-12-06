#include "logger.h"

int wmain(void) noexcept
{
	wchar_t types[3][logger::_name_max] = { L"tag1", L"tag2", L"tag3" };

	if (logger::get_instance().initialize(L"logs", types, _countof(types)) == false) __debugbreak();

	return 0;
}
