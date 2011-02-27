// Copyright 2011 naehrwert
// Licensed under the terms of the GNU GPL, version 2
// http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt

#include <stdlib.h>

#include "common.h"
#include "spud.h"
#include "subroutine.h"
#include "block.h"

static block_t *_block_build_cfg(block_t *start)
{
	return NULL;
}

static block_t *_block_extract(subroutine_t *sr, unsigned int sidx)
{
	unsigned int i;

	for(i = sidx; i <= sr->eidx; i++)
	{
		instr_t *inst = &(sr->er->instrs[i]);
		//Check if a direct branch is inside the subroutine or 
		//if we are at the end of the subroutine because we need 
		//at least one block per subroutine.
		if((disasm_is_direct_branch(inst) && 
			BRANCH_TARGET(inst) >= SUBSADDR(sr) &&
			BRANCH_TARGET(inst) <= SUBEADDR(sr)) ||
			i == sr->eidx)
		{
#ifdef _DEBUG
			//DBGPRINTF("block: exit instruction: ");
			//disasm_print_instr(stdout, inst, true);
			//printf("\n");
#endif
			block_t *res = new block_t;
			res->sr = sr;
			res->sidx = sidx;
			res->eidx = i;
			res->exitinst = inst;
			res->next = NULL;
			return res;
		}
	}

	return NULL;
}

void block_extract_all(subroutine_t *sr)
{
	unsigned int i;

	i = sr->sidx;
	while(i <= sr->eidx)
	{
		block_t *bl = _block_extract(sr, i);
		if(bl != NULL)
		{
			//DBGPRINTF("block: found block @ 0x%05x (end @ 0x%05x)\n", BLOCKSADDR(bl), BLOCKEADDR(bl));
			sr->bblocks.push_back(bl);
			//Move instruction to block end.
			i = bl->eidx;
		}
		i++;
	}
}
