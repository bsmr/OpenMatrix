/**
* @file ANTLRoverride.cpp
* @date March 2014
* Copyright (C) 2014-2018 Altair Engineering, Inc.  
* This file is part of the OpenMatrix Language (�OpenMatrix�) software.
* Open Source License Information:
* OpenMatrix is free software. You can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
* OpenMatrix is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for more details.
* You should have received a copy of the GNU Affero General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.
* 
* Commercial License Information: 
* For a copy of the commercial license terms and conditions, contact the Altair Legal Department at Legal@altair.com and in the subject line, use the following wording: Request for Commercial License Terms for OpenMatrix.
* Altair�s dual-license business model allows companies, individuals, and organizations to create proprietary derivative works of OpenMatrix and distribute them - whether embedded or bundled with other software - under a commercial license agreement.
* Use of Altair�s trademarks and logos is subject to Altair's trademark licensing policies.  To request a copy, email Legal@altair.com and in the subject line, enter: Request copy of trademark and logo usage policy.
*/

#include "ExprCppTreeLexer.h"
#include "ExprCppTreeParser.h"
#include "Evaluator.h"
#include "ErrorInfo.h"
#include "ANTLRoverride.h"

#include <map>

extern std::map<void *, ErrorInfo> errMap;

void displayRecognitionErrorNew  (pANTLR3_BASE_RECOGNIZER recognizer, pANTLR3_UINT8 * tokenNames)
{
	void *par = recognizer->super;
	int lin = recognizer->state->exception->line;

	int chPosit = recognizer->state->exception->charPositionInLine;
	if (chPosit < 1)
		chPosit = 1;

	std::string errFile = "dummy";

	if (recognizer->state->exception->streamName != NULL)
	{
		unsigned char *ch = recognizer->state->exception->streamName->chars;
		errFile =(char*)ch;
	}
	
	ErrorInfo err(errFile,lin, chPosit);
	errMap[par]= err;
}

// I tried this to speed up the ANTLR parsing.  It helped only a very little bit (2.5% or so), and I had to change the antlr3c code to do it.
// IMO, it's not worth it, but if we want to resurrect it, restore this code and add the following to the top of antlr3collections.c
// All of the regression tests passed, so it seems like it's functioning properly.
//#define ANTLR3_CALLOC OML_CALLOC
//#define ANTLR3_FREE OML_FREE
//extern void* OML_CALLOC(size_t num_el, size_t size);
//extern void OML_FREE(void* ptr);

#if 0
class AntlrMemoryPool
{
public:
	AntlrMemoryPool(int item_size, int items_per_block);
	void* Get();
	void  Free(void*);
	bool  Owns(void*);

private:
	void AddBlock();

	int item_size;
	int items_per_block;
	int used;
	int num_in_use; // remove this later
	std::vector<unsigned char*> blocks;
	std::vector<void*> freed_ptrs;
};

AntlrMemoryPool::AntlrMemoryPool(int isize, int bsize) : item_size(isize), items_per_block(bsize), num_in_use(0)
{
	AddBlock();
}

void AntlrMemoryPool::AddBlock()
{
	int            alloc_size = item_size*items_per_block;
	unsigned char* new_block  = new unsigned char [alloc_size];

	memset(new_block, 0, alloc_size);

	blocks.push_back(new_block);
	used = 0;
}

void* AntlrMemoryPool::Get()
{
	unsigned char* ret_ptr = NULL;

	if (freed_ptrs.size())
	{
		ret_ptr = (unsigned char*)freed_ptrs[freed_ptrs.size()-1];
		freed_ptrs.pop_back();
	}
	else
	{
		if (used == items_per_block)
			AddBlock();

		unsigned char* last_block = blocks[blocks.size()-1];

		ret_ptr = last_block + used*item_size;
		used++;		
		num_in_use++;
	}

	return ret_ptr;
}

bool AntlrMemoryPool::Owns(void* ptr)
{
	for (int j=0; j<blocks.size(); j++)
	{
		unsigned char* block = blocks[j];

		unsigned char* block_start = block;
		unsigned char* block_end   = block+item_size*items_per_block;

		if ((ptr >= block) && (ptr < block_end))
			return true;
	}
	
	return false;
}

void AntlrMemoryPool::Free(void* ptr)
{
	memset(ptr, 0, item_size);
	freed_ptrs.push_back(ptr);
}

static AntlrMemoryPool pool56(56, 256);
static AntlrMemoryPool pool40(40, 32768);
static AntlrMemoryPool pool32(32, 32768);

extern "C"
{
	void* OML_CALLOC(size_t num_el, size_t size)
	{
		if (size == 56)
			return pool56.Get();
		else if (size == 40)
			return pool40.Get();
		else if (size == 32)
			return pool32.Get();
		else
			return calloc(num_el, size);
	}

	void OML_FREE(void* ptr)
	{
		if (pool56.Owns(ptr))
			pool56.Free(ptr);
		else if (pool40.Owns(ptr))
			pool40.Free(ptr);
		else if (pool32.Owns(ptr))
			pool32.Free(ptr);
		else
			free(ptr);
	}
}
#endif