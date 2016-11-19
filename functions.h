/*******************************************************************************
 *
 *	occ : The Omega Code Compiler
 *
 *	Copyright (c) 2016 Ammon Dodson
 *
 ******************************************************************************/

#ifndef _FUNCTIIONS_H
#define _FUNCTIIONS_H

/******************************************************************************/
//                             GLOBAL PROTOTYPES
/******************************************************************************/


// From scanner.l
token_t yylex(void);
char * get_name(void);

// From parse_declarations.c
void Decl_Operator(void);
void Decl_Symbol  (void);

// From parse_expressions.c
const sym_entry * Boolean(void);

// from parse_statements.c
void Statement   (uint lvl);

// From intermediate.c
void   Initialize_intermediate(void);
void   Dump_symbols(void);

// Names
char * dx_to_name(name_dx index);
name_dx add_name(char * name);
name_dx     new_label(void); ///< get a new unique label
sym_entry * new_var  (void);

// Emmiters
void emit_cmnt(const char* comment);
void emit_lbl (name_dx lbl);
void emit_iop(
	byte_code        op,
	name_dx          target,
	const sym_entry* out,
	const sym_entry* left,
	const sym_entry* right
);


/******************************************************************************/
//                             INLINE FUNCTIONS
/******************************************************************************/

/**************************** VERBOSITY PRINTERS ******************************/

static inline void print_yuck_results(yuck_t * a){
	printf("\
ARGUMENTS PASSED\n\
nargs             :\t%lu\n\
args              :\t%s\n\
dashv_flag        :\t%u\n\
debug_arg         :\t%s\n\
dashD_arg         :\t%s\n\
outfile_arg       :\t%s\n\
dashp_flag        :\t%u\n\
dasha_flag        :\t%u\n\
dasho_flag        :\t%u\n\
dashe_flag        :\t%u\n\
x86_long_flag     :\t%u\n\
x86_protected_flag:\t%u\n\
arm_v7_flag       :\t%u\n\
arm_v8_flag       :\t%u\n\n" ,
			a->nargs      ,
			*a->args      ,
			a->dashv_flag ,
			a->debug_arg  ,
			a->dashD_arg  ,
			a->outfile_arg,
			a->dashp_flag ,
			a->dasha_flag ,
			a->dasho_flag ,
			a->dashe_flag ,
			a->x86_long_flag     ,
			a->x86_protected_flag,
			a->arm_v7_flag       ,
			a->arm_v8_flag
		);
}

/************************ ERROR REPORTING & RECOVERY **************************/

static inline void parse_error(const char* message){
	printf("CODE ERROR: %s, on line %d.\n", message, yylineno);
	while( (token = yylex()) != T_NL );
	printf("continuing...\n");
	longjmp(anewline, 1);
}

static inline void expected(const char* thing){
	char temp_array[ERR_ARR_SZ];
	sprintf(temp_array, "Expected %s, found %s", thing, yytext);
	parse_error(temp_array);
}

static inline void crit_error(const char* message){
	printf("CRITICAL ERROR: %s, on line %d.\n", message, yylineno);
	exit(EXIT_FAILURE);
}

/********************************* GETTERS ************************************/

static inline void get_token(void){
	token=yylex();
}

static inline umax get_num(void){
	umax num;
	
	if(token != T_NUM) expected("a number");
	num=yynumber;
	get_token();
	return num;
}

static inline bool Match_name(name_dx dx){
	if ( token != T_NAME || strcmp(name_array+dx, yytext) ){
		expected(name_array+dx);
	}
	get_token();
	return true;
}

static inline void Match(token_t t){
	if(token == t) get_token();
	else{
		char temp_array[ERR_ARR_SZ];
		sprintf(temp_array, "0x%x", t);
		expected(temp_array);
	}
}

#endif // _FUNCTIIONS_H
