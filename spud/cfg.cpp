// Copyright 2011 naehrwert
// Licensed under the terms of the GNU GPL, version 2
// http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt

#include "common.h"
#include "spud.h"
#include "subroutine.h"
#include "block.h"

static edge_t *_cfg_add_edge(subroutine_t *sr, block_t *from, block_t *to, int type)
{
	edge_t *edge = new edge_t;

	edge->from = from;
	edge->to = to;
	edge->type = type;

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

		//Check for return (bi $lr) and link directly to the end node in this case.
		if(IS_RETURN(from->exitinst))
			_cfg_add_edge(sr, from, sr->blocks.back(), EDGE_EXIT);
		else if(disasm_is_direct_branch(from->exitinst) && from->type != BLOCK_CALL) //Check for direct branch.
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
				edge_t *edge = _cfg_add_edge(sr, from, to, EDGE_ERROR);
				if(disasm_is_direct_cond_branch(from->exitinst))
				{
					edge->type = EDGE_BRANCH_COND;
					//If the branch is conditional we also have to make a branch to the next block.
					++it;
					if(it != sr->blocks.end())
						_cfg_add_edge(sr, from, *it, EDGE_BRANCH_COND);
					--it;
				}
				else
					edge->type = EDGE_BRANCH;
			}
			else
			{
				printf("cfg: could not find target for block @ 0x%05x\n", BLOCKSADDR(from));
				//Link directly to end node in this case.
				edge_t *edge = _cfg_add_edge(sr, from, sr->blocks.back(), EDGE_ERROR);
			}
		}
		else
		{
			//Just make a link to the next block.
			++it;
			if(it != sr->blocks.end())
				_cfg_add_edge(sr, from, *it, EDGE_NEXT);
			--it;
		}
	}
}

void cfg_print(block_t *bl, int d, int max)
{
	int i;

	if(d > max)
		return;

	for(i = 0; i < d; i++)
		printf("  ");
	if(bl->type == BLOCK_END)
	{
		printf("[end]\n");
		return;
	}
	else
		printf("[0x%05x]\n", BLOCKSADDR(bl));
	for(i = 0; i < bl->outedge.size(); i++)
		cfg_print(bl->outedge[i]->to, d+1, max);
}

void cfg_build_all(ctxt_t *ctxt)
{
	unsigned int i;
	for(i = 0; i < ctxt->subroutines.size(); i++)
	{
		cfg_build(ctxt->subroutines[i]);
		//list<block_t *>::iterator it;
		//for(it = ctxt->subroutines[i]->blocks.begin(); it != ctxt->subroutines[i]->blocks.end(); ++it)
		//	cfg_print(*it, 0, ctxt->subroutines[i]->blocks.size());
	}
}
