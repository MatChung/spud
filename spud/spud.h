// Copyright 2011 naehrwert
// Licensed under the terms of the GNU GPL, version 2
// http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt

#ifndef _SPUD_H_
#define _SPUD_H_

#include <vector>
using std::vector;
#include <list>
using std::list;

#include "types.h"
#include "disasm.h"

typedef struct _execr_t
{
	//Owning context.
	struct _ctxt_t *ctxt;
	//Start address.
	u32 start;
	//Size.
	u32 size;
	//Disassembled instructions.
	vector<instr_t> instrs;
} execr_t;

typedef struct _reference_t
{
	//Referencing subroutine.
	struct _subroutine_t *subroutine;
	//Referencing instruction index.
	unsigned int refidx;
} reference_t;

//Directly go to the next node.
#define EDGE_BRANCH 1
//Go to the next node under some condition.
#define EDGE_BRANCH_COND 2
//Exit.
#define EDGE_EXIT 3
//Next.
#define EDGE_NEXT 4
//Error.
#define EDGE_ERROR 5

typedef struct _edge_t
{
	//Edge type.
	int type;
	//From node.
	struct _block_t *from;
	//To node.
	struct _block_t *to;
} edge_t;

//Simple block.
#define BLOCK_SIMPLE 0
//Call block.
#define BLOCK_CALL 1
//End block.
#define BLOCK_END 2

typedef struct _block_t
{
	//Owning subroutine.
	struct _subroutine_t *sr;
	//Start instruction index.
	unsigned int sidx;
	//End instruction index.
	unsigned int eidx;
	//Exit instruction.
	instr_t *exitinst;

	//Type.
	int type;

	//In edges.
	vector<edge_t *>inedge;
	//Out edges.
	vector<edge_t *>outedge;
} block_t;

typedef struct _operation_t
{
	//...
} operation_t;

typedef struct _subroutine_t
{
	//Owning executable range.
	execr_t *er;
	//Start instruction index;
	unsigned int sidx;
	//End instruction index.
	unsigned int eidx;
	//Reachable?
	bool reachable;
	//References from other subroutines.
	vector<reference_t *> fsubrefs;
	//Reference to other subroutines.
	vector<reference_t *> tsubrefs;
	//Blocks.
	list<block_t *> blocks;
	//Edges.
	vector<edge_t *> edges;
} subroutine_t;

typedef struct _ctxt_t
{
	//Local storage.
	u8 *ls;
	//Entry.
	u32 entry;
	//Executable ranges.
	vector<execr_t *> execrs;
	//Subroutines.
	vector<subroutine_t *>subroutines;
} ctxt_t;

ctxt_t *spud_create_ctxt(const char *elf);
void spud_destroy_ctxt(ctxt_t *ctxt);
void spud_decompile(ctxt_t *ctxt, const char *out);

#endif
