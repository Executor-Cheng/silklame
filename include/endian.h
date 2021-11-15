#ifdef _WIN32
#define _CRT_SECURE_NO_DEPRECATE    1
#endif

#define MAX_BYTES_PER_FRAME     1024
#define MAX_INPUT_FRAMES        5
#define MAX_FRAME_LENGTH        480
#define FRAME_LENGTH_MS         20
#define MAX_API_FS_KHZ          48
#define MAX_LBRR_DELAY          2

#ifdef _SYSTEM_IS_BIG_ENDIAN
short swap_short_endian(short value);

void swap_endian(short vec[], long len);
#endif