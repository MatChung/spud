// Copyright 2011 naehrwert
// Licensed under the terms of the GNU GPL, version 2
// http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt

#ifndef _COMMON_H_
#define _COMMON_H_

#ifdef _DEBUG
#define DBGPRINTF(...) printf(__VA_ARGS__)
#endif

void fail(const char *a, ...);

#endif
