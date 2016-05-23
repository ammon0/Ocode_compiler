

#include "compiler.h"


/******************************************************************************/
//                           PRIVATE PROTOTYPES
/******************************************************************************/


void Label (void    );
void Jump  (void    );
void If    (uint lvl); // only control statements need to know the block_lvl
void While (uint lvl);

void Declaration  (void);
void Storage_class(sym_entry* new_symbol);


/******************************************************************************/
//                           CONTROL STATEMENTS
/******************************************************************************/


void If(uint lvl){
	char if_label[UNQ_LABEL_SZ];
	char else_label[UNQ_LABEL_SZ];
	
	Match(T_IF);
	emit_cmnt("start of IF statement");
	strcpy(if_label, new_label());
	
	Result();
	fprintf(outfile, "\tjz %s\n", if_label);
	
	Statement(lvl);
	
	if(token == T_ELSE){
		Match(T_ELSE);
		emit_cmnt("start of ELSE statement");
		strcpy(else_label, new_label());
		fprintf(outfile, "\tjmp %s\n", else_label);
		emit_lbl(if_label);
		
		Statement(lvl);
		
		fprintf(outfile, "%s: ; End of ELSE\n", else_label);
	}
	else
		fprintf(outfile, "%s:\n", if_label);
	emit_cmnt("End of IF Statement\n");
}

void While(uint lvl){
	char repeat_label[UNQ_LABEL_SZ], skip_label[UNQ_LABEL_SZ];
	
	Match(T_WHILE);
	emit_cmnt("Start of WHILE loop");
	strcpy(repeat_label, new_label());
	strcpy(skip_label  , new_label());
	
	fprintf(outfile, "%s: ; repeat label\n", repeat_label);
	Result();
	fprintf(outfile, "\tjz %s ; skip jump\n", skip_label);
	
	Statement(lvl);
	
	fprintf(outfile, "\tjmp %s ; repeat\n", repeat_label);
	fprintf(outfile, "%s: ; skip label\n" , skip_label);
	emit_cmnt("End of WHILE loop\n");
}

void Label(void){
	Match(T_LBL);
	emit_lbl(get_name());
	Match(T_NL);
}

void Jump(void){
	Match(T_JMP);
	fprintf(outfile, "\tjmp %s\n", get_name());
	Match(T_NL);
}


/******************************************************************************/
//                              DECLARATIONS
/******************************************************************************/


void Declaration(void){
	sym_entry* new_symbol=calloc(1, sizeof(sym_entry));
	
	if (new_symbol == NULL) error("Out of memory");
	
	switch (token){
		case T_8:
			new_symbol->size=byte;
			break;
		case T_16:
			new_symbol->size=word;
			break;
		case T_32:
			new_symbol->size=dword;
			break;
		case T_64:
			new_symbol->size=qword;
			break;
		default: error(_e_noimp);
	}
	get_token();
	Storage_class(new_symbol);
	
	do{
		strncpy(new_symbol->name, get_name(), NAME_MAX);
		if(token == T)
		Assignment(new_symbol);
		sort(symbol_table, new_symbol, new_symbol->name);
	} while (token != T_NL);
	
}

void Storage_class(sym_entry* new_symbol){
	if (token == T_STATIC)
		new_symbol->type |= S_STATIC;
	else if (token == T_CONST)
		new_symbol->type |= S_CONST;
	get_token();
}


/******************************************************************************/
//                             PUBLIC FUNCTIONS
/******************************************************************************/


void Statement (uint lvl){ // any single line. always ends with NL
	sym_entry* type_sym;
	
	if (token == T_NL){
		Match(T_NL);
		if(block_lvl <= lvl); // empty statement
		
		else { // subordinate block
			lvl=block_lvl;
			fprintf(outfile, "\t; START block level %u\n", lvl);
			do {
				Statement(lvl);
			} while (token != T_EOF && block_lvl == lvl);
			if(block_lvl > lvl) error("Unexpected nested block");
			fprintf(outfile, "\t; END block level %u\n", lvl);
		}
	}
	else switch (token){
		case T_LBL:   Label      (   ); break;
		case T_JMP:   Jump       (   ); break;
		case T_IF:    If         (lvl); break;
		case T_WHILE: While      (lvl); break;
		case T_8:     Declaration(   ); break;
		case T_16:    Declaration(   ); break;
		case T_32:    Declaration(   ); break;
		case T_64:    Declaration(   ); break;
		case T_NAME:
			if ((type_sym=iview(symbol_table, yytext))){
				if(type_sym->flags&S_TYPDEF)
					Declaration(); break;
				
			}
			else error("undefined token");
		default: Assignment_statement();
	}
}


