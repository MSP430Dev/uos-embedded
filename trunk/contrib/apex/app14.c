/* 	Aplication number fourteen which uses APEX services

Tested Services:
	PERIODIC_WAIT
	START, GET_TIME ,GET_PROCESS_STATUS, SET_PRIORITY
	UNLOCK_PREEMPTION, LOCK_PREEMPTION haven´t been modeled yet

Scenario:
	Process Master_Test is created with base priority = REGULAR_MASTER_PROCESS_PRIORITY and is aperiodic
	Process P1 is created with base priority = REGULAR_P100_PROCESS_PRIORITY and is periodic
	Master Partition in NORMAL STATE
	Global variable GV1 is initialized to FALSE
	Global variable GV2 is initialized to FALSE
	Global variable NewDeadlineTime is initialized to 0

Description:
Pseudo-code for process Master_Test:
	Call service UNLOCK_PREEMPTION until the value it outputs for LOCK_LEVEL is equal to 0.

	Call service SET_PRIORITY and set Master_Test priority above P1 priority (as such P1 will not preempt Master_Test)

	Call service START with the ID of process P1 as argument (process P1 pre-empts Master_Test)

	Call service GET_PROCESS_STATUS using as argument the ID of process P1 and check the process
	status is either READY or WAITING and set InitialDeadlineTime to the current value of DEADLINE_TIME.

	Call service SET_PRIORITY and set Master_Test to initial value (as such P1 will be able to pre-empt Master_Test)

	WHILE GV1 is FALSE
		Call service GET_PROCESS_STATUS using as argument the ID of process P1 and check the
		process status is WAITING
		Call service TIMED_DELAY with parameter:
		Delay = MINIMUM_WAIT
	END WHILE

	Check value of GV1 is TRUE

	Check value of DEADLINE_TIME is different from value kept previously in InitialDeadlineTime.

	WHILE process status is NOT DORMANT
		Call service GET_PROCESS_STATUS using as argument the ID of process P1
		Call service TIMED_DELAY with parameter:
		Delay = MINIMUM_WAIT
	END WHILE

	Test succeeded if GV1 and GV2 are TRUE, all checks were OK and all procedures returned no errors.

Pseudo code for process P1
	Set value of GV1 to TRUE
	Call procedure CHECKPERIODIC_WAIT with the following arguments:
	NO_ERROR as expected error code

	Check that procedure call returned no errors

	Set GV2 to TRUE if previous checks were all TRUE

Expected Results:
	Process P1 is suspended and pending on a queue. Process is restarted and it is no longer pending on the queue

-----------------------------------------------------------------------------------------------------------------------------
OUTPUT from WindRiver Platform System ARINC 653 using prinf and global variables (GV1, GV2) to show information by the standart output:

	Check successful: CREATE_PROCESS Process Master
	Check successful: CREATE_PROCESS Process 1
	Check successful: START Process Master Test
	Main Process starts ...
	Check successful: GET_PARTITION_STATUS
	Check successful: GET_PROCESS_ID Process Master Test
	Check successful: SET_PRIORITY executed in Process Master
	Check successful: GET_PROCESS_ID Process 1
	Check successful: START Process 1
	Check successful: GET_PROCESS_STATUS Process 1 by Process Master
	Check successful: SET_PRIORITY Process 1 by Process Master
	Check successful: TIMED_WAIT
	Process 1 starts ...
	Check successful: TIMED_WAIT
	Check successful: GET_PROCESS_STATUS
	Check successful: TIMED_WAIT
	Check successful: GET_PROCESS_STATUS
	Check successful: TIMED_WAIT
	Check successful: GET_PROCESS_STATUS
	Check successful: TIMED_WAIT
	Check successful: GET_PROCESS_STATUS
	Check successful: TIMED_WAIT
	Check successful: GET_PROCESS_STATUS
	Check successful: TIMED_WAIT
	Check successful: GET_PROCESS_STATUS
	Check successful: TIMED_WAIT
	Check successful: GET_PROCESS_STATUS
	Check successful: TIMED_WAIT
	Check successful: GET_PROCESS_STATUS
	Check successful: TIMED_WAIT
	Check successful: GET_PROCESS_STATUS
	Check successful: TIMED_WAIT
	Check successful: GET_PROCESS_STATUS
	Check successful: TIMED_WAIT
	Check successful: GET_PROCESS_STATUS
	Check successful: TIMED_WAIT
	Check successful: GET_PROCESS_STATUS
	Check successful: TIMED_WAIT
	Check successful: GET_PROCESS_STATUS
	Check successful: TIMED_WAIT
	Check successful: GET_PROCESS_STATUS
	Check successful: TIMED_WAIT
	Check successful: GET_PROCESS_STATUS
	Check successful: TIMED_WAIT
	Check successful: GET_PROCESS_STATUS
	Check successful: TIMED_WAIT
	Check successful: GET_PROCESS_STATUS
	PERIODIC_WAIT test is successful

OUTPUT from WindRiver Platform System ARINC 653 using prinf (no using any global variables) to show information by the standart output:
	Check successful: CREATE_PROCESS Process Master
	Check successful: CREATE_PROCESS Process 1
	Check successful: START Process Master Test
	Main Process starts ...
	Check successful: GET_PARTITION_STATUS
	Check successful: GET_PROCESS_ID Process Master Test
	Check successful: SET_PRIORITY executed in Process Master
	Check successful: GET_PROCESS_ID Process 1
	Check successful: START Process 1
	Check successful: GET_PROCESS_STATUS Process 1 by Process Master
	Check successful: SET_PRIORITY Process 1 by Process Master
	Process 1 starts ...
	Check successful: TIMED_WAIT
	Check successful: GET_PROCESS_STATUS
	Check successful: TIMED_WAIT
	Check successful: GET_PROCESS_STATUS
	Check successful: TIMED_WAIT
	Check successful: GET_PROCESS_STATUS
	Check successful: TIMED_WAIT
	Check successful: GET_PROCESS_STATUS
	Check successful: TIMED_WAIT
	Check successful: GET_PROCESS_STATUS
	Check successful: TIMED_WAIT
	Check successful: GET_PROCESS_STATUS
	Check successful: TIMED_WAIT
	Check successful: GET_PROCESS_STATUS
	Check successful: TIMED_WAIT
	Check successful: GET_PROCESS_STATUS
	Check successful: TIMED_WAIT
	Check successful: GET_PROCESS_STATUS
	Check successful: TIMED_WAIT
	Check successful: GET_PROCESS_STATUS
	Check successful: TIMED_WAIT
	Check successful: GET_PROCESS_STATUS
	Check successful: TIMED_WAIT
	Check successful: GET_PROCESS_STATUS
	Check successful: TIMED_WAIT
	Check successful: GET_PROCESS_STATUS
	Check successful: TIMED_WAIT
	Check successful: GET_PROCESS_STATUS
	Check successful: TIMED_WAIT
	Check successful: GET_PROCESS_STATUS
	Check successful: TIMED_WAIT
	Check successful: GET_PROCESS_STATUS
	PERIODIC_WAIT test is successful


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
#define TIME_2sec	2000000000ll
/* Minimun non-null wait					   */
#define MINIMUM_WAIT 100000000ll
/* Typical low priority for the Master Process */
#define REGULAR_MASTER_PROCESS_PRIORITY 2
/* Typical priority value for the periodic test processes */
#define REGULAR_P100_PROCESS_PRIORITY 45
#define MAX_CAD 50


/* TRUE =  I find a mistake
   FALSE = I don't find a mistake */
struct periodic_wait_error_type{
	BOOL test_code;
};


/* Functions */
char * codeToStr (RETURN_CODE_TYPE retCode);
void main(void);
void ProcessMT_Master(void);
void Process1(void);
void show_results(struct periodic_wait_error_type p_w_e);
struct periodic_wait_error_type initStruct(void);
struct periodic_wait_error_type checkperiodic_wait(RETURN_CODE_TYPE code);


/* Definition processes	*/
PROCESS_ATTRIBUTE_TYPE processTable[]=
{
	/* NAME ENTRY_POINT STACK_SIZE BASE_PRIORITY PERIOD TIME_CAPACITY DEADLINE */

	/* name,        entry,  stack, prio,    period, t capacity, deadline */
	{"tProcessMasterT", ProcessMT_Master, 4000, REGULAR_MASTER_PROCESS_PRIORITY, 0, TIME_4sec, SOFT},
	{"tProc1", Process1, 4000, REGULAR_P100_PROCESS_PRIORITY, TIME_2sec, TIME_2sec, SOFT},
	/* Don't touch or move next line */
	{""         ,    NULL,      0,    0,          0,         0, SOFT}
};


/* Global variables */
/* Next lines are only to use in WindRiver Platform ARINC 653 but don´t use in Promela. They are global variables  */
/* BOOL GV1;
BOOL GV2;  */



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
void show_results(struct periodic_wait_error_type p_w_e){
	if (!p_w_e.test_code)
		printf("PERIODIC_WAIT test is successful \n");
	else
		printf("ERROR IN PERIODIC_WAIT \n");
}

/* -----------------------------------------------------------------------------------*/
void ProcessMT_Master(void){
	/* LOCK_LEVEL_TYPE lock; */
	PARTITION_STATUS_TYPE statusp;
	PROCESS_STATUS_TYPE status;
	RETURN_CODE_TYPE retCode;
	PROCESS_ID_TYPE procMainIdMT;
	PROCESS_ID_TYPE procMainId1;
	char sal_code[MAX_CAD];
	SYSTEM_TIME_TYPE InitialDeadlineTime;

	printf("Main Process starts ... \n");

	InitialDeadlineTime = 0;

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

	/* To get the procMainIdMT value */
	GET_PROCESS_ID("tProcessMasterT", &procMainIdMT, &retCode);
	CHECK_CODE(": GET_PROCESS_ID Process Master Test", retCode, sal_code);
    printf("%s\n", sal_code);

	SET_PRIORITY(procMainIdMT, REGULAR_P100_PROCESS_PRIORITY, &retCode);
	CHECK_CODE(": SET_PRIORITY executed in Process Master", retCode, sal_code);
    printf("%s\n", sal_code);


	/* P1 will not preempt Master_Test */

	/* To get the procMainId1 value */
	GET_PROCESS_ID("tProc1", &procMainId1, &retCode);
	CHECK_CODE(": GET_PROCESS_ID Process 1", retCode, sal_code);
    printf("%s\n", sal_code);

	START(procMainId1, &retCode);
	CHECK_CODE(": START Process 1", retCode, sal_code);
    printf("%s\n", sal_code);

	/* process P1 pre-empts Master_Test */

	GET_PROCESS_STATUS(procMainId1, &status, &retCode);
	CHECK_CODE(": GET_PROCESS_STATUS Process 1 by Process Master ", retCode, sal_code);
    printf("%s\n", sal_code);

	if ((status.PROCESS_STATE != READY) && (status.PROCESS_STATE != WAITING)){
		printf("PROCESS 1 is not READY or WAITING state \n");
	}

	/*if (status.PROCESS_STATE == READY){
		printf("READY \n");
	}else if (status.PROCESS_STATE == DORMANT){
		printf("DORMANT \n");
	}else if (status.PROCESS_STATE == RUNNING){
		printf("RUNNING \n");
	}else if (status.PROCESS_STATE == WAITING){
		printf("WAITING \n");
	} */

	InitialDeadlineTime = status.DEADLINE_TIME;

	SET_PRIORITY(procMainId1, REGULAR_MASTER_PROCESS_PRIORITY, &retCode);
	CHECK_CODE(": SET_PRIORITY Process 1 by Process Master ", retCode, sal_code);
    printf("%s\n", sal_code);

	/* P1 will be able to pre-empt Master_Test */

	/* Next line is only to use in WindRiver Platform ARINC 653 but don´t use in Promela. It uses a global variable  */
	/* while (!GV1){ */
		GET_PROCESS_STATUS(procMainId1, &status, &retCode);
		if (status.PROCESS_STATE != WAITING){
			printf("P1 is not in WAITING state, error \n");
		}

		TIMED_WAIT(MINIMUM_WAIT, &retCode);
		CHECK_CODE(": TIMED_WAIT", retCode, sal_code);
    	printf("%s\n", sal_code);
	/* } */

	/* Next three lines are only to use in WindRiver Platform ARINC 653 but don´t use in Promela. It uses a global variable  */
	/* if (!GV1){
		printf("GV1 must be TRUE \n");
	}*/

	if (InitialDeadlineTime != status.DEADLINE_TIME){
		printf("Both deadlines haven't to be different \n");
	}

	while (status.PROCESS_STATE != DORMANT){
		GET_PROCESS_STATUS(procMainId1, &status, &retCode);
		CHECK_CODE(": GET_PROCESS_STATUS ", retCode, sal_code);
    	printf("%s\n", sal_code);

		TIMED_WAIT(MINIMUM_WAIT, &retCode);
		CHECK_CODE(": TIMED_WAIT ", retCode, sal_code);
    	printf("%s\n", sal_code);
	}

	/* Next three lines is only to use in WindRiver Platform ARINC 653 but don´t use in Promela. It uses global variables  */
	/*	if ((GV1) && (GV2)){
		printf("if you don't see any error, it's because of the test is successful \n");
	} */

}

/* -----------------------------------------------------------------------------------*/
void Process1(void){

	struct periodic_wait_error_type p_w_e;

	printf("Process 1 starts ... \n");

	/* Next line is only to use in WindRiver Platform ARINC 653 but don´t use in Promela. It uses a global variable  */
	/* GV1 = TRUE; */

	p_w_e = checkperiodic_wait(NO_ERROR);

	show_results(p_w_e);

	/* Next if-line is only to use in WindRiver Platform ARINC 653 but don´t use in Promela. It uses a global variable  */
	/* if (!p_w_e.test_code){
		GV2 = TRUE;
	} */
}

/* -----------------------------------------------------------------------------------*/
struct periodic_wait_error_type initStruct(){
	struct periodic_wait_error_type p_w_e;

	p_w_e.test_code = FALSE;

	return p_w_e;
}

/* -----------------------------------------------------------------------------------*/
/* 	Call service PERIODIC_WAIT.
	Return a structure indicating whether there was a difference between the expected
	return error code and the actual returned error code..  */
struct periodic_wait_error_type checkperiodic_wait(RETURN_CODE_TYPE code){
	struct periodic_wait_error_type p_w_e;
	RETURN_CODE_TYPE retCode;

	p_w_e = initStruct();

	PERIODIC_WAIT(&retCode);
	if (retCode != code){
		p_w_e.test_code = TRUE;
	}

	return p_w_e;
}

/* -----------------------------------------------------------------------------------*/
/* Entry point */
void main(void){
	RETURN_CODE_TYPE retCode;
	char sal_code[MAX_CAD];
	PROCESS_ID_TYPE procMainIdMT;
	PROCESS_ID_TYPE procMainId1;

	CREATE_PROCESS (&(processTable [0]),       /* process attribute */
                    &procMainIdMT,             /* process Id */
                    &retCode);
	CHECK_CODE(": CREATE_PROCESS Process Master ", retCode, sal_code);
    printf("%s\n", sal_code);


	/* Process P1 is created with base priority */
	CREATE_PROCESS (&(processTable [1]),       /* process attribute */
                    &procMainId1,             /* process Id */
                    &retCode);
    CHECK_CODE(": CREATE_PROCESS Process 1 ", retCode, sal_code);
    printf("%s\n", sal_code);


	/* To start Master Process ...*/
	START(procMainIdMT, &retCode);
	CHECK_CODE(": START Process Master Test ", retCode, sal_code);
    printf("%s\n", sal_code);

	/* Next two lines are only to use in WindRiver Platform ARINC 653 but don´t use in Promela. It uses global variables */
	/* GV1 = FALSE;
	GV2 = FALSE; */

	/* To set the partition in a Normal State */
	SET_PARTITION_MODE(NORMAL, &retCode);
	CHECK_CODE(": SET_PARTITION_MODE ", retCode, sal_code);
    printf("%s\n", sal_code);

}
