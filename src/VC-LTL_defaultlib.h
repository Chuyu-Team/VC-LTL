
#define EXPAND_LINKER_OPTION_SD(...)  __VA_ARGS__

#define LINKER_OPTION_SD  "/defaultlib"




GENERATE_LINKER_OPTION(MT, "libucrt_shared.lib")

GENERATE_LINKER_OPTION(SD, "vc.lib")
GENERATE_LINKER_OPTION(SD, "msvcrt_Platform.lib")
GENERATE_LINKER_OPTION(SD, "legacy_stdio_definitions.lib")
