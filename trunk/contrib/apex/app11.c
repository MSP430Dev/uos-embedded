/* 	Aplication number eleven which uses APEX services

Tested Services:
	START
	GET_PROCESS_STATUS
	UNLOCK_PREEMPTION, LOCK_PREEMPTION haven´t been modeled yet

Scenario:
	Process Master_Test is created with base priority = REGULAR_MASTER_PROCESS_PRIORITY and is aperiodic
	Process P1 is created with base priority = REGULAR_P2_PROCESS_PRIORITY and is aperiodic
	Master Partition in NORMAL STATE
	Global variable GV1 is initialized to FALSE

Description:
Pseudo-code for process Master_Test:
	Call service UNLOCK_PREEMPTION until the LOCK_VALUE returned by the service is 0 (preemption is enabled)

	Call procedure CHECK_START, using as arguments:
		The ID of process P1
		Expected error code is NO_ERROR

	Check that procedure returned no errors (Master_Test is pre-empted by P1)

	Test succeeded if procedure returned no errors and GV1 is TRUE



Pseudo-code for process P1
	Set GV1 to TRUE

	Call service STOP_SELF


Expected Results:
	Procedure P1 is started and there is no errors raised
	All processes call service STOP_SELF at the end of its execution code

-----------------------------------------------------------------------------------------------------------------------------
OUTPUT from WindRiver Platform System ARINC 653 using prinf and global variable (GV1) to show information by the standart output:
	Check successful: CREATE_PROCESS Process Master Test
	Check successful: CREATE_PROCESS Process 1
	Check successful: START Process Master Test
	Process Master starts ...
	Check successful: GET_PARTITION_STATUS
	Check successful: GET_PROCESS_ID Process 1
	Process 1 starts ...
	Check successful: START Process 1 by Process Master
	START test is successful

OUTPUT from WindRiver Platform System ARINC 653 using prinf (no any global variable) to show information by the standart output:
	Check successful: CREATE_PROCESS Process Master Test
	Check successful: CREATE_PROCESS Process 1
	Check successful: START Process Master Test
	Process Master starts ...
	Check successful: GET_PARTITION_STATUS
	Check successful: GET_PROCESS_ID Process 1
	Process 1 starts ...
	Check successful: START Process 1 by Process Master
	START test is successful

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
#define REGULAR_P2_PROCESS_PRIORITY 21
#define MAX_CAD 50


/* TRUE =  I find a mistake
   FALSE = I don't find a mistake */
struct start_error_type{
	BOOL test_code_exp;
	BOOL test_state;
	BOOL test_get_status;
};

/* Functions */
char * codeToStr (RETURN_CODE_TYPE retCode);
void main(void);
void ProcessMT_Master(void);
void Process1(void);
struct start_error_type initStruct();
struct start_error_type check_start(PROCESS_ID_TYPE id, RETURN_CODE_TYPE code);
void show_results(struct start_error_type s_e);


/* Definition processes	*/
PROCESS_ATTRIBUTE_TYPE processTable[]=
{
	/* NAME ENTRY_POINT STACK_SIZE BASE_PRIORITY PERIOD TIME_CAPACITY DEADLINE */
	/* name,        entry,  stack, prio,    period, t capacity, deadline */
	{"tProcessMasterT", ProcessMT_Master, 4000, REGULAR_MASTER_PROCESS_PRIORITY, 0, TIME_4sec, SOFT},
	{"tProc1", Process1, 4000, REGULAR_P2_PROCESS_PRIORITY, 0, TIME_4sec, SOFT},
	/* Don't touch or move next line */
	{""         ,    NULL,      0,    0,          0,         0, SOFT}
};

/* Global variables */
/* Next variable is only to use in WindRiver Platform ARINC 653 but don´t use them in Promela.*/
/* BOOL GV1; */

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
struct start_error_type initStruct(){
	struct start_error_type s_e_t;

	s_e_t.test_code_exp = FALSE;
	s_e_t.test_state = FALSE;
	s_e_t.test_get_status = FALSE;

	return s_e_t;
}

/* -----------------------------------------------------------------------------------*/
/*	1. Call START service, using as argument the received value for the process ID.
	2. Check whether the output return code equals the expected one, received as argument.
	3. If no error occurred in service call, call service GET_PROCESS_STATUS, using as argument the
		received value for the process ID.
	4. If no error occurred in service call, check that process state is equal to dormant.
	5. Return a bit structure holding information about the possible occurrence of any of the following errors:
		a. Expected error code different from the returned one.
		b. Process state different from dormant.
		c. Error in GET_PROCESS_STATUS service invocation.
	6. Output the PROCESS_STATUS structure, resulting from the call to the GET_PROCESS_STATUS service. */

struct start_error_type check_start(PROCESS_ID_TYPE id, RETURN_CODE_TYPE code){
	struct start_error_type s_e;
	PROCESS_STATUS_TYPE status;
	RETURN_CODE_TYPE retCode;
	char sal_code[MAX_CAD];

	s_e = initStruct();

	START(id, &retCode);
	if (code==retCode){
		if (retCode==NO_ERROR){
			GET_PROCESS_STATUS(id, &status, &retCode);
			if (retCode == NO_ERROR){
				if (status.PROCESS_STATE != DORMANT){
					s_e.test_state = TRUE;
				}
			}else
				s_e.test_get_status = TRUE;
		}else{
		    CHECK_CODE(": TIMED_WAIT ", retCode, sal_code);
    		printf("%s\n", sal_code);
		}
	}else
		s_e.test_code_exp = TRUE;

	return s_e;
}

/* -----------------------------------------------------------------------------------*/
	/* Set GV1 to TRUE if CURRENT_PRIORITY is the BASE_PRIORITY
	Call service STOP_SELF  */
void Process1(void){
	printf("Process 1 starts ... \n");

	/* Next line is only to use in WindRiver Platform ARINC 653 but don´t use in Promela. It uses a global variable  */
	/* GV1=TRUE; */

	STOP_SELF();
}

/* -----------------------------------------------------------------------------------*/
void show_results(struct start_error_type s_e){
	/* Next line is only to use in WindRiver Platform ARINC 653 but don´t use them in Promela.*/
	/* if ((GV1) && (!s_e.test_state) && (!s_e.test_get_status) && (!s_e.test_code_exp)){ */
	/* If you want to use SPIN, please use next code line  */
	if ((!s_e.test_state) && (!s_e.test_get_status) && (!s_e.test_code_exp)){
		printf("START test is successful \n");
	}else if (s_e.test_state){
		printf("Process state different from dormant \n");
	}else if (s_e.test_get_status){
		printf("Error in GET_PROCESS_STATUS service invocation \n");
	}else if (s_e.test_code_exp){
		printf("Expected error code different from the returned one NO_ERROR \n");
	}
}

/* -----------------------------------------------------------------------------------*/
void ProcessMT_Master(void){
	struct start_error_type s_e;
	PROCESS_ID_TYPE procMainId1;
	RETURN_CODE_TYPE retCode;
	char sal_code[MAX_CAD];
	PARTITION_STATUS_TYPE statusp;
	/* LOCK_LEVEL_TYPE lock; 	*/

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
	} */

	/* To get the procMainId1 value */
	GET_PROCESS_ID("tProc1", &procMainId1, &retCode);
	CHECK_CODE(": GET_PROCESS_ID Process 1", retCode, sal_code);
    printf("%s\n", sal_code);

	s_e =  check_start(procMainId1, NO_ERROR);
	CHECK_CODE(": START Process 1 by Process Master", retCode, sal_code);
    printf("%s\n", sal_code);

	/* Master_Test is pre-empted by P1 */

	show_results(s_e);

	STOP_SELF();
}

/* -----------------------------------------------------------------------------------*/
/* Entry point */
void main(void){
	/* Next declaration is only to use in WindRiver Platform ARINC 653 but don´t use in Promela. It uses a global variable  */
	/* BOOL GV1; */

	PROCESS_ID_TYPE procMainIdMT;
	PROCESS_ID_TYPE procMainId1;
	RETURN_CODE_TYPE retCode;
	char sal_code[MAX_CAD];

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

    /* Next line is only to use in WindRiver Platform ARINC 653 but don´t use in Promela. It uses a global variable  */
	/* GV1=FALSE;  */

	/* Start main process ... */
	START(procMainIdMT, &retCode);
	CHECK_CODE(": START Process Master Test", retCode, sal_code);
    printf("%s\n", sal_code);

	/* To set the partition in a Normal State */
	SET_PARTITION_MODE(NORMAL, &retCode);
	CHECK_CODE(": SET_PARTITION_MODE ", retCode, sal_code);
    printf("%s\n", sal_code);

}
