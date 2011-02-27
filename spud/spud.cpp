// Copyright 2011 naehrwert
// Copyright 2010 fail0verflow <master@fail0verflow.com>
// Licensed under the terms of the GNU GPL, version 2
// http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt

//ELF loader from fail0verflow's anergistic.

#include <stdio.h>
#include <string.h>
#include <malloc.h>

#include "config.h"
#include "types.h"
#include "common.h"
#include "spud.h"

static void _elf_load_phdr(ctxt_t *ctxt, FILE *fp, u32 phdr_offset, u32 i)
{
	u8 phdr[0x20];
	u32 offset, paddr, size, flags;

	fseek(fp, phdr_offset + 0x20 * i, SEEK_SET);
	fread(phdr, sizeof phdr, 1, fp);

	if(be32(phdr) != 1)
	{
		DBGPRINTF("elf: phdr #%u: no LOAD\n", i);
		return;
	}

	offset = be32(phdr + 0x04);
	paddr = be32(phdr + 0x0c);
	size = be32(phdr + 0x10);
	flags = be32(phdr + 0x18);
	DBGPRINTF("elf: phdr #%u: 0x%08x bytes (0x%08x -> 0x%08x)\n", i, size, offset, paddr);

	if(flags & 0x1)
	{
		execr_t *er = new execr_t;
		er->start = paddr;
		er->size = size;
		ctxt->execrs.push_back(er);
		DBGPRINTF("spud: added executable range @ 0x%08x (0x%08x bytes)\n", er->start, er->size);
	}

	//XXX: integer overflow
	if(offset > LS_SIZE || (offset + size) > LS_SIZE)
		fail("elf: phdr exceeds local storage");

	fseek(fp, offset, SEEK_SET);
	fread(ctxt->ls + paddr, size, 1, fp);
}

static void _elf_load(ctxt_t *ctxt, const char *path)
{
	static const char elf_magic[] = {0x7f, 'E', 'L', 'F'};
	u8 ehdr[0x34];
	u32 i, phdr_offset, n_phdrs;

	FILE *fp = fopen(path, "rb");
	if(fp == NULL)
		fail("elf: unable to load elf");

	fread(ehdr, sizeof ehdr, 1, fp);
	if(memcmp(ehdr, elf_magic, 4))
		fail("elf: not a ELF file");

	phdr_offset = be32(ehdr + 0x1c);
	n_phdrs = be16(ehdr + 0x2c);

	DBGPRINTF("elf: %u phdrs at offset 0x%08x\n", n_phdrs, phdr_offset);

	for(i = 0; i < n_phdrs; i++)
		_elf_load_phdr(ctxt, fp, phdr_offset, i);

	ctxt->entry = be32(ehdr + 0x18);
	DBGPRINTF("elf: entry is at 0x%08x\n", ctxt->entry);

	fclose(fp);
}

ctxt_t *spud_create_ctxt(const char *elf)
{
	ctxt_t *res = new ctxt_t;

	if((res->ls = (u8 *)malloc(LS_SIZE)) == NULL)
	{
		delete res;
		fail("unable to allocate local storage");
	}

	memset(res->ls, 0, LS_SIZE);

	_elf_load(res, elf);

	return res;
}

void spud_destroy_ctxt(ctxt_t *ctxt)
{
	unsigned int i, j;

	if(ctxt->ls != NULL)
		free(ctxt->ls);

	for(i = 0; i < ctxt->execrs.size(); i++)
		delete ctxt->execrs[i];

	for(i = 0; i < ctxt->subroutines.size(); i++)
	{
		subroutine_t *sr = ctxt->subroutines[i];

		for(j = 0; j < sr->tsubrefs.size(); j++)
			delete sr->tsubrefs[j];
		
		for(j = 0; j < sr->fsubrefs.size(); j++)
			delete sr->fsubrefs[j];

		delete sr;
	}

	delete ctxt;
}
