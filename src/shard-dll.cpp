

#define main shardd_main
#include "shardd.cpp"

extern "C" __declspec(dllexport) void SharddEntry() {
	exit(shardd_main(__argc, __argv));
}

#undef main
#define main shard_cli_main
#include "shard-cli.cpp"

extern "C" __declspec(dllexport) void ShardCliEntry() {
	exit(shard_cli_main(__argc, __argv));
}

#undef main
#define main shard_tx_main
#include "shard-tx.cpp"

extern "C" __declspec(dllexport) void ShardTxEntry() {
	exit(shard_tx_main(__argc, __argv));
}

#undef main
#include "qt/bitcoin.cpp"

extern "C" WORD __cdecl __scrt_get_show_window_mode();

extern "C" __declspec(dllexport) void ShardQtEntry() {
	ExitProcess(wWinMain(GetModuleHandle(0), nullptr, _get_wide_winmain_command_line(), __scrt_get_show_window_mode()));
}
