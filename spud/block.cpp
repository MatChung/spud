// Copyright 2011 naehrwert
// Licensed under the terms of the GNU GPL, version 2
// http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt

#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "config.h"
#include "spud.h"
#include "disasm.h"
#include "subroutine.h"
#include "block.h"

static block_t *_block_copy(block_t *bl)
{
	block_t *res = new block_t;
	memcpy(res, bl, sizeof(block_t));
	return res;
}

static void _block_split_after(subroutine_t *sr, u32 target, list<block_t *>::iterator &it)
{
	block_t *tbl = *it;

	//Create two new blocks.
	block_t *first = _block_copy(tbl);
	block_t *second = _block_copy(tbl);
	
	//First block ends with the instruction previous the target one.
	first->exitinst = disasm_get_instr(sr->er, target - INSTR_SIZE);
	first->eidx = first->exitinst->idx;
	
	//Second block starts with target instruction.
	second->sidx = disasm_get_instr(sr->er, target)->idx;

	//Replace the blocks in the original list.
	sr->blocks.insert(it, first);
	sr->blocks.insert(it, second);
	sr->blocks.erase(it);

	DBGPRINTF("block: split block @ 0x%05x into block @ 0x%05x and block @ 0x%05x\n",
		BLOCKSADDR(tbl), BLOCKSADDR(first), BLOCKSADDR(second));
}

static void _block_split_blocks(subroutine_t *sr)
{
	//Outer and inner iterators.
	list<block_t *>::iterator oit, iit;

	for(oit = sr->blocks.begin(); oit != sr->blocks.end(); ++oit)
	{
		block_t *bl = *oit;

		if(disasm_is_direct_branch(bl->exitinst))
		{
			u32 target = BRANCH_TARGET(bl->exitinst);

			//Check for a branch target inside another block.
			for(iit = sr->blocks.begin(); iit != sr->blocks.end(); ++iit)
			{
				block_t *tbl = *iit;

				//Don't check if the target is the first instruction.
				if(target > BLOCKSADDR(tbl) && target <= BLOCKEADDR(tbl))
				{
					if(target > bl->exitinst->address)
					{
						_block_split_after(sr, target, iit);
						break;
					}
					else
					{
						DBGPRINTF("block: todo: backward branches! (0x%05x)\n", target);
					}
				}
			}
		}
	}
}

static block_t *_block_extract(subroutine_t *sr, unsigned int sidx)
{
	unsigned int i;

	for(i = sidx; i <= sr->eidx; i++)
	{
		instr_t *inst = &(sr->er->instrs[i]);
		//Check if a branch occurs to a target inside the subroutine 
		//or if we are at the end of the subroutine because we need 
		//at least one block per subroutine.
		if((disasm_is_direct_branch(inst) && 
			BRANCH_TARGET(inst) >= SUBSADDR(sr) &&
			BRANCH_TARGET(inst) <= SUBEADDR(sr)) ||
			i == sr->eidx)
		{
			block_t *res = new block_t;
			res->sr = sr;
			res->sidx = sidx;
			res->eidx = i;
			res->exitinst = inst;
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
			DBGPRINTF("block: found block @ 0x%05x (end @ 0x%05x)\n", BLOCKSADDR(bl), BLOCKEADDR(bl));
			sr->blocks.push_back(bl);
			//Move instruction to block end.
			i = bl->eidx;
		}
		i++;
	}

	//Split all blocks.
	_block_split_blocks(sr);
}
