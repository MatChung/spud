// Copyright 2011 naehrwert
// Licensed under the terms of the GNU GPL, version 2
// http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt

#ifndef _OUTPUT_H_
#define _OUTPUT_H_

#include <stdio.h>

#include "spud.h"

void output_write_subroutine(FILE *fp, subroutine_t *sr);
void output_write(ctxt_t *ctxt, const char *file);

#endif
