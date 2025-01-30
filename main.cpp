#include "logger.h"

int wmain(void) noexcept
{
	const wchar_t* types[3] = { L"tag1", L"tag2", L"tag3" };

	if (logger::get_instance().initialize(L"logs", types, _countof(types)) == false) __debugbreak();

	LOG(logger::_level_debug, L"tag1", L"log1");
	LOG(logger::_level_debug, L"tag1", L"log2 %d %d %d", 1, 2, 3);

	LOG(logger::_level_debug, L"tag2", L"%s", L"log1");
	LOG(logger::_level_debug, L"tag2", L"%s %lf %lf", L"log2", 0.5, 0.25);

	LOG(
		logger::_level_debug,
		L"tag3",
		L"log1 123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890\n"
	);

	return 0;
}
