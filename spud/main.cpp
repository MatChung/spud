// Copyright 2011 naehrwert
// Licensed under the terms of the GNU GPL, version 2
// http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt

#include <stdio.h>
#include <stdlib.h>

#include "spud.h"

void print_usage(char *self)
{
	printf("%s Copyright 2011 by naehrwert\n");
	printf("usage: %s in_elf out_code\n");
	exit(1);
}

int main(int argc, char **argv)
{
	/*
	if(argc != 3)
		print_usage(argv[0]);

	ctxt_t *ctxt = spud_create_ctxt(argv[1]);
	spud_decompile(ctxt, argv[2]);
	spud_destroy_ctxt(ctxt);
	*/

	//ctxt_t *ctxt = spud_create_ctxt("../../isomodules/aim_spu_module.elf");
	ctxt_t *ctxt = spud_create_ctxt("test3");
	spud_decompile(ctxt, "test1.txt");
	spud_destroy_ctxt(ctxt);

	getchar();

	return 0;
}
