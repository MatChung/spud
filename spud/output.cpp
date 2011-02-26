// Copyright 2011 naehrwert
// Licensed under the terms of the GNU GPL, version 2
// http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt

#include <stdio.h>

#include "common.h"
#include "spud.h"
#include "disasm.h"
#include "subroutine.h"

void output_write_subroutine(FILE *fp, subroutine_t *sr)
{
	int i;

	//Write header.
	fprintf(fp, "sub_%05x()\n{\n", IIDX_TO_ADDR(sr->execr, sr->sidx));
	//Write instructions for now.
	for(i = sr->sidx; i <= sr->eidx; i++)
	{
		instr_t *inst = &(sr->execr->instrs[i]);

		//Check for direct function call.
		if((inst->instr == INSTR_BR || 
			inst->instr == INSTR_BRA || 
			inst->instr == INSTR_BRSL))
		{
			subroutine_t *tsr = subroutine_find_tsubref(sr, BRANCH_TARGET(inst));
			if(tsr != NULL)
				fprintf(fp, "\tsub_%05x();\n", BRANCH_TARGET(inst));
		}
		else if(inst->instr == INSTR_ORI)
		{
			//Nah we don't do it this way, this is just for testing.
			//The idea is to write an optimizer that collects and optimizes 
			//expressions and generates c statements out of them.
			fprintf(fp, "\t$%d = $%d | $%d;\n", inst->rr.rt, inst->rr.ra, inst->rr.rb);
		}
		else if(inst->instr == INSTR_IL)
		{
			//The same as above here.
			fprintf(fp, "\t$%d = %d;\n", inst->ri16.rt, inst->ri16.i16);
		}
		else
		{
			//Fallback: output asm statement.
			fprintf(fp, "\tasm(\"");
			disasm_print_instr(fp, inst, false);
			fprintf(fp, "\");\n");
		}

	}
	//Write footer.
	fprintf(fp, "}\n");
}

void output_write(ctxt_t *ctxt, const char *file)
{
	int i;
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
