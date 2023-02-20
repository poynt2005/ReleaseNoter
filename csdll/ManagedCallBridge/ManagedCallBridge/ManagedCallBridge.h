#pragma once


#ifdef BUILDUNMANGEDAPI
#define EXPORTUNMANGEDAPI __declspec(dllexport)
#else
#define EXPORTUNMANGEDAPI __declspec(dllimport)
#endif


#ifdef __cplusplus
extern "C" {
#endif
	EXPORTUNMANGEDAPI int NativeClipboardCopier(const char* inputHTML);
	EXPORTUNMANGEDAPI int NativeClipboardCopierPath(const char*, const char*);
#ifdef __cplusplus
}
#endif