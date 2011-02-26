// Copyright 2011 naehrwert
// Licensed under the terms of the GNU GPL, version 2
// http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt

#ifndef _SUBROUTINE_H_
#define _SUBROUTINE_H_

#include "spud.h"

subroutine_t *subroutine_find_tsubref(subroutine_t *sr, u32 addr);
void subroutine_find_refs(ctxt_t *ctxt, subroutine_t *sr);
void subroutine_find_refs_all(ctxt_t *ctxt);
subroutine_t *subroutine_extract(execr_t *er, int sidx);
void subroutine_extract_all(ctxt_t *ctxt);

#endif
