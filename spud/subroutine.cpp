// Copyright 2011 naehrwert
// Licensed under the terms of the GNU GPL, version 2
// http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt

#include "common.h"
#include "disasm.h"
#include "spud.h"

subroutine_t *subroutine_find_tsubref(subroutine_t *sr, u32 addr)
{
	int i;

	for(i = 0; i < sr->tsubrefs.size(); i++)
	{
		reference_t *ref = sr->tsubrefs[i];
		//Check if the address matches a subroutine reference.
		if(IIDX_TO_ADDR(ref->subroutine->execr, ref->refidx) == addr)
			return ref->subroutine;
	}

	return NULL;
}

void subroutine_find_refs(ctxt_t *ctxt, subroutine_t *sr)
{
	int i, j;

	//Check all subroutines.
	for(i = 0; i < ctxt->subroutines.size(); i++)
	{
		subroutine_t *tsr = ctxt->subroutines[i];
		//Check all instructions.
		for(j = tsr->sidx; j <= tsr->eidx; j++)
		{
			instr_t *inst = &(tsr->execr->instrs[j]);
			//Check for a relative branch to start of subroutine.
			if(disasm_is_direct_branch(inst) && BRANCH_TARGET(inst) == IIDX_TO_ADDR(sr->execr, sr->sidx))
			{
				DBGPRINTF("subroutines: found ref from sub @ 0x%05x (instr @ 0x%05x) to 0x%05x\n", 
					IIDX_TO_ADDR(tsr->execr, tsr->sidx), IIDX_TO_ADDR(tsr->execr, j), IIDX_TO_ADDR(sr->execr, sr->sidx));
				//Add reference to referenced subroutine.
				reference_t *ref = new reference_t;
				ref->subroutine = tsr;
				ref->refidx = j;
				sr->fsubrefs.push_back(ref);
				//Add reference to referencing subroutine.
				ref = new reference_t;
				ref->subroutine = sr;
				ref->refidx = sr->sidx;
				tsr->tsubrefs.push_back(ref);
			}
		}
	}
}

void subroutine_find_refs_all(ctxt_t *ctxt)
{
	int i;

	//Find references for all subroutines.
	for(i = 0; i < ctxt->subroutines.size(); i++)
		subroutine_find_refs(ctxt, ctxt->subroutines[i]);
}

subroutine_t *subroutine_extract(execr_t *er, int sidx)
{
	int j;

	for(j = sidx; j < er->instrs.size(); j++)
	{
		instr_t *inst = &(er->instrs[j]);
		//Try to find a bi $lr instruction for now.
		//A subroutine could also have more than one bi $lr, check this later.
		if(inst->instr == INSTR_BI && inst->rr.rt == REG_LR)
		{
			subroutine_t *res = new subroutine_t;
			res->execr = er;
			res->sidx = sidx;
			res->eidx = j;
			return res;
		}
	}

	return NULL;
}

void subroutine_extract_all(ctxt_t *ctxt)
{
	int i, j;

	//Check all executable ranges.
	for(i = 0; i < ctxt->execrs.size(); i++)
	{
		execr_t *er = ctxt->execrs[i];
		j = 0;
		//Check all instructions.
		while(j < er->instrs.size())
		{
			//Ignore nops.
			if(er->instrs[j].instr != INSTR_NOP && er->instrs[j].instr != INSTR_LNOP)
			{
				//Extract next subroutine.
				subroutine_t *sr = subroutine_extract(er, j);
				if(sr != NULL)
				{
					DBGPRINTF("subroutines: found subroutine (start 0x%05x, end 0x%05x)\n", 
						IIDX_TO_ADDR(er, sr->sidx), IIDX_TO_ADDR(er, sr->eidx));
					//Move instruction index to subroutine end.
					j = sr->eidx;
					ctxt->subroutines.push_back(sr);
				}
			}
			j++;
		}
	}
}
