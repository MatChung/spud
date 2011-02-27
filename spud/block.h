// Copyright 2011 naehrwert
// Licensed under the terms of the GNU GPL, version 2
// http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt

#ifndef _BLOCK_H_
#define _BLOCK_H_

#include "disasm.h"

#define BLOCKSADDR(bl) IIDX2ADDR(bl->sr->er, bl->sidx)
#define BLOCKEADDR(bl) IIDX2ADDR(bl->sr->er, bl->eidx)

void block_extract_all(subroutine_t *sr);

#endif
