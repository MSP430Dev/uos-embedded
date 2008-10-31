/* 	Aplication number thirdteen which uses APEX services
Tested Services:
	TIMED_WAIT
	START,GET_PROCESS_STATUS, TIMED_WAIT, CREATE_PROCESS, SET_PARTITION_MODE, STOP_SELF();
	UNLOCK_PREEMPTION, LOCK_PREEMPTION haven´t been modeled yet

Scenario:
	Process Master_Test is created with base priority 2 and is aperiodic
	Process P1 is created with base priority 21 and is periodic
	Global variable GV1 is initialized to FALSE
	Global variable GV2 is initialized to FALSE
	Master Partition in NORMAL STATE

Description:
Pseudo-code for process Master_Test:
	Call service UNLOCK_PREEMPTION until the value it outputs for LOCK_LEVEL is equal to 0 (preemption is enabled).

	Call service START with the ID of process P1 as argument (process P1 pre-empts Master_Test)

	Read value of variable GV1

	Call service GET_PROCESS_STATUS using as argument the ID of process P1 and check the process status is WAITING

	Call procedure CHECKTIMED_WAIT with the following arguments:
		DELAY_TIME := REGULAR_TIME_WAIT
		NO_ERROR as expected error code

	Read value of variable GV2

	Test succeeded if read values of GV1 and GV2 are TRUE, procedure call returned no errors.


Pseudo code for process P1
	Set GV1 to TRUE

	Call procedure CHECKTIMED_WAIT with the following arguments:
		DELAY_TIME := REGULAR_TIME_WAIT - MINIMUM_WAIT
		NO_ERROR as expected error code

	Set GV2 to TRUE if procedure call returned no errors

Expected Results:
	The Master_Test process is pre-empted by process P1 once the TIMED_WAIT service is called. Since the process P1
	has a similar priority, the round-robin rescheduling feature will enable it to set to True, variable Gv1.
	Rescheduling is triggered thereafter, enabling the Master_Test process to read the contents of the variables.
	TIMED_WAIT of 99 ms in P1 against 100ms of Master Test is a robustness test to ensure the full precision of TIMED WAIT.

-----------------------------------------------------------------------------------------------------------------------------
OUTPUT from WindRiver Platform System ARINC 653 using prinf and global variables (GV1, GV2) to show information by the standart output:
	Check successful: CREATE_PROCESS Process Master Test
	Check successful: CREATE_PROCESS Process 1
	Check successful: START Process Master Test
	Process Master starts ...
	Check successful: GET_PARTITION_STATUS
	Check successful: GET_PROCESS_ID Process 1
	Process 1 starts ...
	Check successful: START Process 1
	TIMED_WAIT test is successful

OUTPUT from WindRiver Platform System ARINC 653 using prinf (no any global variables) to show information by the standart output:
	Check successful: CREATE_PROCESS Process Master Test
	Check successful: CREATE_PROCESS Process 1
	Check successful: START Process Master Test
	Process Master starts ...
	Check successful: GET_PARTITION_STATUS
	Check successful: GET_PROCESS_ID Process 1
	Process 1 starts ...
	Check successful: START Process 1
	TIMED_WAIT test is successful


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
/* 4 sec */
#define TIME_4sec	4000000000ll
#define REGULAR_TIME_WAIT 2000000000ll
/* Typical low priority for the Master Process */
#define REGULAR_MASTER_PROCESS_PRIORITY 2
/* Typical high priority value for a process */
#define REGULAR_P2_PROCESS_PRIORITY 21
/* 0.1 sec */
#define MINIMUN_WAIT 100000000ll
#define MAX_CAD 50


/* TRUE =  I find a mistake
   FALSE = I don't find a mistake */
struct timed_wait_error_type{
	BOOL test_code;
};


/* Functions */
char * codeToStr (RETURN_CODE_TYPE retCode);
void main(void);
void ProcessMT_Master(void);
void Process1(void);
struct timed_wait_error_type checktimed_wait(SYSTEM_TIME_TYPE time, RETURN_CODE_TYPE code);
void show_results(struct timed_wait_error_type t_w_e);


/* Global variables */
/* Next lines are only to use in WindRiver Platform ARINC 653 but don´t use in Promela. They are global variables  */
/* BOOL GV1;
BOOL GV2; */


/* Definition processes	*/
PROCESS_ATTRIBUTE_TYPE processTable[]=
{
	/* name,        entry,  stack, prio,    period, t capacity, deadline */
	{"tProcessMasterT", ProcessMT_Master, 4000, REGULAR_MASTER_PROCESS_PRIORITY, 0, TIME_4sec, SOFT},
	{"tProc1", Process1, 4000, REGULAR_P2_PROCESS_PRIORITY, 0, TIME_4sec, SOFT},
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
struct timed_wait_error_type checktimed_wait(SYSTEM_TIME_TYPE time, RETURN_CODE_TYPE code){
	struct timed_wait_error_type t_w_e;
	RETURN_CODE_TYPE retCode;

	t_w_e.test_code = FALSE;

	TIMED_WAIT(time, &retCode);
	if (code!=retCode){
		t_w_e.test_code = TRUE;
	}

	return t_w_e;
}

/* -----------------------------------------------------------------------------------*/
void show_results(struct timed_wait_error_type t_w_e){
	/* Next line is only to use in WindRiver Platform ARINC 653 but don´t use in Promela. It uses global variables  */
	/* if ((GV1) && (GV2) && (!t_w_e.test_code)){ */
	/* Instance of last line you can use next line to check the code in SPIN*/
	if (!t_w_e.test_code){
		printf("TIMED_WAIT test is successful \n");
	/* Next two lines are only to use in WindRiver Platform ARINC 653 but don´t use in Promela. It uses global variables  */
	/*}else if (GV1 || GV2){
		printf("Problem with the process 1 \n"); */
	}else if (t_w_e.test_code){
		printf("Problem with TIMED_WAIT service \n");
	}else
		printf("Ha pasado por aqui \n");
}

/* -----------------------------------------------------------------------------------*/
void Process1(void){
	struct timed_wait_error_type t_w_e;

	printf("Process 1 starts ...\n");

	/* Next line is only to use in WindRiver Platform ARINC 653 but don´t use in Promela. It uses global variables  */
	/* GV1 = TRUE; */

	t_w_e = checktimed_wait(REGULAR_TIME_WAIT-MINIMUN_WAIT, NO_ERROR);

	if (!t_w_e.test_code){
		/* Next line is only to use in WindRiver Platform ARINC 653 but don´t use in Promela. It uses global variables  */
		/* GV2 = TRUE; */
	}

	STOP_SELF();
}

/* -----------------------------------------------------------------------------------*/
void ProcessMT_Master(void){
	/*LOCK_LEVEL_TYPE lock; */
	RETURN_CODE_TYPE retCode;
	struct timed_wait_error_type t_w_e;
	PROCESS_STATUS_TYPE status;
	PROCESS_ID_TYPE procMainId1;
	char sal_code[MAX_CAD];
	PARTITION_STATUS_TYPE statusp;

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

	START(procMainId1, &retCode);
	CHECK_CODE(": START Process 1", retCode, sal_code);
    printf("%s\n", sal_code);

	/* process P1 pre-empts Master_Test */

	GET_PROCESS_STATUS(procMainId1, &status, &retCode);
	if (status.PROCESS_STATE == WAITING){
		t_w_e = checktimed_wait(REGULAR_TIME_WAIT, NO_ERROR);
	}else
		printf("GET_PROCESS_STATUS; error. P1 is not a WAITING State \n");

	show_results(t_w_e);

	STOP_SELF();
}

/* -----------------------------------------------------------------------------------*/
/* Entry point */
void main(void){
	char sal_code[MAX_CAD];
	RETURN_CODE_TYPE retCode;
	PROCESS_ID_TYPE procMainIdMT;
	PROCESS_ID_TYPE procMainId1;

	CREATE_PROCESS (&(processTable [0]),       /* process attribute */
                    &procMainIdMT,             /* process Id */
                    &retCode);
	CHECK_CODE(": CREATE_PROCESS Process Master Test", retCode, sal_code);
    printf("%s\n", sal_code);

	/* Process P1 is created with base priority */
	CREATE_PROCESS (&(processTable [1]),       /* process attribute */
                    &procMainId1,             /* process Id */
                    &retCode);
    CHECK_CODE(": CREATE_PROCESS Process 1", retCode, sal_code);
    printf("%s\n", sal_code);

    /* Next line are only to use in WindRiver Platform ARINC 653 but don´t use in Promela. They use global variables  */
	/* GV1 = FALSE;
	GV2 = FALSE; */

	/* To start Master Process ...*/
	START(procMainIdMT, &retCode);
	CHECK_CODE(": START Process Master Test", retCode, sal_code);
    printf("%s\n", sal_code);

	/* To set the partition in a Normal State */
	SET_PARTITION_MODE(NORMAL, &retCode);
	CHECK_CODE(": SET_PARTITION_MODE ", retCode, sal_code);
    printf("%s\n", sal_code);

}
