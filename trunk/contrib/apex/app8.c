/* 	Aplication number eight which uses APEX services

Tested Services:
	SUSPEND, hasn´t been modeled yet
	START, GET_PROCESS_STATUS, STOP_SELF, GET_PROCESS_ID
	UNLOCK_PREEMPTION, LOCK_PREEMPTION -> LOCK_PREEMPTION, UNLOCK_PREEMPTION haven´t been modeled yet

Scenario:
	Process Master_Test is created and started with base priority = REGULAR_MASTER_PROCESS_PRIORITY and is aperiodic
	Process P1 is created with base priority = REGULAR_P1_PROCESS_PRIORITY, is aperiodic
	Process P2 is created with base priority = REGULAR_P2_PROCESS_PRIORITY and is aperiodic
	Master Partition in NORMAL STATE

Description:
	Pseudo-code for process Master_Test:
	Call service UNLOCK_PREEMPTION until value for parameter LOCK_LEVEL returned by service is
	equal to 0 (this assures that pre-emption is activated)

	Call service START with the ID of process P1 as argument. (Master_Test is pre-empted by P1)


	Pseudo-code for process P1:
	Call service START with the ID of P2 as argument

	Call procedure CHECK_SUSPEND using as arguments the following values:
		ID of Process P2
		Expected error code is NO_ERROR
		Check OutProcessStatus returns WAITING

	Check that procedure returned no errors

	Pseudo-code for process P2
	Call service STOP_SELF

Expected Results:
	The process P2 is suspended
	All processes call service STOP_SELF at the end of its execution code.

----------------------------------------------------------------------------------------------------------------
OUTPUT from WindRiver Platform System ARINC 653 using prinf to show information by the standart output:
	Check successful: CREATE_PROCESS Master Test
	Check successful: CREATE_PROCESS Process 1
	Check successful: CREATE_PROCESS Process 2
	Check successful: START Master Test
	Process Master starts ...
	Check successful: GET_PARTITION_STATUS
	Check successful: GET_PROCESS_ID Process 1
	Process 1 starts ...
	Check successful: GET_PROCESS_ID Process 2
	Check successful: START Process 2
	SUSPEND test is successful
	Check successful: START Process 1



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
/* Typical high priority value for a process */
#define REGULAR_P1_PROCESS_PRIORITY 42
/* Typical low priority value for a process */
#define REGULAR_P2_PROCESS_PRIORITY 21
#define REGULAR_TIME_WAIT 2000000000ll
#define MAX_CAD 50


/* TRUE =  I find a mistake
   FALSE = I don't find a mistake */
struct suspend_error_type{
	BOOL test_code_suspend;
	BOOL test_code_get;
	BOOL test_status;
};

/* Functions */
char * codeToStr (RETURN_CODE_TYPE retCode);
void main(void);
void ProcessMT_Master(void);
void Process1(void);
void Process2(void);
void show_result(struct suspend_error_type s_e);
struct suspend_error_type checksuspend(PROCESS_ID_TYPE id, RETURN_CODE_TYPE code);


/* Definition processes	*/
PROCESS_ATTRIBUTE_TYPE processTable[]=
{
	/* NAME ENTRY_POINT STACK_SIZE BASE_PRIORITY PERIOD TIME_CAPACITY DEADLINE */

	/* name,        entry,  stack, prio,    period, t capacity, deadline */
	{"tProcessMasterT", ProcessMT_Master, 4000, REGULAR_MASTER_PROCESS_PRIORITY, 0, TIME_4sec, SOFT},
	{"tProc1", Process1, 4000, REGULAR_P1_PROCESS_PRIORITY, 0, TIME_4sec, SOFT},
	{"tProc2", Process2, 4000, REGULAR_P2_PROCESS_PRIORITY, 0, TIME_4sec, SOFT},
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
struct suspend_error_type initStruct(void){
	struct suspend_error_type s_e;

	s_e.test_code_suspend = FALSE;
	s_e.test_code_get = FALSE;
	s_e.test_status = FALSE;

	return s_e;
}

/* -----------------------------------------------------------------------------------*/
/*
1. Call service SUSPEND using as argument the received value for the process ID.
2. Compare the output return code against the expected return code
3. If no error occurred call service GET_PROCESS_STATUS, using as argument the received value
   for the process ID.
4. Check that no error occurred and that the process state is Waiting
5. Return a bit structure indicating any of the following errors:
	a. Expected error code different from returned one.
	b. GET_PROCESS_STATUS invocation failed
	c. StatusProcessState different from Waiting.
6. Output the PROCESS_STATUS structure returned from the invocation of service GET_PROCESS_STATUS.
7. Output the error code returned from the invocation of service SUSPEND */

struct suspend_error_type checksuspend(PROCESS_ID_TYPE id, RETURN_CODE_TYPE code){
	struct suspend_error_type s_e;
	RETURN_CODE_TYPE retCode;
	PROCESS_STATUS_TYPE status;

	s_e = initStruct();

	SUSPEND(id, &retCode);
	if (code!=retCode){
		s_e.test_code_suspend = TRUE;
	}else{
		GET_PROCESS_STATUS(id, &status, &retCode);
		if (retCode!=code){
			s_e.test_code_get = TRUE;
		}

		if (status.PROCESS_STATE != WAITING){
			s_e.test_status = TRUE;
		}
	}

	return s_e;

}

/* -----------------------------------------------------------------------------------*/
void show_result(struct suspend_error_type s_e){
	if ((!s_e.test_code_suspend) && (!s_e.test_code_get) && (!s_e.test_status)){
		printf("SUSPEND test is successful \n");
	}else if (s_e.test_code_suspend){
		printf("Problem with the error code from SUSPEND, it's not the expected one \n");
	}else if (s_e.test_code_get){
		printf("Problem with the error code from GET_PROCESS_STATUS(), it's not the expected one \n");
	}else if (s_e.test_status){
		printf("Problem with the process status, it's not the expected WAITING State \n");
	}

}

/* -----------------------------------------------------------------------------------*/
void Process1(void){
	PROCESS_ID_TYPE procMainId2;
	struct suspend_error_type s_e;
	RETURN_CODE_TYPE retCode;
	char sal_code[MAX_CAD];

	printf("Process 1 starts ...\n");

	/* To get the procMainId2 value */
	GET_PROCESS_ID("tProc2", &procMainId2, &retCode);
	CHECK_CODE(": GET_PROCESS_ID Process 2", retCode, sal_code);
    printf("%s\n", sal_code);

	START(procMainId2 ,&retCode);
	CHECK_CODE(": START Process 2", retCode, sal_code);
    printf("%s\n", sal_code);

	/* TIMED_WAIT(REGULAR_TIME_WAIT, &retCode);
	CHECK_CODE(": TIMED_WAIT", retCode, sal_code);
    printf("%s\n", sal_code);				*/

	s_e = checksuspend(procMainId2, NO_ERROR);

	show_result(s_e);

	STOP_SELF();
}

/* -----------------------------------------------------------------------------------*/
void Process2(void){

	printf("Process 2 starts ...\n");

	STOP_SELF();

}

/* -----------------------------------------------------------------------------------*/
void ProcessMT_Master(void){
	/* LOCK_LEVEL_TYPE lock; */
    PARTITION_STATUS_TYPE statusp;
    RETURN_CODE_TYPE retCode;
    PROCESS_ID_TYPE procMainId1;
    char sal_code[MAX_CAD];

	printf("Process Master starts ...\n");

	GET_PARTITION_STATUS(&statusp, &retCode);
	CHECK_CODE(": GET_PARTITION_STATUS", retCode, sal_code);
    printf("%s\n", sal_code);

	/* Make sure the preemption is enabled, lock == 0 */
	/* if (statusp.LOCK_LEVEL>0){
		UNLOCK_PREEMPTION(&lock, &retCode);
		while (lock!=0){
			UNLOCK_PREEMPTION(&lock, &retCode);
		}
	}else if (statusp.LOCK_LEVEL<0){
		LOCK_PREEMPTION(&lock, &retCode);
		while (lock!=0){
			LOCK_PREEMPTION(&lock, &retCode);
		}
	}*/

	/* To get the procMainId1 value */
	GET_PROCESS_ID("tProc1", &procMainId1, &retCode);
	CHECK_CODE(": GET_PROCESS_ID Process 1", retCode, sal_code);
    printf("%s\n", sal_code);

	START(procMainId1 ,&retCode);
	CHECK_CODE(": START Process 1", retCode, sal_code);
    printf("%s\n", sal_code);

	/* TIMED_WAIT(REGULAR_TIME_WAIT, &retCode); */

	/* Master_Test is pre-empted by P1 */

}

/* -----------------------------------------------------------------------------------*/
/* Entry point */
void main(void){
    PROCESS_ID_TYPE procMainIdMT;
	PROCESS_ID_TYPE procMainId1;
    PROCESS_ID_TYPE procMainId2;
    RETURN_CODE_TYPE retCode;
    char sal_code[MAX_CAD];

	CREATE_PROCESS (&processTable[0],  /* process attribute */
                    &procMainIdMT,    /* process Id */
                    &retCode);
	CHECK_CODE(": CREATE_PROCESS Master Test", retCode, sal_code);
    printf("%s\n", sal_code);

	CREATE_PROCESS (&processTable[1],  /* process attribute */
                    &procMainId1,    /* process Id */
                    &retCode);
	CHECK_CODE(": CREATE_PROCESS Process 1", retCode, sal_code);
    printf("%s\n", sal_code);

	CREATE_PROCESS (&processTable[2],  /* process attribute */
                    &procMainId2,    /* process Id */
                    &retCode);
	CHECK_CODE(": CREATE_PROCESS Process 2", retCode, sal_code);
    printf("%s\n", sal_code);

	START(procMainIdMT, &retCode);
	CHECK_CODE(": START Master Test", retCode, sal_code);
    printf("%s\n", sal_code);

	/* To set the partition in a Normal State */
	SET_PARTITION_MODE(NORMAL, &retCode);
	CHECK_CODE(": SET_PARTITION_MODE", retCode, sal_code);
    printf("%s\n", sal_code);
}

/* -----------------------------------------------------------------------------------*/
