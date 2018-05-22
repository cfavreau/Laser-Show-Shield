#include <stdio.h>
#include <string.h>


#ifndef __TRACE_H__
#define __TRACE_H__

void EnableTrace(bool enable);

bool TraceIsEnabled(void);

void OpenTraceFile();

void CloseTraceFile();

void Trace(const char *format, ...);

#endif // __TRACE_H__