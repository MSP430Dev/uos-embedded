/*
	Aplication number seven which uses APEX services

Tested Services:
	SUSPEND_SELF
	START, TIMED_WAIT, STOP_SELF, GET_PROCESS_ID
	UNLOCK_PREEMPTION, LOCK_PREEMPTION -> LOCK_PREEMPTION, UNLOCK_PREEMPTION haven´t been modeled yet

Scenario:
  Process Master_Test is created with base priority = REGULAR_MASTER_PROCESS_PRIORITY and is aperiodic
  Process P1 is created with base priority = HIGH_PROCESS_PRIORITY and is aperiodic
  Master Partition in NORMAL STATE
  Global variable GV1 is initialized to FALSE
  Global variable GV2 is initialized to FALSE

Description:

Pseudo-code for process Master_Test:
	Call service UNLOCK_PREEMPTION until the value it outputs for LOCK_LEVEL is equal to 0 (to ensure
	that pre-emption is enabled).

	Call service START with the ID of process P1 as argument. (Master_Test is pre-empted by P1)

	Check that GV1=TRUE and GV2=FALSE

	Call service TIMED_WAIT with delay value of REGULAR_TIME_WAIT

	Check that GV2 and GV1 are TRUE

	Test succeeds if GV1 and GV2 values are TRUE, otherwise fails

Pseudo code for process P1
	Set GV1 to TRUE

	Call procedure CHECKSUSPEND_SELF with the following parameters:
		Timeout = MINIMUM_WAIT
		Expected error is TIMED_OUT

	Check that procedure returned no errors

	Set GV2 to TRUE if procedure call returned no errors

Expected Results:
	Process P1 is suspended and resumes after its timeout expires. No errors occur in service call
	All processes call service STOP_SELF at the end of its execution code.

----------------------------------------------------------------------------------------------------------------
OUTPUT from WindRiver Platform System ARINC 653 using prinf and global variables (GV1, GV2) to show information by the standart output:
	Check successful: CREATE_PROCESS Process Master Test
	Check successful: CREATE_PROCESS Process 1
	Check successful: START Process Master Test
	Process Master starts ...
	Check successful: GET_PARTITION_STATUS
	Check successful: GET_PROCESS_ID Process 1
	Process 1 starts ...
	Check successful: START Process 1
	Test0 is successful
	SUSPEND_SELF test is successful
	Check successful: TIMED_WAIT
	Test1 is successful

OUTPUT from WindRiver Platform System ARINC 653 using prinf (no any global variables) to show information by the standart output:
	Check successful: CREATE_PROCESS Process Master Test
	Check successful: CREATE_PROCESS Process 1
	Check successful: START Process Master Test
	Process Master starts ...
	Check successful: GET_PARTITION_STATUS
	Check successful: GET_PROCESS_ID Process 1
	Process 1 starts ...
	Check successful: START Process 1
	SUSPEND_SELF test is successful
	Check successful: TIMED_WAIT


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
/* High priority for a process */
#define HIGH_PROCESS_PRIORITY 63
/* Normal value needed to ensure that most of the time waits defined in the specification are met */
#define REGULAR_TIME_WAIT 2000000000ll
/* Minimum non-null wait, 0.1 sec */
#define MINIMUM_WAIT 100000000ll
#define MAX_CAD 50


/* TRUE =  I find a mistake
   FALSE = I don't find a mistake */
struct suspend_self_error_type{
	BOOL test_code;
};

/* Functions */
char * codeToStr (RETURN_CODE_TYPE retCode);
void main(void);
struct suspend_self_error_type initStruct(void);
struct suspend_self_error_type checksuspend_self(SYSTEM_TIME_TYPE timeout, RETURN_CODE_TYPE code);
void ProcessMT_Master(void);
void Process1(void);


/* Definition processes	*/
PROCESS_ATTRIBUTE_TYPE processTable[]=
{
	/* NAME ENTRY_POINT STACK_SIZE BASE_PRIORITY PERIOD TIME_CAPACITY DEADLINE */

	/* name,        entry,  stack, prio,    period, t capacity, deadline */
	{"tProcessMasterT", ProcessMT_Master, 4000, REGULAR_MASTER_PROCESS_PRIORITY, 0, TIME_4sec, SOFT},
	{"tProc1", Process1, 4000, HIGH_PROCESS_PRIORITY, 0, TIME_4sec, SOFT},
	/* Don't touch or move next line */
	{""         ,    NULL,      0,    0,          0,         0, SOFT}
};

/* Global variables */
/* Next variables are only to use in WindRiver Platform ARINC 653 but don´t use them in Promela.*/
/* BOOL GV1;
BOOL GV2; */



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
struct suspend_self_error_type initStruct(){
	struct suspend_self_error_type s_s_e;

	s_s_e.test_code =FALSE;

	return s_s_e;
}
/* -----------------------------------------------------------------------------------*/
/*
	1. Call service SUSPEND_SELF, using as argument the received value for the timeout.
	2. Check the return code and compare it with the expected return code, passed as parameter.
	   Return the result of the comparison.
*/
struct suspend_self_error_type checksuspend_self(SYSTEM_TIME_TYPE timeout, RETURN_CODE_TYPE code){
	struct suspend_self_error_type s_s_e;
    RETURN_CODE_TYPE retCode;

	s_s_e = initStruct();

	SUSPEND_SELF(timeout, &retCode);
	if (retCode!=code){
		s_s_e.test_code = TRUE;
	}

	return s_s_e;
}

/* -----------------------------------------------------------------------------------*/
/* Task of process 1 */
void Process1(void){
	struct suspend_self_error_type s_s_e;

	printf("Process 1 starts ...\n");

	/* Next line is only to use in WindRiver Platform ARINC 653 but don´t use in Promela. It uses global variables  */
	/* GV1 = TRUE; */
	/* //////////////////////////////////////////////////////////////////////////////////////////////////////////// */

	s_s_e = checksuspend_self(MINIMUM_WAIT, TIMED_OUT);

	if (!s_s_e.test_code){ /* everything is correct in checksuspend_self*/
		/* GV2 = TRUE;  */
		printf("SUSPEND_SELF test is successful \n");
	}else
		printf("The SUSPEND_SELF return code doesn't match with the expected one TIMED_OUT \n");

	STOP_SELF();
}

/* -----------------------------------------------------------------------------------*/
/* Task of process Master Test*/
void ProcessMT_Master(void){
	/* LOCK_LEVEL_TYPE lock;  */
	PARTITION_STATUS_TYPE statusp;
    RETURN_CODE_TYPE retCode;
    PROCESS_ID_TYPE procMainId1;
    char sal_code[MAX_CAD];

	printf("Process Master starts ...\n");

	GET_PARTITION_STATUS(&statusp, &retCode);
	CHECK_CODE(": GET_PARTITION_STATUS ", retCode, sal_code);
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
	} */

	/* To get the procMainId1 value */
	GET_PROCESS_ID("tProc1", &procMainId1, &retCode);
	CHECK_CODE(": GET_PROCESS_ID Process 1", retCode, sal_code);
    printf("%s\n", sal_code);

	START(procMainId1, &retCode);
	CHECK_CODE(": START Process 1 ", retCode, sal_code);
    printf("%s\n", sal_code);

	/* Master_Test is pre-empted by P1 */

	/* Next 'if/else' is only to use in WindRiver Platform ARINC 653 but don´t use in Promela. It uses global variables  */
	/* if ((GV1==TRUE) && (GV2==FALSE)){
		printf("Test0 is successful \n");
	}else
		printf("Scheduling fails 0...");  */
	/* ///////////////////////////////////////////////////////////////////////////////////////////////////////////////// */

	TIMED_WAIT(REGULAR_TIME_WAIT, &retCode);
	CHECK_CODE(": TIMED_WAIT ", retCode, sal_code);
    printf("%s\n", sal_code);

	/* Next 'if/else' is only to use in WindRiver Platform ARINC 653 but don´t use in Promela. It uses global variables  */
	/* if ((GV1==TRUE) && (GV2==TRUE)){
		printf("Test1 is successful \n");
	}else
		printf("Scheduling fails 1..."); */
	/* ///////////////////////////////////////////////////////////////////////////////////////////////////////////////// */

	STOP_SELF();

}

/* -----------------------------------------------------------------------------------*/
/* Entry point */
void main(void){
    PROCESS_ID_TYPE procMainIdMT;
    PROCESS_ID_TYPE procMainId1;
    RETURN_CODE_TYPE retCode;
    char sal_code[MAX_CAD];

	CREATE_PROCESS (&processTable[0],  /* process attribute */
                    &procMainIdMT,    /* process Id */
                    &retCode);
	CHECK_CODE(": CREATE_PROCESS Process Master Test ", retCode, sal_code);
    printf("%s\n", sal_code);

	CREATE_PROCESS (&processTable[1],  /* process attribute */
                    &procMainId1,    /* process Id */
                    &retCode);
    CHECK_CODE(": CREATE_PROCESS Process 1 ", retCode, sal_code);
    printf("%s\n", sal_code);

    /* Next code is only to use in WindRiver Platform ARINC 653 but don´t use in Promela. It uses global variables  */
    /* GV1 = FALSE;
	GV2 = FALSE; */
    /* //////////////////////////////////////////////////////////////////////////////////////////////////////////// */

	START(procMainIdMT, &retCode);
	CHECK_CODE(": START Process Master Test ", retCode, sal_code);
    printf("%s\n", sal_code);

	SET_PARTITION_MODE(NORMAL, &retCode);
	CHECK_CODE(": SET_PARTITION_MODE ", retCode, sal_code);
    printf("%s\n", sal_code);
}

/* -----------------------------------------------------------------------------------*/
