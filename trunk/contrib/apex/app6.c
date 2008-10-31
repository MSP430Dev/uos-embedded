/*
Aplication number six which uses APEX services

Tested Services
  SET_PRIORITY
  START, GET_PROCESS_STATUS, STOP_SELF, GET_PARTITION_STATUS, GET_PROCESS_ID
  UNLOCK_PREEMPTION, LOCK_PREEMPTION -> LOCK_PREEMPTION, UNLOCK_PREEMPTION haven´t been modeled yet

Scenario:
  Process Master_Test is created and started with base priority = REGULAR_MASTER_PROCESS_PRIORITY and is  aperiodic
  Process P1 is created with base priority = REGULAR_P1_PROCESS_PRIORITY and is aperiodic
  Process P2 is created with base priority = REGULAR_P2_PROCESS_PRIORITY and is aperiodic
  Master Partition in NORMAL STATE

Description:
  Pseudo-code for process Master_Test:
	Call service UNLOCK_PREEMPTION until the value outputs for LOCK_LEVEL is equal to 0 (preemption
	is enabled).

	Call service START with the ID of process P1 as argument (P1 will pre-empt this process)

  Pseudo-code for process P1:
	Call service START with the ID of process P2 as argument

	Call service GET_PROCESS_STATUS using as argument the ID of process P2

	Call procedure CHECKSET_PRIORITY with the following arguments:
		ID of Process P1.
		REGULAR_P2_PROCESS_PRIORITY - 1 as new priority value.
		NO_ERROR as expected error code. (P1 will be pre-empted by P2)

	Check that procedure call didn't return any error.

	Set GV2 to TRUE if GET_PROCESS_STATUS returned READY and procedure returned no errors and GV1 is TRUE

  Pseudo-code for process P2:
	Set GV1 to TRUE

Expected Results:
  Call to service SET_PRIORITY returns NO_ERROR
  Process P2 pre-empts P1 after P1 calls the service SET_PRIORITY
  GV1 and GV2 are TRUE (process scheduling asked)

Note:
  GV1 and GV2 will be used to verify the code in WindRiver Platform ARINC 653, but we don´t use it in SPIN 4.0

----------------------------------------------------------------------------------------------------------------
OUTPUT from WindRiver Platform System ARINC 653 using prinf and global variables (GV1, GV2) to show information by the standart output:
	Check successful: CREATE_PROCESS Process Master Test
	Check successful: CREATE_PROCESS Process 1
	Check successful: CREATE_PROCESS Process 2
	Check successful: START Process Master Test
	Process Master starts ...
	Check successful: GET_PARTITION_STATUS
	Check successful: GET_PROCESS_ID Process 1
	Process 1 starts ...
	Check successful: GET_PROCESS_ID Process 1
	Check successful: GET_PROCESS_ID Process 2
	Check successful: START Process 2
	Check successful: GET_PROCESS_STATUS Process 2
	Process 2 starts ...
	SET_PRIORITY test is successful
	Check successful: START Process 1
	The process scheduling has been successful


OUTPUT from WindRiver Platform System ARINC 653 using prinf (no any global variables) to show information by the standart output:
	Check successful: CREATE_PROCESS Process Master Test
	Check successful: CREATE_PROCESS Process 1
	Check successful: CREATE_PROCESS Process 2
	Check successful: START Process Master Test
	Process Master starts ...
	Check successful: GET_PARTITION_STATUS
	Check successful: GET_PROCESS_ID Process 1
	Process 1 starts ...
	Check successful: GET_PROCESS_ID Process 1
	Check successful: GET_PROCESS_ID Process 2
	Check successful: START Process 2
	Check successful: GET_PROCESS_STATUS Process 2
	Process 2 starts ...
	SET_PRIORITY test is successful
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
#define REGULAR_MASTER_PROCESS_PRIORITY 2
#define REGULAR_P1_PROCESS_PRIORITY 42
#define REGULAR_P2_PROCESS_PRIORITY 21
#define MAX_CAD 50


/* TRUE =  I find a mistake
   FALSE = I don't find a mistake */
struct set_priority_error_type{
	BOOL test_code_prio;			/*  the error code from SET_PRIORITY */
	BOOL test_prio;					/*	the current priority  */
	BOOL test_get_status;			/*  the error code from GET_PROCESS_STATUS  */
};

/* Functions */
struct set_priority_error_type checkset_priority(PROCESS_ID_TYPE id, PRIORITY_TYPE prior, RETURN_CODE_TYPE code);
char * codeToStr (RETURN_CODE_TYPE retCode);
void main(void);
struct set_priority_error_type initStruct();
void show_results(struct set_priority_error_type s_p_e);
BOOL no_error(struct set_priority_error_type s_p_e);
void printState(PROCESS_STATE_TYPE p, char* np);
void ProcessMT_Master(void);
void Process1(void);
void Process2(void);

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

/* Global variables */
/* Next two lines is only to use in WindRiver Platform ARINC 653 but don´t use in Promela. They are global variables  */
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
/* Task of process Master Test*/
/* LOCK_PREEMPTION, UNLOCK_PREEMPTION don´t belong to model calls  */
void ProcessMT_Master(void){
	/* LOCK_LEVEL_TYPE lock; */
	RETURN_CODE_TYPE retCode;
	PARTITION_STATUS_TYPE statusp;
	char sal_code[MAX_CAD];
	PROCESS_ID_TYPE procMainId1;

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

	/* Start process 1*/
	START (procMainId1, &retCode);
	CHECK_CODE(": START Process 1", retCode, sal_code);
    printf("%s\n", sal_code);


	/* P1 must pre-empt this process */

	/* Next 'if/else' is only to use in WindRiver Platform ARINC 653 but don´t use in Promela. It uses global variables  */
	/* Check values of GV1 and GV2*/
	/* if (GV1 && GV2)
		printf("The process scheduling has been successful");
	else if (!GV1){
		printf("Problem with the scheduling service, gv1 = false");
	}else if (!GV2){
		printf("Problem with the scheduling service, gv2 = false");
	} */

}

/* -----------------------------------------------------------------------------------*/
BOOL no_error(struct set_priority_error_type s_p_e){
	return ((!s_p_e.test_code_prio) && (!s_p_e.test_prio) && (!s_p_e.test_get_status));
}
/* -----------------------------------------------------------------------------------*/
/* Show errors from the struct checkset_priority */
void show_results(struct set_priority_error_type s_p_e){
	if (no_error(s_p_e)){
		printf("SET_PRIORITY test is successful \n");
	}else if (s_p_e.test_code_prio){
		printf("Problem with the error code from SET_PRIORITY, it's not the expected one \n");
	}else if (s_p_e.test_prio){
		printf("Problem with the current priority of the process 1, it's not the expected one \n");
	}else if (s_p_e.test_get_status){
		printf("Problem with the error code from GET_PROCESS_STATUS, it's not the expected one \n");
	}
}

/* -----------------------------------------------------------------------------------*/
/* Entry point of process 2 */
void Process2(void){
	printf("Process 2 starts ...\n");

	/* Next line is only to use in WindRiver Platform ARINC 653 but don´t use in Promela. It uses global variables  */
	/* GV1 = TRUE; */

	STOP_SELF();
}

/* -----------------------------------------------------------------------------------*/
void printState(PROCESS_STATE_TYPE p, char* np){
	if (p == DORMANT){
		printf("THE %s is DORMANT \n", np);
	}else if (p == READY){
		printf("THE %s is READY \n", np);
	}else if (p == RUNNING){
		printf("THE %s is RUNNING \n", np);
	}else if (p == WAITING){
		printf("THE %s is WAITING \n", np);
	}
}

/* -----------------------------------------------------------------------------------*/
/* Entry point of process 1 */
void Process1(void){
	struct set_priority_error_type s_p_e;
	PROCESS_STATUS_TYPE statusp2;
	RETURN_CODE_TYPE retCode;
	char sal_code[MAX_CAD];
	PROCESS_ID_TYPE procMainId2;
	PROCESS_ID_TYPE procMainId1;

	printf("Process 1 starts ...\n");

	/* To get the procMainId1 value */
	GET_PROCESS_ID("tProc1", &procMainId1, &retCode);
	CHECK_CODE(": GET_PROCESS_ID Process 1", retCode, sal_code);
    printf("%s\n", sal_code);

	/* To get the procMainId2 value */
	GET_PROCESS_ID("tProc2", &procMainId2, &retCode);
	CHECK_CODE(": GET_PROCESS_ID Process 2", retCode, sal_code);
    printf("%s\n", sal_code);

	/* Start process 2*/
	START (procMainId2, &retCode);
	CHECK_CODE(": START Process 2", retCode, sal_code);
    printf("%s\n", sal_code);

	GET_PROCESS_STATUS(procMainId2, &statusp2, &retCode);
	CHECK_CODE(": GET_PROCESS_STATUS Process 2", retCode, sal_code);
    printf("%s\n", sal_code);

	s_p_e = checkset_priority(procMainId1, REGULAR_P2_PROCESS_PRIORITY - 1, NO_ERROR);

	/* P1 will be pre-empted by P2 */

	show_results(s_p_e);

	if (statusp2.PROCESS_STATE != READY){
		printState(statusp2.PROCESS_STATE, "Process 2");
	}

	/* Next code is only to use in WindRiver Platform ARINC 653 but don´t use in Promela. It uses global variables  */
	/* if ((GV1 = TRUE) && (statusp2.PROCESS_STATE == READY) && (no_error(s_p_e))){
		GV2 = TRUE;
	}else if (!GV1){
		printf(" GV1 is FALSE");
	} */
	/* ///////////////////////////////////////////////////////////////////////////////////////////////////////////// */

	STOP_SELF();
}
/* -----------------------------------------------------------------------------------*/
/* To init to the struct used to check errors */
struct set_priority_error_type initStruct(){
	struct set_priority_error_type s_p_e;

	s_p_e.test_code_prio = FALSE;
	s_p_e.test_prio = FALSE;
	s_p_e.test_get_status = FALSE;

	return s_p_e;

}

/* -----------------------------------------------------------------------------------*/
/*  1. Call the SET_PRIORITY service, using as argument the received value for the process ID.
	2. Check the return code and compare it with the expected return code, passed as parameter.
	3. If service call gave no error, call service GET_PROCESS_STATUS with process ID as parameter.
	4. Compare the returned current process priority against the expected process priority, received
	   an input parameter.
	5. Return the error code resulting from the call to SET_PRIORITY, the PROCESS_STATUS resulting from the
       call to the GET_PROCESS_STATUS service and a structure with three bit fields indicating possible
	   errors in: the error code from SET_PRIORITY, the current priority, the error code from GET_PROCESS_STATUS */

struct set_priority_error_type checkset_priority(PROCESS_ID_TYPE id, PRIORITY_TYPE prior, RETURN_CODE_TYPE code) {
	RETURN_CODE_TYPE retCode;
	struct set_priority_error_type s_p_e;
	PROCESS_STATUS_TYPE status;

	s_p_e = initStruct();

	SET_PRIORITY(id, prior, &retCode);
	if (code == retCode){
		GET_PROCESS_STATUS(id, &status, &retCode);
		if (code != retCode){
			s_p_e.test_get_status = TRUE;
		}
		if (prior != status.CURRENT_PRIORITY){
			s_p_e.test_prio = TRUE;
		}
	}else
		s_p_e.test_code_prio = TRUE;

	return s_p_e;
}

/* -----------------------------------------------------------------------------------*/
void main(void){
	char sal_code[MAX_CAD];
	PROCESS_ID_TYPE procMainIdMT;
	PROCESS_ID_TYPE procMainId1;
  	PROCESS_ID_TYPE procMainId2;
    RETURN_CODE_TYPE retCode;

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

	CREATE_PROCESS (&processTable[2],  /* process attribute */
                    &procMainId2,    /* process Id */
                    &retCode);
	CHECK_CODE(": CREATE_PROCESS Process 2", retCode, sal_code);
    printf("%s\n", sal_code);

    /* start the processes */
    START (procMainIdMT, &retCode);
    CHECK_CODE(": START Process Master Test ", retCode, sal_code);
    printf("%s\n", sal_code);

    /* Next code is only to use in WindRiver Platform ARINC 653 but don´t use in Promela. It uses global variables  */
    /* GV1 = FALSE;
	GV2 = FALSE; */
	/* //////////////////////////////////////////////////////////////////////////////////////////////////////////// */

	SET_PARTITION_MODE(NORMAL, &retCode);
	CHECK_CODE(": SET_PARTITION_MODE ", retCode, sal_code);
    printf("%s\n", sal_code);

}
