/* 	Aplication number twelve which uses APEX services

Tested Services:
	GET_MY_ID
	CREATE_PROCESS, SET_PARTITION_MODE, START

Scenario:
	Process Master_Test is created.

Description:
	Call procedure CHECKGETMYID with the following arguments:
		NO_ERROR as expected error code.

Expected Results:
	The call to Check My ID will return no errors, by making the GET_MY_ID to return the Master_Test process ID

-----------------------------------------------------------------------------------------------------------------------------
OUTPUT from WindRiver Platform System ARINC 653 using prinf to show information by the standart output:
	Check successful: CREATE_PROCESS Process Master Test
	Check successful: START Process Master Test
	Main Process starts ...
	GET_MY_ID test is successful

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
/* Typical low priority for the Master Process */
#define REGULAR_MASTER_PROCESS_PRIORITY 2
#define MAX_CAD 50


/* TRUE =  I find a mistake
   FALSE = I don't find a mistake */
struct get_my_error_type{
	BOOL test_code;
};

/* Functions */
char * codeToStr (RETURN_CODE_TYPE retCode);
void main(void);
void ProcessMT_Main(void);
struct get_my_error_type checkgetmyid(RETURN_CODE_TYPE code);
void show_results(struct get_my_error_type g_m_e);


/* Definition processes	*/
PROCESS_ATTRIBUTE_TYPE processTable[]=
{
	/* NAME ENTRY_POINT STACK_SIZE BASE_PRIORITY PERIOD TIME_CAPACITY DEADLINE */

	/* name,        entry,  stack, prio,    period, t capacity, deadline */
	{"tProcessMasterT", ProcessMT_Main, 4000, REGULAR_MASTER_PROCESS_PRIORITY, 0, TIME_4sec, SOFT},
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
/*
	1. Call service GET_MY_ID.
	2. Check that returned error code is equal to NO_ERROR
	3. Output time value returned by the service.
	4. Return bit structure with one field, indicating whether the returned error code from the service was
	   no_error.
*/
struct get_my_error_type checkgetmyid(RETURN_CODE_TYPE code){
	PROCESS_ID_TYPE id;
	struct get_my_error_type g_m_e;
	RETURN_CODE_TYPE retCode;

	g_m_e.test_code = FALSE;

	GET_MY_ID(&id, &retCode);
	if (retCode != code){
		g_m_e.test_code = TRUE;
		if (retCode==INVALID_MODE){
			printf("INVALID_MODE, ID: %d \n", id);
		}
	}

	return g_m_e;
}
/* -----------------------------------------------------------------------------------*/
void show_results(struct get_my_error_type g_m_e){
	if (!g_m_e.test_code){
		printf("GET_MY_ID test is successful \n");
	}else
		printf("Expected error code different from the returned one NO_ERROR \n INVALID_MODE: current process has no ID \n");

}
/* -----------------------------------------------------------------------------------*/
void ProcessMT_Main(){
	struct get_my_error_type g_m_e;

	printf("Main Process starts ...\n ");

	g_m_e = checkgetmyid(NO_ERROR);

	show_results(g_m_e);

}

/* -----------------------------------------------------------------------------------*/
/* Entry point */
void main(void){
	RETURN_CODE_TYPE retCode;
	PROCESS_ID_TYPE procMainIdMT;
	char sal_code[MAX_CAD];

	CREATE_PROCESS (&(processTable [0]),       /* process attribute */
                    &procMainIdMT,             /* process Id */
                    &retCode);
    CHECK_CODE(": CREATE_PROCESS Process Master Test", retCode, sal_code);
    printf("%s\n", sal_code);


	/* Start main process ... */
	START(procMainIdMT, &retCode);
	CHECK_CODE(": START Process Master Test", retCode, sal_code);
    printf("%s\n", sal_code);


	/* To set the partition in a Normal State */
	SET_PARTITION_MODE(NORMAL, &retCode);
	CHECK_CODE(": SET_PARTITION_MODE ", retCode, sal_code);
    printf("%s\n", sal_code);

}

/* -----------------------------------------------------------------------------------*/
