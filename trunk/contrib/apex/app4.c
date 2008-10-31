/* app4.c - Application number four which uses the APEX services.

Tested Services:
	Service GET_PROCESS_STATUS

Scenario:
	-Process Master_Test is created.
	-Process Proc1 is defined.
	-Test running either during the partition start or during normal operating mode.
	 Final result must be the same.

Description:

	Call procedure CHECKGET_PROCESS_STATUS with the following arguments:
		ProcessId returned from CHECKCREATE_PROCESS.
		Valid ProcessStatus
		NO_ERROR as expected error code.

----------------------------------------------------------------------------------------------------------------
OUTPUT from WindRiver Platform System ARINC 653 using prinf to show information by the standart output:
	Check successful: CREATE_PROCESS Process Master Test
	Check successful: CREATE_PROCESS Process 1
	GET_PROCESS_STATUS test is successful

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
#define REGULAR_MASTER_PROCESS_PRIORITY 62
#define MAX_CAD 50


/* TRUE =  I find a mistake
   FALSE = I don't find a mistake
*/
struct get_process_status_type{
	BOOL test_attributes;
	BOOL test_current_priority;
	BOOL test_deadline_time;
	BOOL test_process_state;
};

/* Functions */
struct get_process_status_type checkget_process_status(PROCESS_STATUS_TYPE st, PROCESS_ID_TYPE idP, RETURN_CODE_TYPE code);
char * codeToStr (RETURN_CODE_TYPE retCode);
void main(void);
void ProcessMastes_Test(void);
void Process1(void);
void show_resultget_p(struct get_process_status_type g_p_s);
struct get_process_status_type initGet(void);
BOOL compararAtributos(PROCESS_ATTRIBUTE_TYPE pat0, PROCESS_ATTRIBUTE_TYPE pat1);
PROCESS_STATUS_TYPE initStatus();
PROCESS_ATTRIBUTE_TYPE initAttributes();


PROCESS_ATTRIBUTE_TYPE processTable[]=
{
	/* NAME ENTRY_POINT STACK_SIZE BASE_PRIORITY PERIOD TIME_CAPACITY DEADLINE */
	/* name,        entry,  stack, prio,    period, t capacity, deadline */
	{"tProcessMasterT", ProcessMastes_Test, 4000, REGULAR_MASTER_PROCESS_PRIORITY+1, 0, TIME_4sec, SOFT},
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
void ProcessMastes_Test(void){
	int temp = 1;	/* This code is useful to avoid empty body in translations without the 'prinf' function */
	printf("Main process starts ... \n");
}

/* -----------------------------------------------------------------------------------*/
void Process1(void){
	int temp = 1;	/* This code is useful to avoid empty body in translations without the 'prinf' function */
	printf("Process 1 starts... \n");
}

/* -----------------------------------------------------------------------------------*/
void show_resultget_p(struct get_process_status_type g_p_s){

	if ((!g_p_s.test_attributes) && (!g_p_s.test_current_priority) && (!g_p_s.test_deadline_time) &&
			(!g_p_s.test_process_state)){
		printf("GET_PROCESS_STATUS test is successful\n");
	}else if (g_p_s.test_attributes){
		printf("Problem with the attribute, it's not the expected one\n");
	}else if (g_p_s.test_current_priority){
		printf("Problem with the current priority, it's not the expected one\n");
	}else if (g_p_s.test_deadline_time){
		printf("Problem with the deadline_time, it's not the expected one\n");
	}else if (g_p_s.test_process_state){
		printf("Problem with the process state , it's not the expected one\n");
	}
}


/* -----------------------------------------------------------------------------------*/
/* Inicio de rutina para inicializar el entorno de accion */
void main(void){
	struct get_process_status_type g_p_s;
	RETURN_CODE_TYPE retCode;
	PROCESS_ID_TYPE procMainIdMT;
	PROCESS_ID_TYPE procMainId1;
	char sal_code[MAX_CAD];
	PROCESS_STATUS_TYPE status;

	CREATE_PROCESS (&processTable[0],  /* process attribute */
                    &procMainIdMT,    /* process Id */
                    &retCode);

	CHECK_CODE(": CREATE_PROCESS Process Master Test", retCode, sal_code);
    printf("%s\n", sal_code);

    CREATE_PROCESS (&processTable[1],  /* process attribute */
                    &procMainId1,    /* process Id */
                    &retCode);
    CHECK_CODE(": CREATE_PROCESS Process 1", retCode, sal_code);
    printf("%s\n", sal_code);

    /* To get the init values of Process 1*/
    status = initStatus();

	/* check process 1 status */
	g_p_s = checkget_process_status(status, procMainId1, NO_ERROR);

	show_resultget_p(g_p_s);

	SET_PARTITION_MODE(NORMAL, &retCode);
	CHECK_CODE(": SET_PARTITION_MODE", retCode, sal_code);
	printf("%s\n", sal_code);
}


/* -----------------------------------------------------------------------------------*/
/*
1. Checks the returned error code against the expected one.
2. If the returned error code is ok, call the GET_PROCESS_STATUS service using the returned
   process parameter as input parameter.
3. Check that the return code of the method is equal to NO_ERROR.
4. If no error occurred, compare the attribute values returned by the method against the
   expected values for them.
5. Return the returned processId, the process status returned by the GET_PROCESS_STATUS service,
	the error code and one structure indicating the results of the comparisons between the expected values
	passed as parameters and the actual values, returned by the invoked services.

*/
struct get_process_status_type checkget_process_status(PROCESS_STATUS_TYPE statusComp, PROCESS_ID_TYPE idP, RETURN_CODE_TYPE code){
	struct get_process_status_type g_p_e;
	RETURN_CODE_TYPE retCode;
	PROCESS_STATUS_TYPE status;
	char sal_code[MAX_CAD];
	BOOL sonIgualesSt;

	g_p_e = initGet();

    if (retCode == code){
		GET_PROCESS_STATUS(idP, &status, &retCode);
		if (retCode == code){
			sonIgualesSt = compararAtributos(status.ATTRIBUTES, statusComp.ATTRIBUTES);
			if (sonIgualesSt==FALSE){
				g_p_e.test_attributes = TRUE;
			}else if (status.CURRENT_PRIORITY != statusComp.CURRENT_PRIORITY){
				g_p_e.test_current_priority = TRUE;
			}else if (status.DEADLINE_TIME != statusComp.DEADLINE_TIME){
				printf("STATUS: %d.\n", status.DEADLINE_TIME);
				printf("STATUSCOMP: %d.\n", statusComp.DEADLINE_TIME);
				g_p_e.test_deadline_time = TRUE;
			}else if (status.PROCESS_STATE != statusComp.PROCESS_STATE){
				g_p_e.test_process_state = TRUE;
			}
		}else{
			CHECK_CODE(": GET_PROCESS_STATUS", retCode, sal_code);
    		printf("%s\n", sal_code);
    	}
	}

	return g_p_e;
}

/* -----------------------------------------------------------------------------------*/
PROCESS_STATUS_TYPE initStatus(){
	PROCESS_STATUS_TYPE	st;

	st.ATTRIBUTES = initAttributes();
	st.CURRENT_PRIORITY = REGULAR_MASTER_PROCESS_PRIORITY;
	st.DEADLINE_TIME = 0;
	st.PROCESS_STATE = DORMANT;

	return st;

}
/* -----------------------------------------------------------------------------------*/
PROCESS_ATTRIBUTE_TYPE initAttributes(){
	PROCESS_ATTRIBUTE_TYPE pat;

	strcpy (pat.NAME, "tProc1");
	pat.STACK_SIZE = 4000;
	pat.BASE_PRIORITY = REGULAR_MASTER_PROCESS_PRIORITY;
	pat.PERIOD = 0;
	pat.TIME_CAPACITY = TIME_4sec;
	pat.DEADLINE = SOFT;

	return pat;
}

/* -----------------------------------------------------------------------------------*/
struct get_process_status_type initGet(void){
	struct get_process_status_type g_p_e;

	g_p_e.test_attributes = FALSE;
	g_p_e.test_current_priority = FALSE;
	g_p_e.test_deadline_time = FALSE;
	g_p_e.test_process_state = FALSE;

	return g_p_e;
}

/* -----------------------------------------------------------------------------------*/
BOOL compararAtributos(PROCESS_ATTRIBUTE_TYPE pat0, PROCESS_ATTRIBUTE_TYPE pat1){
	BOOL iguales = TRUE;

	if (strcmp(pat0.NAME, pat1.NAME)!=0){
		iguales = FALSE;
	}else if (pat0.STACK_SIZE != pat1.STACK_SIZE){
		iguales = FALSE;
	}else if (pat0.BASE_PRIORITY != pat1.BASE_PRIORITY){
		iguales = FALSE;
	}else if (pat0.PERIOD != pat1.PERIOD){
		iguales = FALSE;
	}else if (pat0.TIME_CAPACITY != pat1.TIME_CAPACITY){
		iguales = FALSE;
	}else if (pat0.DEADLINE != pat1.DEADLINE){
		iguales = FALSE;
	}

	return iguales;
}
