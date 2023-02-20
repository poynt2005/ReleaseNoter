#pragma once

#ifdef BUILDUNMANGEDFINDERAPI
#define EXPORTUNMANGEDFINDERAPI __declspec(dllexport)
#else
#define EXPORTUNMANGEDFINDERAPI __declspec(dllimport)
#endif

#ifdef __cplusplus
extern "C" {
#endif

	EXPORTUNMANGEDFINDERAPI int Finder(const char*, char***, int*);

#ifdef __cplusplus
}
#endif

