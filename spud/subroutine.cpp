// Copyright 2011 naehrwert
// Licensed under the terms of the GNU GPL, version 2
// http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt

#include "common.h"
#include "disasm.h"
#include "spud.h"
#include "subroutine.h"
#include "block.h"

static void _subroutine_find_refs(ctxt_t *ctxt, subroutine_t *sr)
{
	unsigned int i, j;

	//Check all subroutines.
	for(i = 0; i < ctxt->subroutines.size(); i++)
	{
		subroutine_t *tsr = ctxt->subroutines[i];
		//Check all instructions.
		for(j = tsr->sidx; j <= tsr->eidx; j++)
		{
			instr_t *inst = &(tsr->er->instrs[j]);
			//Check for a relative branch to start of subroutine.
			if(disasm_is_direct_branch(inst) && BRANCH_TARGET(inst) == SUBSADDR(sr))
			{
				DBGPRINTF("subroutine: found ref from sub @ 0x%05x (instr @ 0x%05x) to 0x%05x\n", 
					SUBSADDR(tsr), IIDX2ADDR(tsr->er, j), SUBSADDR(sr));
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

static void _subroutine_find_refs_all(ctxt_t *ctxt)
{
	int i;

	//Find references for all subroutines.
	for(i = 0; i < ctxt->subroutines.size(); i++)
		_subroutine_find_refs(ctxt, ctxt->subroutines[i]);
}

static subroutine_t *_subroutine_extract(execr_t *er, unsigned int sidx)
{
	unsigned int i;

	for(i = sidx; i < er->instrs.size(); i++)
	{
		instr_t *inst = &(er->instrs[i]);
		//Try to find a bi $lr instruction for now.
		//A subroutine could also have more than one bi $lr, check this later.
		if(IS_RETURN(inst))
		{
			subroutine_t *res = new subroutine_t;
			res->er = er;
			res->sidx = sidx;
			res->eidx = i;
			return res;
		}
		//Discard subroutines with unknown instructions.
		if(inst->instr == INSTR_NONE)
		{
			DBGPRINTF("subroutine: unknown instruction @ 0x%05x\n", IIDX2ADDR(er, i));
			return NULL;
		}
	}

	return NULL;
}

subroutine_t *subroutine_find_tsubref(subroutine_t *sr, u32 addr)
{
	int i;

	for(i = 0; i < sr->tsubrefs.size(); i++)
	{
		reference_t *ref = sr->tsubrefs[i];
		//Check if the address matches a subroutine reference.
		if(IIDX2ADDR(ref->subroutine->er, ref->refidx) == addr)
			return ref->subroutine;
	}

	return NULL;
}

void subroutine_extract_all(ctxt_t *ctxt)
{
	unsigned int i, j;

	//Check all executable ranges.
	for(i = 0; i < ctxt->execrs.size(); i++)
	{
		execr_t *er = ctxt->execrs[i];
		j = 0;
		//Check all instructions.
		while(j < er->instrs.size())
		{
			instr_t *inst = &(er->instrs[j]);
			//Ignore nops and unknown instructions.
			if(inst->instr != INSTR_NOP && 
				inst->instr != INSTR_LNOP &&
				inst->instr != INSTR_NONE)
			{
				//Extract next subroutine.
				subroutine_t *sr = _subroutine_extract(er, j);
				if(sr != NULL)
				{
					DBGPRINTF("subroutine: found sub @ 0x%05x (end @ 0x%05x)\n", SUBSADDR(sr), SUBEADDR(sr));
					ctxt->subroutines.push_back(sr);
					//Move instruction index to subroutine end.
					j = sr->eidx;
				}
			}
			j++;
		}
	}

	//Now find all references.
	_subroutine_find_refs_all(ctxt);

	//Check if every subroutine is reachable and mark them respective.
	for(i = 0; i < ctxt->subroutines.size(); i++)
	{
		subroutine_t *sr = ctxt->subroutines[i];
		sr->reachable = (sr->fsubrefs.size() == 0 && 
			SUBSADDR(sr) != ctxt->entry ? false : true);
		DBGPRINTF("subroutine: sub @ 0x%05x is %s\n", 
			SUBSADDR(sr), (sr->reachable == true ? "reachable" : "not reachable"));
	}
}
