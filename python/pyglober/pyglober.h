#ifndef __PY_GLOBER_H__
#define __PY_GLOBER_H__

#ifdef BUILDPYGLOBER
#define EXPORTPYGLOBER __declspec(dllexport)
#else
#define EXPORTPYGLOBER __declspec(dllimport)
#endif

#ifdef __cplusplus
extern "C"
{
#endif

    /** Initialize the python runtime, can only initialize once */
    EXPORTPYGLOBER int PyRTInit();

    /** Finialize the python runtime, and it cannot be re-initialized */
    EXPORTPYGLOBER int PYRTFinal();

    EXPORTPYGLOBER int PyGlobber(
        /** In const char* pathname cannot be null */
        const char *,
        /** In const char* root_dir(optional) can be NULL */
        const char *,
        /** In void* dir_fd(optional) must be NULL in Windows */
        void *,
        /** In const int is recursive (boolean) */
        const int,
        /** In const int is include_hidden (boolean) */
        const int,
        /** Out char** string array indicate the glob result*/
        char ***,
        /** Out int* N, how many file is globed */
        int *);

#ifdef __cplusplus
}
#endif

#endif