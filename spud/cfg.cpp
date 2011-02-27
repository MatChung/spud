// Copyright 2011 naehrwert
// Licensed under the terms of the GNU GPL, version 2
// http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt

#include "common.h"
#include "spud.h"
#include "subroutine.h"
#include "block.h"

static edge_t *_cfg_add_edge(subroutine_t *sr, block_t *from, block_t *to)
{
	edge_t *edge = new edge_t;

	edge->from = from;
	edge->to = to;

#ifdef _DEBUG
	if(to->type == BLOCK_END)
		DBGPRINTF("cfg: adding edge from block @ 0x%05x to sub end\n", 
		BLOCKSADDR(from));
	else
		DBGPRINTF("cfg: adding edge from block @ 0x%05x to block @ 0x%05x\n", 
		BLOCKSADDR(from), BLOCKSADDR(to));
#endif

	sr->edges.push_back(edge);
	from->outedge.push_back(edge);
	to->inedge.push_back(edge);

	return edge;
}

void cfg_build(subroutine_t *sr)
{
	list<block_t *>::iterator it;

	for(it = sr->blocks.begin(); it != sr->blocks.end() && (*it)->type != BLOCK_END; ++it)
	{
		block_t *from = *it;

		//Check for end block.
		if(from->type == BLOCK_END)
			break;

		//Check for return (bi $lr).
		if(from->exitinst->instr == INSTR_BI && from->exitinst->rr.rt == REG_LR)
		{
			//Just link directly to end node.
			edge_t *edge = _cfg_add_edge(sr, from, sr->blocks.back());
			edge->type = EDGE_RETURN;
			continue;
		}

		//Check for direct branch.
		if(disasm_is_direct_branch(from->exitinst))
		{
			block_t *to = NULL;
			list<block_t *>::iterator iit;

			//Try to find target block.
			for(iit = sr->blocks.begin(); iit != sr->blocks.end(); ++iit)
			{
				block_t *tbl = *iit;
				if(BRANCH_TARGET(from->exitinst) == BLOCKSADDR(tbl))
				{
					to = tbl;
					break;
				}
			}

			if(to != NULL)
			{
				//Add edge if we found the target block.
				edge_t *edge = _cfg_add_edge(sr, from, to);
				if(disasm_is_direct_cond_branch(from->exitinst))
					edge->type = EDGE_BRANCH_COND;
				else
					edge->type = EDGE_BRANCH;
			}
			else
			{
				printf("cfg: could not find target for block @ 0x%05x\n", BLOCKSADDR(from));
				//Link directly to end node in this case.
				edge_t *edge = _cfg_add_edge(sr, from, sr->blocks.back());
				edge->type = EDGE_ERROR;
			}

			//If the branch is conditional we also have to make a branch to the next block.
			if(disasm_is_direct_cond_branch(from->exitinst))
			{
				++it;
				if(it != sr->blocks.end())
				{
					edge_t *edge = _cfg_add_edge(sr, from, *it);
					edge->type = EDGE_BRANCH;
				}
				--it;
			}

			continue;
		}
	}
}
