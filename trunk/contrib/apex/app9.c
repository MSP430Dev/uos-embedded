/* 	Aplication number nine which uses APEX services


Tested Services:
	RESUME,
	START, SUSPEND_SELF,GET_PROCESS_STATUS, STOP_SELF
	UNLOCK_PREEMPTION, LOCK_PREEMPTION -> LOCK_PREEMPTION, UNLOCK_PREEMPTION haven´t been modeled yet

Scenario:
	Process Master_Test is created and started with priority = REGULAR_MASTER_PROCESS_PRIORITY and is aperiodic
	Process P1 is created with base priority = REGULAR_P1_PROCESS_PRIORITY, is aperiodic
	Master Partition in NORMAL STATE

Description:
	Pseudo-code for process Master_Test:
	Call service UNLOCK_PREEMPTION until the value LOCK_LEVEL, returned by the service, is equal to 0 (this
	ensures that pre-emption is enabled)

	Call service START with the ID of process P1 as argument. (Master_Test is pre-empted by P1)

	Call service GET_PROCESS_STATUS using as argument the ID of process P1

	Check GET_PROCESS_STATUS returned WAITING and GV1 is FALSE

	Call procedure CHECK_RESUME with the following arguments:
		ID of process P1
		Expected value equal to NO_ERROR

	Check that procedure returned no errors.

	Test succeeds if the procedure call didn't return an error and GV1 value is TRUE, otherwise it fails.



	Pseudo-code for process P1:
	Call service SUSPEND_SELF, using as argument the following values:
		ID of process P1
		Timeout = HALF_REGULAR_TIME_WAIT

	Set GV1 value to TRUE

Expected Results:
	Process P1 is suspends itself and then it is resumed by Master_Test
	All processes call service STOP_SELF at the end of its execution code.


----------------------------------------------------------------------------------------------------------------
OUTPUT from WindRiver Platform System ARINC 653 using prinf and global variable (GV1) to show information by the standart output:
	Check successful: CREATE_PROCESS Process Master Test
	Check successful: CREATE_PROCESS Process 1
	Check successful: START Process Master Test
	Process Master starts ...
	Check successful: GET_PARTITION_STATUS in Master Test
	Check successful: GET_PROCESS_ID Process 1
	Process 1 starts ...
	Check successful: START Process 1 by Master Process
	Check successful: GET_PROCESS_STATUS in Master Test
	Check successful: SUSPEND_SELF
	RESUME test is successful


OUTPUT from WindRiver Platform System ARINC 653 using prinf (no any global variable) to show information by the standart output:
	Check successful: CREATE_PROCESS Process Master Test
	Check successful: CREATE_PROCESS Process 1
	Check successful: START Process Master Test
	Process Master starts ...
	Check successful: GET_PARTITION_STATUS in Master Test
	Check successful: GET_PROCESS_ID Process 1
	Process 1 starts ...
	Check successful: START
	Check successful: GET_PROCESS_STATUS in Master Test
	Check successful: SUSPEND_SELF
	RESUME test is successful


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
#define HALF_REGULAR_TIME_WAIT 2000000000ll
/* Typical low priority for the Master Process */
#define REGULAR_MASTER_PROCESS_PRIORITY 2
 /* Typical high priority value for a process */
#define REGULAR_P1_PROCESS_PRIORITY 42
#define MAX_CAD 50


/* TRUE =  I find a mistake
   FALSE = I don't find a mistake */
struct resume_error_type{
	BOOL test_code_res;
	BOOL test_state;
	BOOL test_code_getstatus;
};


/* Functions */
char * codeToStr (RETURN_CODE_TYPE retCode);
void main(void);
void ProcessMT_Master(void);
void Process1(void);
struct resume_error_type check_resume(PROCESS_ID_TYPE id, RETURN_CODE_TYPE code);
void show_result(struct resume_error_type r_e);
struct resume_error_type initStruct(void);


/* Global variables */
/* Next variable is only to use in WindRiver Platform ARINC 653 but don´t use them in Promela.*/
/* BOOL GV1; */

/* Definition processes	*/
PROCESS_ATTRIBUTE_TYPE processTable[] = {
	/* NAME ENTRY_POINT STACK_SIZE BASE_PRIORITY PERIOD TIME_CAPACITY DEADLINE */

	/* name,        entry,  stack, prio,    period, t capacity, deadline */
	{"tProcessMasterT", ProcessMT_Master, 4000, REGULAR_MASTER_PROCESS_PRIORITY, 0, TIME_4sec, SOFT},
	{"tProc1", Process1, 4000, REGULAR_P1_PROCESS_PRIORITY, 0, TIME_4sec, SOFT},
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
/* Entry point */
void main(void){
	PROCESS_ID_TYPE procMainIdMT;
	PROCESS_ID_TYPE procMainId1;
	RETURN_CODE_TYPE retCode;
	char sal_code[MAX_CAD];

	/* Next line is only to use in WindRiver Platform ARINC 653 but don´t use in Promela. It uses a global variable  */
	/* GV1 = FALSE; */

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

    START(procMainIdMT, &retCode);
	CHECK_CODE(": START Process Master Test", retCode, sal_code);
    printf("%s\n", sal_code);

	/* To set the partition in a Normal State */
	SET_PARTITION_MODE(NORMAL, &retCode);
	CHECK_CODE(": SET_PARTITION_MODE ", retCode, sal_code);
    printf("%s\n", sal_code);

}

/* -----------------------------------------------------------------------------------*/
struct resume_error_type initStruct(void){
	struct resume_error_type r_e;

	r_e.test_code_res = FALSE;
	r_e.test_state = FALSE;
	r_e.test_code_getstatus = FALSE;

	return r_e;
}

/* -----------------------------------------------------------------------------------*/
/*  1. Call RESUME service, using as argument the received value for the process ID.
	2. Check the output return code against the expected return code, received as argument.
	3. If service call gave no error, call service GET_PROCESS_STATUS, using as argument the
	   received value for the process ID.
	4. If call to GET_PROCESS_STATUS gave no error, check that process state is Ready.
	5. Return a bit structure returning any of the following errors:
		a. Expected error code different from the returned one.
		b. Process state different from ready.
		c. GET_PROCESS_STATUS invocation gave returned an error.
	6. Output the returned process status and the returned error code from service RESUME invocation. */

struct resume_error_type check_resume(PROCESS_ID_TYPE id, RETURN_CODE_TYPE code){
	struct resume_error_type r_e;
	RETURN_CODE_TYPE retCode;
	PROCESS_STATUS_TYPE status;

	r_e = initStruct();

	RESUME(id, &retCode);
	if (retCode == code){
		GET_PROCESS_STATUS(id, &status, &retCode);
		if (retCode == NO_ERROR){
			if (status.PROCESS_STATE != READY){
				r_e.test_state = TRUE;
			}
		}else
			r_e.test_code_getstatus = TRUE;
	}else{
		r_e.test_code_res = TRUE;
	}

	return r_e;
}

/* -----------------------------------------------------------------------------------*/
void show_result(struct resume_error_type r_e){
	if ((!r_e.test_code_res) && (r_e.test_state) && (!r_e.test_code_getstatus)){
		printf("RESUME test is successful \n");
	}else if (r_e.test_code_res){
		printf("Expected RESUME error code different from the returned one \n");
	}else if (r_e.test_state){
		printf("Process state different from ready \n");
	}else if (r_e.test_code_getstatus){
		printf("GET_PROCESS_STATUS invocation gave returned an error \n");
	}
}
/* -----------------------------------------------------------------------------------*/
void Process1(void){
	RETURN_CODE_TYPE retCode;
	char sal_code[MAX_CAD];

	printf("Process 1 starts ... \n");

	SUSPEND_SELF(HALF_REGULAR_TIME_WAIT, &retCode);
	CHECK_CODE(": SUSPEND_SELF ", retCode, sal_code);
    printf("%s\n", sal_code);

	/* Next line is only to use in WindRiver Platform ARINC 653 but don´t use in Promela. It uses a global variable  */
	/* GV1 = TRUE; */

	STOP_SELF();
}

/* -----------------------------------------------------------------------------------*/
void ProcessMT_Master(void){
	/* LOCK_LEVEL_TYPE lock; */
	struct resume_error_type r_e;
	RETURN_CODE_TYPE retCode;
	PARTITION_STATUS_TYPE statusp;
	PROCESS_STATUS_TYPE status;
	PROCESS_ID_TYPE procMainId1;
	char sal_code[MAX_CAD];

	printf("Process Master starts ...\n");

	/* Make sure the preemption is enabled */
	GET_PARTITION_STATUS(&statusp, &retCode);
	CHECK_CODE(": GET_PARTITION_STATUS in Master Test", retCode, sal_code);
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

	START(procMainId1 ,&retCode);
	CHECK_CODE(": START", retCode, sal_code);
    printf("%s\n", sal_code);

	/* Master_Test is pre-empted by P1 */

	GET_PROCESS_STATUS(procMainId1, &status, &retCode);
	CHECK_CODE(": GET_PROCESS_STATUS in Master Test", retCode, sal_code);
    printf("%s\n", sal_code);

	if (status.PROCESS_STATE!=WAITING){
		printf ("status error. The process Master test must be in WAITING %s \n", codeToStr(retCode));
	}

	r_e = check_resume(procMainId1, NO_ERROR);

	/* Next lines are only to use in WindRiver Platform ARINC 653 but don´t use in Promela. It uses a global variable  */
	/* if (!GV1){
		printf ("GV1 must be TRUE \n");
	} */

	show_result(r_e);

	STOP_SELF();
}
