/*******************************************************************************
 *
 *	occ : The Omega Code Compiler
 *
 *	Copyright (c) 2016 Ammon Dodson
 *
 ******************************************************************************/


#ifndef _ERRORS_H
#define _ERRORS_H


/******************************************************************************/
//                            TYPE DEFINITIONS
/******************************************************************************/


#include <util/msg.h>

#include <stdio.h>
//#include <stdbool.h>
#include <stdlib.h>

//typedef enum {
//	V_QUIET,
//	V_ERROR,
//	V_WARN,
//	V_INFO,
//	V_DEBUG
//} verb_t;


/******************************************************************************/
//                                CONSTANTS
/******************************************************************************/


#define DEFAULT_VERBOSITY (msg_log_lvl)V_NOTE
#define ERR_ARR_SZ  100 ///< temp arrays used in error reporting


/******************************************************************************/
//                             GLOBAL VARIABLES
/******************************************************************************/


#ifdef _GLOBAL_C
	#define EXTERN
#else
	#define EXTERN extern
#endif


//EXTERN verb_t  verbosity; ///< set in Set_files() and never changed again
EXTERN bool    make_debug;
EXTERN FILE *  debug_fd;
//EXTERN jmp_buf anewline;  ///< to facilitate error recovery
EXTERN char    err_array[ERR_ARR_SZ]; ///< Temporary string for messages

#undef EXTERN

extern log_descriptor logfile;

//#ifndef _GLOBAL_C
//extern log_descriptor logfile;
//#endif


/******************************************************************************/
//                         GLOBAL INLINE FUNCTIONS
/******************************************************************************/


static inline void crit_error(const char* message){
	msg_print(NULL, V_ERROR, "CRITICAL: %s.\n", message);
	exit(EXIT_FAILURE);
}


#endif // _ERRORS_H


