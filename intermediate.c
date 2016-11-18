/*
 *	OMNI INTERMEDIATE CODE GENERATION
 *
 *	Omni Intermediate Code is an implementation of three-address-code. It
 *	consists of one command per line or structure. Normally, intermediate code
 *	is generated in a queue of structures. However, when the compiler's debug
 *	flag is set, the intermediate code emitters will also create text files
 *	reprsenting the contents of the interm_q.
 *
 *	The general format of Three Address Codes is:
 *		OP		result	arg1	arg2
 *		OP		result	arg1
 *	The code must also contain jump statements and labels for flow control:
 *		LBL		label
 *		JMP		label	[condition]
 *	There must be function calls
 *		PARAM	reg		val
 *		CALL	label
 *		RTRN
 *
 *	Every control structure is reimplemented in the Intermediate Code with
 *	labels and jump statements. This requires extra labels provided by the
 *	new_label() function.
 *
 *	Expressions are reimplemented as individual operations. These operations
 *	will eventually act directly on register values. In the Intermediate Code
 *	registers are represented as temp symbols. Temp symbol names all begin with
 *	the % symbol.
 *
 *	In the future we will be able to implement an optimizer that processes
 *	Intermediate code into better Intermediate Code.
 *
 */

#include "compiler.h"

/******************************************************************************/
//                             LOCAL PROTOTYPES
/******************************************************************************/


//icmd * New_iop(void);
/*int cmp_sym    (const void * left, const void * right);*/
/*int cmp_sym_key(const void * key , const void * symbol);*/


/******************************************************************************/
//                            PRIVATE FUNCTIONS
/******************************************************************************/


static int cmp_sym(const void * left, const void * right){
	return strcmp(
		dx_to_name(((sym_pt) left)->name),
		dx_to_name(((sym_pt)right)->name)
	);
}

static int cmp_sym_key(const void * key, const void * symbol){
	return strcmp((char*) key, dx_to_name(((sym_pt)symbol)->name));
}


/******************************************************************************/
//                            PUBLIC FUNCTIONS
/******************************************************************************/


void Initialize_intermediate(void){
	if(debug_fd) fprintf(debug_fd,"#Omnicode Intermidiate File\n");
	
	// initialize the symbol table
	global_symbols = DS_new(
		DS_bst,
		sizeof(sym_entry),
		false,
		&cmp_sym,
		&cmp_sym_key
	);
	
	// initialize the intermediate code queue
	interm_q = DS_new(
		DS_list,
		sizeof(icmd),
		false,
		NULL,
		NULL
	);
	
	nxt_lbl       = 0; // make sure the nxt_lbl is empty
}

// Dump the symbol Table
void Dump_symbols(void){
	sym_pt sym;
	
	fputs("# SYMBOL TABLE", debug_fd);
	fprintf(debug_fd,"\n#Name\tType\tconst\tInit\tDref\n");
	
	sym = DS_first(global_symbols);
	do {
		if( sym->type != literal )
			fprintf(debug_fd, "%s:\t%4d\t%5d\t%4d\t%p\n",
				dx_to_name(sym->name),
				sym->type,
				sym->constant,
				sym->init,
				(void*) sym->dref
			);
	} while(( sym = DS_next(global_symbols) ));
}

/********************************** NAMES *************************************/

/// insert a new name into the name_array. returns its name_dx
name_dx add_name(char * name){
	static name_dx size;
	static name_dx next;
	       name_dx name_sz;
	       name_dx temporary;
	
	/* we have to keep track of things with an index to ensure that the data
	will continue to be accessable when the array is resized.
	*/
	
	// Initialize the name array
	if(!size){ // This should only run the first time
		name_array=malloc(sizeof(char) * NAME_ARR_SZ);
		if(!name_array) crit_error("Out of Memory");
		size = NAME_ARR_SZ;
	}
	
	name_sz = strlen(name) + 1; // +1 for the null
	
	// resize the array if necessary
	if(size - next < name_sz){
		name_array = realloc(name_array, (size *= 2));
		if(!name_array) crit_error("Out of Memory");
	}
	
	strncpy(dx_to_name(next), name, name_sz);
	temporary = next;
	next += name_sz;
	return temporary;
}

/// find a name in the name_array by its name_dx
inline char * dx_to_name(name_dx index){
	return name_array+index;
}

// create and return a pointer to a unique label
name_dx new_label(void){
	static umax i;
	static char label[UNQ_NAME_SZ];
	// sufficiently large for 32-bit numbers in decimal and then some.
	
	
	sprintf(label, "_%04lld$", i++); // use $ to prevent collisions
	return add_name(label);
}

/**	create a new symbol table entry with a unique name.
 *	*	temporary symbol names all begin with %.
 */
sym_entry* new_var(void){
	static umax i;
	char name[UNQ_NAME_SZ];
	static sym_entry new_symbol; // initialized to 0
	
	// give it a unique name
	sprintf(name, "%%%04lld", i++);
	new_symbol.name = add_name(name);
	new_symbol.type = temp;
	
	// insert it into the symbol table
	DS_sort(global_symbols, &new_symbol);
	
	// and return it
	return DS_current(global_symbols);
}


/******************************** EMITTERS ************************************/



void emit_cmnt(const char* comment){
	if (debug_fd) fprintf(debug_fd, "\t# %s\n", comment);
}

void emit_lbl(name_dx lbl){
	if (nxt_lbl) emit_iop(I_NOP, 0, NULL, NULL, NULL);
	// If there is already a label in the q emit it with a nop
	
	nxt_lbl = lbl;
	
	if (debug_fd) fprintf(debug_fd, "\nlbl %s:", dx_to_name(lbl));
}

void emit_iop(
	byte_code        op,
	name_dx          target,
	const sym_entry* out,
	const sym_entry* left,
	const sym_entry* right
){
	char arg1[20], arg2[20];
	char err_array[ERR_ARR_SZ];
	icmd intermediate_cmd;
	icmd * iop = &intermediate_cmd;
	
	if (nxt_lbl) {
		iop->label = nxt_lbl;
		nxt_lbl = 0;
	}
	
	iop->op = op;
	
	switch (op){
	case I_NOP : break;
	
	// Binaries
	case I_MUL :
	case I_DIV :
	case I_MOD :
	case I_EXP :
	case I_LSH :
	case I_RSH :
	case I_ADD :
	case I_SUB :
	case I_BAND:
	case I_BOR :
	case I_XOR :
	case I_EQ  :
	case I_NEQ :
	case I_LT  :
	case I_GT  :
	case I_LTE :
	case I_GTE :
	case I_AND :
	case I_OR  : // stuff for binaries only
		if (right->type == literal){
			iop->arg2_lit   = true;
			iop->arg2.value = right->value;
			sprintf(arg2, "#%4llx", right->value);
		}
		else {
			iop->arg2.symbol = right;
			sprintf(arg2, "%s", dx_to_name(right->name));
		}
		
	// Unaries
	case I_ASS :
	case I_REF :
	case I_DREF:
	case I_NEG :
	case I_NOT :
	case I_INV : // stuff for unaries and binaries
		if (left->type == literal){
			iop->arg1_lit   = true;
			iop->arg1.value = left->value;
			sprintf(arg1, "#%4llx", left->value);
		}
		else {
			iop->arg1.symbol = left;
			sprintf(arg1, "%s", dx_to_name(left->name));
		}
		
		iop->result = out;
		break;
		
	case I_JMP :
	case I_JZ  :
		if (left->type == literal){
			iop->arg1_lit   = true;
			iop->arg1.value = left->value;
			sprintf(arg1, "#%4llx", left->value);
		}
		else {
			iop->arg1.symbol = left;
			sprintf(arg1, "%s", dx_to_name(left->name));
		}
		iop->target = target;
		break;
		
	case I_BLK :
	case I_EBLK:
	case I_CALL:
	case I_RTRN:
	default:
		sprintf(err_array,
			"Internal Compiler Error: emit_triple() called with cmd = %d",
			op
		);
		crit_error(err_array);
	}
	
	// queue up this operation
	DS_nq(interm_q, iop);
	
	// Print to the text file if present
	if (debug_fd)
		fprintf(
			debug_fd,
			"%s\t%5s\t%5s\t%s\n",
			byte_code_dex[op],
			dx_to_name(out->name),
			arg1,
			right? arg2 : ""
		);
}

