// Copyright 2011 naehrwert
// Licensed under the terms of the GNU GPL, version 2
// http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt

#include <stdio.h>

#include "config.h"
#include "common.h"
#include "spud.h"
#include "disasm.h"
#include "subroutine.h"

void output_write_block(FILE *fp, block_t *bl)
{
	unsigned int i;

	//Write header.
	fprintf(fp, "\t//block @ %05x\n\t{\n", IIDX2ADDR(bl->sr->er, bl->sidx));
	//Write instructions for now.
	for(i = bl->sidx; i <= bl->eidx; i++)
	{
		instr_t *inst = &(bl->sr->er->instrs[i]);

		//Check for direct function call.
		if((inst->instr == INSTR_BR || 
			inst->instr == INSTR_BRA || 
			inst->instr == INSTR_BRSL) && 
			subroutine_find_tsubref(bl->sr, BRANCH_TARGET(inst)) != NULL)
			fprintf(fp, "\t\tsub_%05x();\n", BRANCH_TARGET(inst));
		else
		{
			//Fallback: output asm statement.
			fprintf(fp, "\t\tasm(\"");
			disasm_print_instr(fp, inst, false);
			fprintf(fp, "\");\n");
		}

	}
	//Write footer.
	//Write footer.
	fprintf(fp, "\t}\n");
}

void output_write_subroutine(FILE *fp, subroutine_t *sr)
{
	unsigned int i;

	//Write header.
	if(sr->reachable == false)
		fprintf(fp, "//Seems to be not reachable.\n");
	if(SUBSADDR(sr) == sr->er->ctxt->entry)
		fprintf(fp, "void _start()\n{\n");
	else
		fprintf(fp, "sub_%05x()\n{\n", IIDX2ADDR(sr->er, sr->sidx));
	//Write blocks.
	for(i = 0; i < sr->bblocks.size(); i++)
		output_write_block(fp, sr->bblocks[i]);
	//Write footer.
	fprintf(fp, "}\n");
}

void output_write(ctxt_t *ctxt, const char *file)
{
	unsigned int i;
	FILE *fp = fopen(file, "w");

	if((fp = fopen(file, "w")) == NULL)
		fail("output: could not open file %s", file);

	fprintf(fp, "//This file was generated with spud (c) 2011 by naehrwert\n\n");

	//Write out all subroutines.
	for(i = 0; i < ctxt->subroutines.size(); i++)
	{
		output_write_subroutine(fp, ctxt->subroutines[i]);
		fprintf(fp, "\n");
	}

	fclose(fp);
}
