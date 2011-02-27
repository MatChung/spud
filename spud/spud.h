// Copyright 2011 naehrwert
// Licensed under the terms of the GNU GPL, version 2
// http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt

#ifndef _SPUD_H_
#define _SPUD_H_

#include <vector>
using std::vector;

#include "types.h"
#include "disasm.h"

typedef struct _execr_t
{
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

typedef struct _block_t
{
	//Owning subroutine.
	struct _subroutine_t *sr;
	//Start instruction index.
	unsigned int sidx;
	//End instruction index.
	unsigned int eidx;
} block_t;

typedef struct _subroutine_t
{
	//Owning executable range.
	execr_t *execr;
	//Start instruction index;
	unsigned int sidx;
	//End instruction index.
	unsigned int eidx;
	//Reachable?
	bool reachable;
	//References from other subroutines.
	vector<reference_t *>fsubrefs;
	//Reference to other subroutines.
	vector<reference_t *>tsubrefs;
	//Blocks.
	vector<block_t *>blocks;
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

#endif
