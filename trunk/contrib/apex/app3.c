/* app3.c - Application number three which uses the APEX services.

Tested Services:
  Service GET_PROCESS_ID

Scenario:
    Process Master_Test is created.
	Process Proc1 is created
	Test running either during the partition start or during normal operating mode. Final result must
		be the same.


Description:
ProcessMain:
	Call procedure CHECKGET_PROCESS_ID with the following arguments:
		ProcName of Process Proc1
		NO_ERROR as expected error code.
		Process Id returned in the creation of Proc1

----------------------------------------------------------------------------------------------------------------
OUTPUT from WindRiver Platform System ARINC 653 using prinf to show information by the standart output:
	Check successful: CREATE_PROCESS, Process Master Test
	Check successful: CREATE_PROCESS, Process1
	Check successful: GET_PROCESS_ID
	Test GET_PROCESS_ID is correct

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
#define REGULAR_MASTER_PROCESS_PRIORITY 62
#define MAX_CAD 50


/* TRUE =  I find a mistake
   FALSE = I don't find a mistake
*/
struct get_process_id_error_type{
	BOOL id_test;
	BOOL code_test;
};

/* Functions */
struct get_process_id_error_type checkget_process_id(char *name, RETURN_CODE_TYPE code, PROCESS_ID_TYPE idin);
char * codeToStr (RETURN_CODE_TYPE retCode);
void main(void);
void ProcessMT_Master(void);
void Process1(void);
void show_results(struct get_process_id_error_type g_p_i_e);


PROCESS_ATTRIBUTE_TYPE processTable[]=
{
	/* NAME ENTRY_POINT STACK_SIZE BASE_PRIORITY PERIOD TIME_CAPACITY DEADLINE */

	/* name,        entry,  stack, prio,    period, t capacity, deadline */
	{"tProcessMasterT", ProcessMT_Master, 4000, REGULAR_MASTER_PROCESS_PRIORITY+1, 0, TIME_4sec, SOFT},
	{"tProc1", Process1, 4000, REGULAR_MASTER_PROCESS_PRIORITY, 0, TIME_4sec, SOFT},
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
void ProcessMT_Master(void){
	int temp = 1;	/* This code is useful to avoid empty body in translations without the 'prinf' function */

	printf("Master process starts ... \n");
}

/* -----------------------------------------------------------------------------------*/
void Process1(void){
	int temp = 1;	/* This code is useful to avoid empty body in translations without the 'prinf' function */

	printf("Process 1 starts... \n");
}

/* -----------------------------------------------------------------------------------*/
/* 1. Invokes the GET_PROCESS_ID service, passing it the process name as an argument
   2. Compares the returned error code against the expected one.
   3. Check that the returned process id is equal to the expected one
   4. Returns a structure holding a field with the result of the comparisons made in the
      two previous points.
   5. Additionally, outputs the error code returned by the service and the output process ID. */

struct get_process_id_error_type checkget_process_id(char *name, RETURN_CODE_TYPE code, PROCESS_ID_TYPE idin){
	PROCESS_ID_TYPE idout;
	struct get_process_id_error_type g_p_i_e;
	RETURN_CODE_TYPE retCode;
	char sal_code[MAX_CAD];

	/* Init the above structure. I use this way to avoid pointers*/
	g_p_i_e.id_test = FALSE;
	g_p_i_e.code_test = FALSE;

	GET_PROCESS_ID(name, &idout, &retCode);
	CHECK_CODE(": GET_PROCESS_ID ", retCode, sal_code);
    printf("%s\n", sal_code);

	if (idout != idin){
		g_p_i_e.id_test = TRUE;
	}else if (retCode != code){
		g_p_i_e.code_test = TRUE;
	}

	return g_p_i_e;

}

/* -----------------------------------------------------------------------------------*/
/* Show if test GET_PROCESS_ID is successful or not */
void show_results(struct get_process_id_error_type g_p_i_e){
	if ((!g_p_i_e.id_test) && (!g_p_i_e.code_test)){
		printf("Test GET_PROCESS_ID is correct \n");
	}else if (g_p_i_e.id_test){
		printf("The ID is not correct \n");
	}else if (g_p_i_e.code_test){
		printf("The return code is not NOT_ERROR\n");
	}
}

/* -----------------------------------------------------------------------------------*/
/* Inicio de rutina para inicializar el entorno de accion */
void main(void){
	struct get_process_id_error_type g_p_i_e;
	RETURN_CODE_TYPE retCode;
	PROCESS_ID_TYPE procMainIdMT;
	PROCESS_ID_TYPE procMainId1;
	char sal_code[MAX_CAD];

	CREATE_PROCESS (&processTable[0],
                    &procMainIdMT,
                    &retCode);
	CHECK_CODE(": CREATE_PROCESS, Process Master Test ", retCode, sal_code);
    printf("%s\n", sal_code);

	CREATE_PROCESS (&processTable[1],
                    &procMainId1,
                    &retCode);
    CHECK_CODE(": CREATE_PROCESS, Process1 ", retCode, sal_code);
    printf("%s\n", sal_code);

	g_p_i_e = checkget_process_id("tProc1", NO_ERROR, procMainId1);

	show_results(g_p_i_e);

}
