#include <windows.h>
#include "trace.h"

// Turn Debug Trace OFF by default
bool m_bEnableTrace = false;
FILE *pTraceFile = NULL;

void EnableTrace(bool enable)
{
	m_bEnableTrace = enable;
}

bool TraceIsEnabled(void)
{
	return m_bEnableTrace;
}

void OpenTraceFile()
{
	if (!pTraceFile)
	{
		pTraceFile = fopen("trace.txt","w");
	}
}

void CloseTraceFile()
{
	if (pTraceFile)
	{
		fclose(pTraceFile);
	}
}

void Trace(const char *format, ...)
{
	if (!m_bEnableTrace) return;
	
	va_list list;
	va_start(list, format);

	// Attempt to open the trace file if it hasn't been opened yet
	OpenTraceFile();
	
	if (pTraceFile)
	{
		// Print something in the file
		vfprintf(pTraceFile, format, list);
		// Flush the data to the file
		fflush(pTraceFile);
	}
}
