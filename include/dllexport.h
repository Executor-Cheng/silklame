#ifdef WIN32
#define DLLEXPORT __declspec(dllexport)
#define STDCALL __stdcall
#elif __GNUC__
#define DLLEXPORT __attribute__((dllexport))
#define STDCALL __attribute__((__stdcall__))
#endif