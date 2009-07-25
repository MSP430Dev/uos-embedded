/* app1.c - Application number 1 which uses the APEX services.

Tested Services:
Service GET_PARTITION_STATUS

Scenario:
  -Process Master_Test is created with base priority = REGULAR_MASTER_PROCESS_PRIORITY and is
  aperiodic
  -Master Partition is in normal state

Description:
Call procedure CHECKGET_PARTITION_STATUS using as arguments the following values:
  Expected values for the status:
	IDENTIFIER (Partition ID) = value defined in the configuration table
	PERIOD = period defined in the configuration table
	DURATION = duration defined in the configuration table
	LOCK_LEVEL = 0
	OPERATING_MODE = NORMAL
  Expected error code is NO_ERROR

Check that procedure call returned no error

Expected Results:
Service GET_PARTITION_STATUS is called

----------------------------------------------------------------------------------------------------------------
OUTPUT from WindRiver Platform System ARINC 653 using prinf to show information by the standart output:
	Init starts ...
	Check successful: CREATE_PROCESS
	Check successful: Start Process Master

 	Start Process ProcessMT_Master...
	Check successful: GET_PARTITION_STATUS
 	ID: 1
 	PERIOD: 1000000000
 	DURATION: 0
 	LOCK: 250000000
	Partition mode is NORMAL
	Partition is in START_CONDITION
	The GET_PARTITION_STATUS test is successful

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

/* constant */
#define TIME_4sec	4000000000ll
#define REGULAR_MASTER_PROCESS_PRIORITY 63
#define MAX_CAD 50


/* TRUE =  I find a mistake
   FALSE = I don't find a mistake
*/
struct get_partition_status_error_type{
	BOOL id_test;
	BOOL period_test;
	BOOL duration_test;
	BOOL lock_test;
	BOOL operating_mode_test;
	BOOL start_cond_test;
};

/* Functions */
void main(void);
struct get_partition_status_error_type checkget_partition_status(RETURN_CODE_TYPE code, PARTITION_STATUS_TYPE expectedStatus);
PARTITION_STATUS_TYPE init_status(void);
void show_results(struct get_partition_status_error_type g_p_s_e);
struct get_partition_status_error_type initStruct();
void ProcessMT_Master(void);
void show_status(PARTITION_STATUS_TYPE status);
void show_operating_mode(PARTITION_STATUS_TYPE status);
void show_start_cond(PARTITION_STATUS_TYPE status);

PROCESS_ATTRIBUTE_TYPE processTable[]=
{
	/* NAME ENTRY_POINT STACK_SIZE BASE_PRIORITY PERIOD TIME_CAPACITY DEADLINE_TYPE */

	/* name, entry, stack, prio, period, t capacity, deadline */
	{"tProcessMasterT", ProcessMT_Master, 4000, REGULAR_MASTER_PROCESS_PRIORITY,  0, TIME_4sec, SOFT},
	/*{"tPeriod", Period, 4000, 100,  TIME_200mSec, TIME_200mSec, SOFT},*/
	/* Don't touch or move next line */
	{""     ,    NULL,               0,    0,   0, 0, SOFT}
};





/*------------------------------------------------------------------*/
LOCAL char * codeToStr (RETURN_CODE_TYPE retCode){
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


/*------------------------------------------------------------------*/
/* Init the status variable according to the partition xml configuration files */
PARTITION_STATUS_TYPE init_status(void){
	PARTITION_STATUS_TYPE expectedStatus;

	/*
	 * Next parameters are used to check the code in Wind River Platform for Safety Critical ARINC 653
	 */
	/* expectedStatus.IDENTIFIER = 1;
	expectedStatus.PERIOD = 1000000000;
	expectedStatus.DURATION = 250000000;
	expectedStatus.LOCK_LEVEL = 0; */

	/*
	*  Next parameters are used to check the code in spin 4 using the model developed by P. de la Cámara
	*/
	expectedStatus.IDENTIFIER = 0;
	expectedStatus.PERIOD = 1000;
	expectedStatus.DURATION = 1000;
	expectedStatus.LOCK_LEVEL = 0;


	/* Common parameters */
	/* expectedStatus.OPERATING_MODE = COLD_START; */
	expectedStatus.OPERATING_MODE = NORMAL;
	expectedStatus.START_CONDITION = NORMAL_START;

	return expectedStatus;
}

/*------------------------------------------------------------------*/
void show_results(struct get_partition_status_error_type g_p_s_e){
	if ((!g_p_s_e.id_test) && (!g_p_s_e.period_test) && (!g_p_s_e.duration_test)
		&& (!g_p_s_e.lock_test) && (!g_p_s_e.operating_mode_test) && (!g_p_s_e.start_cond_test)){
		printf("The GET_PARTITION_STATUS test is successful \n");
	}else if (g_p_s_e.id_test){
		printf("The ID is not the expected ID 1 \n");
	}else if (g_p_s_e.period_test){
		printf("The Period is not the expected Period \n");
	}else if (g_p_s_e.duration_test){
		printf("The Duration is not the expected Duration \n");
	}else if (g_p_s_e.lock_test) {
		printf("The Lock is not the expected Lock \n");
	}else if (g_p_s_e.operating_mode_test){
		printf("The Operating Mode is not the expected Operating Mode NORMAL \n");
	}else if (g_p_s_e.start_cond_test){
		printf("The Start Condition is not the expected Start Condition one NORMAL_START \n");
	}
}

/*------------------------------------------------------------------*/
struct get_partition_status_error_type initStruct(){

	struct get_partition_status_error_type g_p_s_e;

	g_p_s_e.id_test = FALSE;
	g_p_s_e.period_test = FALSE;
	g_p_s_e.duration_test = FALSE;
	g_p_s_e.lock_test = FALSE;
	g_p_s_e.operating_mode_test = FALSE;
	g_p_s_e.start_cond_test = FALSE;

	return g_p_s_e;
}

/*------------------------------------------------------------------*/
void show_status(PARTITION_STATUS_TYPE status){
	printf(" ID: %Ld \n PERIOD: %Ld \n DURATION: %Ld \n LOCK: %Ld \n",
		status.IDENTIFIER, status.PERIOD, status.DURATION , status.LOCK_LEVEL);
}

/*------------------------------------------------------------------*/
/* Although it´s not going to be executed, we need it to avoid mistakes */
void ProcessMT_Master(void){
	struct get_partition_status_error_type g_p_s_e;
	PARTITION_STATUS_TYPE expectedStatus;

	printf("\n Start Process ProcessMT_Master... \n");

	expectedStatus = init_status();

	g_p_s_e = checkget_partition_status(NO_ERROR, expectedStatus);
	show_results(g_p_s_e);
}

/*------------------------------------------------------------------------------
 Returns a structure with a bit indicating, for the return code and for all the fields
 of the PARTITION_STATUS_TYPE parameter, whether the comparison test was succeeded or not
*/
struct get_partition_status_error_type checkget_partition_status(RETURN_CODE_TYPE code, PARTITION_STATUS_TYPE expectedStatus){
	char sal_code[MAX_CAD];
	struct get_partition_status_error_type g_p_s_e;
	PARTITION_STATUS_TYPE status;
	RETURN_CODE_TYPE retCode;

	g_p_s_e = initStruct();

	GET_PARTITION_STATUS(&status, &retCode);
	CHECK_CODE(": GET_PARTITION_STATUS ", retCode, sal_code);
	printf("%s\n", sal_code);

	show_status(status);

	/* Check status */
	if (expectedStatus.IDENTIFIER != status.IDENTIFIER){
		g_p_s_e.id_test = TRUE;
	}else if (expectedStatus.PERIOD != status.PERIOD){
		g_p_s_e.period_test = TRUE;
	}else if (expectedStatus.DURATION != status.DURATION){
		g_p_s_e.duration_test = TRUE;
	}else if (expectedStatus.LOCK_LEVEL != status.LOCK_LEVEL){
		g_p_s_e.lock_test = TRUE;
	}else if (expectedStatus.OPERATING_MODE != status.OPERATING_MODE){
		/* printf("ExpectedSt: %Ld\n", expectedStatus.OPERATING_MODE); printf("Status: %Ld\n", status.OPERATING_MODE); */
		g_p_s_e.operating_mode_test = TRUE;
	}else if (expectedStatus.START_CONDITION != status.START_CONDITION){
		g_p_s_e.start_cond_test = TRUE;
	}

	show_operating_mode(status);

	show_start_cond(status);


	return g_p_s_e;
}

/*------------------------------------------------------------------------------*/
void show_operating_mode(PARTITION_STATUS_TYPE status){
	switch (status.OPERATING_MODE){
		case IDLE: printf ("Partition mode is IDLE\n");             break;
		case COLD_START: printf ("Partition mode is COLD_START\n"); break;
		case WARM_START: printf ("Partition mode is WARM_START\n"); break;
		case NORMAL: printf ("Partition mode is NORMAL\n");         break;
	}
}

/*------------------------------------------------------------------------------*/
void show_start_cond(PARTITION_STATUS_TYPE status){
	switch(status.START_CONDITION){
		case NORMAL_START: printf("Partition is in START_CONDITION \n");              break;
		case PARTITION_RESTART: printf("Partition is in PARTITION_RESTART \n");       break;
		case HM_MODULE_RESTART: printf("Partition is in HM_MODULE_RESTART \n");       break;
		case HM_PARTITION_RESTART: printf("Partition is in HM_PARTITION_RESTART \n"); break;
	}
}

/*------------------------------------------------------------------*/
/*
 * Aplicacion de inicio
 */
void main(void){
	char sal_code[MAX_CAD];
	PROCESS_ID_TYPE procMainIdMT;
	RETURN_CODE_TYPE retCode;

	printf("Init starts ... \n");

	CREATE_PROCESS (&processTable[0],	/* process attribute */
                        &procMainIdMT,		/* process Id */
                        &retCode);
	CHECK_CODE(": CREATE_PROCESS ", retCode, sal_code);
	printf("%s\n", sal_code);

	START (procMainIdMT, &retCode);
	CHECK_CODE(": Start Process Master ", retCode, sal_code);
	printf("%s\n", sal_code);

	SET_PARTITION_MODE(NORMAL, &retCode);
	CHECK_CODE(": SET_PARTITION_MODE", retCode, sal_code);
	printf("%s\n", sal_code);
}
