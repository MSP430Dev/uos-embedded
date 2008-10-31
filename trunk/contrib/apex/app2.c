/* app2.c - Application number two which uses the APEX services.

Tested Services:
  Service SET_PARTITION_MODE,
     	  GET_PARTITION_STATUS

Scenario:
  -Process Master_Test is created and started, with base priority = REGULAR_MASTER_PROCESS_PRIORITY and is aperiodic
  -Master Partition is initializing

Description:
ProcessMain:
	Call procedure CHECKSET_PARTITION_MODE with the following arguments:
		New mode is NORMAL mode
		Expected error code is NO_ERROR

	Check error code

Master_Test:

	Call service GET_PARTITION_STATUS and check that partition is in NORMAL mode

	If partition is in NORMAL mode then the test is successful

----------------------------------------------------------------------------------------------------------------
OUTPUT from WindRiver Platform System ARINC 653 using prinf to show information by the standart output:
	Check successful: CREATE_PROCESS
	Check successful: START
	Starting Process Master ...
	Check successful: GET_PARTITION_STATUS
	The SET_PARTITION_MODE test is successful

Sintax of output:
	Check successful: <ARINC653 Call>  			 -> This lines show us that the ARINC call returned NO_ERROR
	<RETURN_CODE_TYPE errori> : <ARINC653 Call>  -> In case that the call returned any error, this one is printing (typical RETURN_CODE_TYPE error)
	<ProcessN> starts ...              			 -> It is the first line on the top code to the process body
	Rest of printing 				   			 -> Some of them are only to debug and other depends on the Pedro´s model printing.


////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Update Date : September 13th, 2007
This update is IMPORTANT. If you don´t find this mark, the author of this application doesn´t certificate
      that this code can be translated to Promela code by his automatic translator.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

*/

/* includes */

#include "copyright_wrs.h"
#include "vxWorks.h"
#include "apex/apexLib.h"
#include "stdio.h"
#include "string.h"
#include "taskLib.h"
#include "math.h"

/* Macros */
#define CHECK_CODE(msg, code, sal) if (code!=NO_ERROR) strcpy(sal, codeToStr(retCode)); else strcpy(sal, "Check successful"); strcat(sal, msg)

/* Constants*/
#define TIME_4sec	4000000000ll
/* 63 is the  maximum value in the Pedro´s model, check MAX_PRIORITY_VALUE */
#define REGULAR_MASTER_PROCESS_PRIORITY 63
#define MAX_CAD 50


/* TRUE =  I find a mistake
   FALSE = I don't find a mistake
*/
struct set_partition_mode_error_type{
	BOOL code_test;
};

/* Functions */
struct set_partition_mode_error_type checkset_partition_mode(OPERATING_MODE_TYPE new_type, RETURN_CODE_TYPE inCode);
void show_results(struct set_partition_mode_error_type s_p_m_e);
char * codeToStr (RETURN_CODE_TYPE retCode);
void main(void);
void ProcessMaster_test(void);


PROCESS_ATTRIBUTE_TYPE processTable[]=
{
	/* NAME ENTRY_POINT STACK_SIZE BASE_PRIORITY PERIOD TIME_CAPACITY DEADLINE */

	/* name,        entry,  stack, prio,    period, t capacity, deadline */
	{"tProcessMasterT", ProcessMaster_test, 4000, REGULAR_MASTER_PROCESS_PRIORITY, 0, TIME_4sec, SOFT},
	/* Don't touch or move next line */
	{""         ,    NULL,      0,    0,          0,         0, SOFT}
};




/* -----------------------------------------------------------------------------------*/
char * codeToStr (RETURN_CODE_TYPE retCode) {
    switch (retCode){
	case NO_ERROR:
	    return "NO_ERROR"; break;
	case NO_ACTION:
	    return "NO_ACTION"; break;
	case NOT_AVAILABLE:
	    return "NOT_AVAILABLE"; break;
	case INVALID_PARAM:
	    return "INVALID_PARAM"; break;
	case INVALID_CONFIG:
	    return "INVALID_CONFIG"; break;
	case INVALID_MODE:
	    return "INVALID_MODE"; break;
	case TIMED_OUT:
	    return "TIMED_OUT"; break;
	}
    /* should not go here */
    return "Unknown code";
}

/* -----------------------------------------------------------------------------------*/
/* Show if test SET_PARTITION_MODE is successful or not */
void show_results(struct set_partition_mode_error_type s_p_m_e){
	if (!s_p_m_e.code_test){
		printf("Test SET_PARTITION_MODE is correct \n");
	}else{
		printf("The SET_PARTITION_MODE service failed.\n");
	}
}

/* -----------------------------------------------------------------------------------*/
/*
	1. Invokes the SET_PARTITION_MODE service, passing it the received OPERATING_MODE_TYPE
       parameter.
	2. Checks the expected error code against the returned error code, passed as input parameter.
	3. Returns a structure with one field indicating the result of the test made
	   in point 2.																		*/

struct set_partition_mode_error_type checkset_partition_mode(OPERATING_MODE_TYPE mode,
															 RETURN_CODE_TYPE inCode){
	struct set_partition_mode_error_type g_p_i_e;
	RETURN_CODE_TYPE retCode;

	/* Init the above structure. I use this way to avoid pointers*/
	g_p_i_e.code_test = FALSE;

	SET_PARTITION_MODE(mode, &retCode);

	if (retCode != inCode)
	    g_p_i_e.code_test = TRUE;

	return g_p_i_e;

}

/* -----------------------------------------------------------------------------------*/
/* Inicio de rutina para inicializar el proceso Master_test */
void main(void){
	struct set_partition_mode_error_type s_p_m_e;
	RETURN_CODE_TYPE retCode;
	char sal_code[MAX_CAD];
	PROCESS_ID_TYPE procMainId1;

	CREATE_PROCESS (&processTable[0],  /* process attribute */
                    &procMainId1,    /* process Id */
                    &retCode);

	CHECK_CODE(": CREATE_PROCESS", retCode, sal_code);
	printf("%s\n", sal_code);

    /* start the process */
    START (procMainId1, &retCode);
    CHECK_CODE(": START ", retCode, sal_code);
    printf("%s\n", sal_code);

    /* Check the SET_PARTITION_MODE service */
    s_p_m_e = checkset_partition_mode(NORMAL, NO_ERROR);

	show_results(s_p_m_e);

    return;

}

/* -----------------------------------------------------------------------------------*/
/* Process Master_test  */
void ProcessMaster_test(void){
	PARTITION_STATUS_TYPE status;
	RETURN_CODE_TYPE retCode;
	char sal_code[MAX_CAD];

	printf("Starting Process Master ...\n");

	GET_PARTITION_STATUS(&status, &retCode);
	CHECK_CODE(": GET_PARTITION_STATUS ", retCode, sal_code);
	printf("%s\n", sal_code);

	if (status.OPERATING_MODE == NORMAL){
		printf("The SET_PARTITION_MODE test is successful \n");
	}else{
		printf("The partition initialization failed\n");
	}
}

/* -----------------------------------------------------------------------------------*/
