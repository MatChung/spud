// Copyright 2011 naehrwert
// Licensed under the terms of the GNU GPL, version 2
// http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt

#include "common.h"
#include "disasm.h"
#include "spud.h"
#include "subroutine.h"

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
			instr_t *inst = &(tsr->execr->instrs[j]);
			//Check for a relative branch to start of subroutine.
			if(disasm_is_direct_branch(inst) && BRANCH_TARGET(inst) == IIDX2ADDR(sr->execr, sr->sidx))
			{
				DBGPRINTF("subroutine: found ref from sub @ 0x%05x (instr @ 0x%05x) to 0x%05x\n", 
					SUBSADDR(tsr), IIDX2ADDR(tsr->execr, j), SUBSADDR(sr));
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

static subroutine_t *_subroutine_extract(execr_t *er, int sidx)
{
	unsigned int j;

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

subroutine_t *subroutine_find_tsubref(subroutine_t *sr, u32 addr)
{
	int i;

	for(i = 0; i < sr->tsubrefs.size(); i++)
	{
		reference_t *ref = sr->tsubrefs[i];
		//Check if the address matches a subroutine reference.
		if(IIDX2ADDR(ref->subroutine->execr, ref->refidx) == addr)
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
			//Ignore nops.
			if(er->instrs[j].instr != INSTR_NOP && er->instrs[j].instr != INSTR_LNOP)
			{
				//Extract next subroutine.
				subroutine_t *sr = _subroutine_extract(er, j);
				if(sr != NULL)
				{
					DBGPRINTF("subroutine: found sub @ 0x%05x (end @ 0x%05x)\n", SUBSADDR(sr), SUBEADDR(sr));
					//Move instruction index to subroutine end.
					j = sr->eidx;
					ctxt->subroutines.push_back(sr);
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
