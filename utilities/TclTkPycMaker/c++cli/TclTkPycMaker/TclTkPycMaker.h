#pragma once

#ifdef BUILDTCLPYCMAKER
#define EXPORTTCLPYCMAKER __declspec(dllexport)
#else
#define EXPORTTCLPYCMAKER __declspec(dllimport)
#endif


#define FIND_PYTHON_INSTALL_DIR 0xf0
#define LIST_TKINTER_PY_FILES 0xf1
#define MAKE_DST_FOLDER 0xf2
#define COPY_TKINTER_PY_FILES 0xf3
#define COMPILE_TKINTER_PYC_FILES 0xf4
#define COPY_TKINTER_PYC_FILES 0xf5
#define MAKE_ZIP_BALL 0xf6

#define INVALID_REG_PATH 0xd0
#define PYTHON_NOT_FOUND 0xd1
#define PYTHON_RT_INITIALIZED_FAILED 0xd2
#define LIST_TKINTER_PY_FILES_FAILED 0xd3
#define COMPILE_PYC_FAILED 0xd4
#define COPY_TKINTER_PYC_FILES_FAILED 0xd5

#define PROCESS_CODE 0xc0
#define ERROR_CODE 0xc1

#ifdef __cplusplus
extern "C" {
#endif
	EXPORTTCLPYCMAKER int MakeTclTkPycFiles(void (*infoFunc)(const int, const int));
	EXPORTTCLPYCMAKER int CodeMapping(const int, const int, char**);
#ifdef __cplusplus
}
#endif