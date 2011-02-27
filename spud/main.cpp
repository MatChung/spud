// Copyright 2011 naehrwert
// Licensed under the terms of the GNU GPL, version 2
// http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt

#include <stdio.h>
#include <stdlib.h>

#include "spud.h"
#include "disasm.h"
#include "subroutine.h"
#include "output.h"

void print_usage(char *self)
{
	printf("%s Copyright 2011 by naehrwert\n");
	printf("usage: %s in_elf out_code\n");
	exit(1);
}

int main(int argc, char **argv)
{
	/*if(argc != 3)
		print_usage(argv[0]);

	ctxt_t *ctxt = spud_create_ctxt(argv[1]);
	disasm_disassemble(ctxt);
	subroutines_extract_all(ctxt);
	output_write(ctxt, argv[2]);
	getchar();
	spud_destroy_ctxt(ctxt);*/

	ctxt_t *ctxt = spud_create_ctxt("test3");
	
	disasm_disassemble(ctxt);

	//Extract all subroutines.
	subroutine_extract_all(ctxt);
	
	output_write(ctxt, "test1.txt");
	
	spud_destroy_ctxt(ctxt);

	getchar();

	return 0;
}
