// Copyright 2011 naehrwert
// Licensed under the terms of the GNU GPL, version 2
// http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt

#ifndef _SUBROUTINE_H_
#define _SUBROUTINE_H_

#include "spud.h"

#define SUBSADDR(sr) IIDX2ADDR(sr->execr, sr->sidx)
#define SUBEADDR(sr) IIDX2ADDR(sr->execr, sr->eidx)

subroutine_t *subroutine_find_tsubref(subroutine_t *sr, u32 addr);
void subroutine_extract_all(ctxt_t *ctxt);

#endif
