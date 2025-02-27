#pragma once

#ifdef BASEMODULE_PLUGIN_EXPORT
#define BASEMODULE_PLUGIN_API __declspec(dllexport)
#else
#define BASEMODULE_PLUGIN_API __declspec(dllimport)
#endif


#ifdef __cplusplus
extern "C"
{
#endif
	typedef void*   PMODULE;

	BASEMODULE_PLUGIN_API int GetModulesCount();
	BASEMODULE_PLUGIN_API PMODULE CreateModule(int nIndex);
	BASEMODULE_PLUGIN_API void ReleaseModule(PMODULE p);

#ifdef __cplusplus
};
#endif
