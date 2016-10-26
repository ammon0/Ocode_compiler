/******************************************************************************/
//	Author:	Ammon Dodson
//	Spring 2016
//
//	This program should be build with make. A makefile has been provided.
//
/******************************************************************************/


#include "compiler.h"


int main (int argc, const char** argv){
	sym_entry* sym_pt;
	
	/*************************** INITIALIZATIONS ******************************/
	
	// The output file
	if (argc > 1) outfile=fopen(argv[1], "w");
	else outfile=stdout;
	
	// The build architecture and mode
	arch=x86;
	x86_mode=Long;
	
	// Initialize the lookahead token
	get_token();
	
	// Initialize the symbol table
	global_symbols=new_DS('l');
	
	// Block_lvl is initialized in scanner.l
	
	// boilerplate in the output
/*	fprintf(outfile,"; a NASM object file created by the Omega Compiler\n");*/
/*	fprintf(*/
/*		outfile,*/
/*		"BITS 64 ; tell nasm that we are building for 64-bit\n"*/
/*	);*/
/*	fprintf(outfile,"global\t_start\n");*/
	
	
	fprintf(outfile,"#Omnicode Intermidiate File\n");
	
	
	/***************************** COMPILATION ********************************/
	
	
	// executable code
/*	fprintf(outfile,"\nsection .text\t; Program code\n");*/
/*	fprintf(outfile,"_start:\n");*/
	
	do {
		Statement(0);
	} while (token != T_EOF);
	
	
	/**************************** DATA SECTIONS *******************************/
	
	
	//fprintf(outfile,"\nsection .data\t; Data Section contains constants\n");
/*	fprintf(outfile,"\nsection .bss\t; Declare static variables\n");*/
/*	fprintf(outfile,"align 8\n");*/
	
	// Dump the symbol Table
	
	fprintf(outfile,"\n#Symbol Table\n");
	
	pview(global_symbols, 0);
	while((sym_pt=view_next(global_symbols))){
		if(!( sym_pt->constant == true ))
			fprintf(outfile, "%p: %s\n", (void*)sym_pt, sym_pt->name);
	}
	
	
	/******************************* Cleanup **********************************/
	
	
	fclose(outfile);
	return EXIT_SUCCESS;
}
