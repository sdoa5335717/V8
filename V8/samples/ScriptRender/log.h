//
// Created by JerryZhu on 2018/5/18.
//
//#include <android/log.h>
#include <windows.h>

#ifndef V8DEMO_LOG_H
#define V8DEMO_LOG_H

#define  LOG_TAG    "v8demojni"

//#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
//#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

inline void WriteError(char* pszstr, ...)
{
	// this function prints out the error string to the error file

	char buffer[1024]; // working buffer

	va_list arglist; // variable argument list

	// make sure both the error file and string are valid
	if (!pszstr)
		return;

	// print out the string using the variable number of arguments on stack
	va_start(arglist, pszstr);
	vsprintf_s(buffer, pszstr, arglist);
	va_end(arglist);
#ifdef _CONSOLE
	printf(buffer);
	printf("\n");
#else
	OutputDebugString(buffer);
	OutputDebugString("\n");
#endif
}

//#define LOGI(...) WriteError(...)

#endif //V8DEMO_LOG_H

