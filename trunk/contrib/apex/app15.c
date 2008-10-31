/* 	Aplication number fifteen which uses APEX services
Tested Services:
	REPLENISH,
	GET_PROCESS_STATUS


Scenario:
	Master Partition is in NORMAL state
	Process Master_Test is created and started in Master Partition with priority = REGULAR_ MASTER_PROCESS_PRIORITY
	and is aperiodic

Description:
Pseudo-code for Master_Test
	Call service GET_PROCESS_STATUS using as argument the ID of process Master_Test, to get the deadline time.

	Call procedure CHECKREPLENISH with the following arguments:
		BudgetTime = (deadline time - 1 - current system clock).
		NO_ERROR as expected error code.

	Call service GET_PROCESS_STATUS using as argument the ID of process Master_Test

	Test succeeds if procedure returned no errors and deadline time was decreased by one between the two calls to
	GET_PROCESS_STATUS

Expected Results:
	First call to the GET_PROCESS_STATUS service returns a PROCESS_STATUS_TYPE structure encapsulating the current
	process DEADLINE_TIME. The call to procedure CHECKREPLENISH updates the deadline removing 1 unit, which is confirmed
	by the final call to GET_PROCESS_STATUS.

-----------------------------------------------------------------------------------------------------------------------------
OUTPUT from WindRiver Platform System ARINC 653 using prinf to show information by the standart output:
	Check successful: CREATE_PROCESS Process Master Test
	Check successful: START Process Master Test
	Main Process starts...
	Check successful: GET_PROCESS_ID Process Master Test
	Check successful: GET_PROCESS_STATUS Status0
	Check successful: GET_TIME
	Check successful: GET_PROCESS_STATUS Status1
	REPLENISH test is successful

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
#define MAX_CAD 50

/* TRUE =  I find a mistake
   FALSE = I don't find a mistake */
struct replenish_error_type{
	BOOL test_code;
	BOOL test_status;
};

/* Functions */
char * codeToStr (RETURN_CODE_TYPE retCode);
void main(void);
void ProcessMT_Master(void);
struct replenish_error_type initStruct(void);
struct replenish_error_type checkreplenish(PROCESS_ID_TYPE id, SYSTEM_TIME_TYPE budget, RETURN_CODE_TYPE code);
void show_result(struct replenish_error_type r_e);


/* Definition processes	*/
PROCESS_ATTRIBUTE_TYPE processTable[]=
{
	/* NAME ENTRY_POINT STACK_SIZE BASE_PRIORITY PERIOD TIME_CAPACITY DEADLINE */

	/* name,        entry,  stack, prio,    period, t capacity, deadline */
	{"tProcessMasterT", ProcessMT_Master, 4000, REGULAR_MASTER_PROCESS_PRIORITY, 0, TIME_4sec, SOFT},
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
struct replenish_error_type initStruct(){
	struct replenish_error_type r_e;

	r_e.test_code = FALSE;
	r_e.test_status = FALSE;

	return r_e;
}

/* -----------------------------------------------------------------------------------*/
/*  1. Call service.
	2. Check whether the returned error code is different from the expected one.
	3. If no error occurred, call service GET_PROCESS_STATUS, using as argument the received process
	   ID. This process ID MUST be the process ID of the current process but this procedure has no way to
	   guarantee it!
	4. Return structure indicating whether the returned error code was different from the expected
	one.
*/
struct replenish_error_type checkreplenish(PROCESS_ID_TYPE id, SYSTEM_TIME_TYPE budget,
										   RETURN_CODE_TYPE code){
	struct replenish_error_type r_e;
	RETURN_CODE_TYPE retCode;
	PROCESS_STATUS_TYPE status0;

	r_e = initStruct();

	REPLENISH(budget, &retCode);
	if (retCode==code){
		GET_PROCESS_STATUS(id, &status0, &retCode);
		if (retCode!=NO_ERROR){
			r_e.test_status = TRUE;
		}
	}else
		r_e.test_code = TRUE;


	return r_e;
}
/* -----------------------------------------------------------------------------------*/
void show_result(struct replenish_error_type r_e){
	if ((!r_e.test_code) && (!r_e.test_status)){
		printf("REPLENISH test is successful \n");
	}else if (r_e.test_code){
		printf("The returned error code was different from the expected NO_ERROR \n");
	}else if (r_e.test_status){
		printf("Problem with GET_PROCESS_STATUS in checkreplenish \n");
	}
}

/* -----------------------------------------------------------------------------------*/
void ProcessMT_Master(void){
	struct replenish_error_type r_e;
	SYSTEM_TIME_TYPE s_t;
	SYSTEM_TIME_TYPE budg;
	PROCESS_ID_TYPE procMainIdMT;
	char sal_code[MAX_CAD];
	RETURN_CODE_TYPE retCode;
	PROCESS_STATUS_TYPE status0;
	PROCESS_STATUS_TYPE status1;

	printf("Main Process starts... \n");

	/* To get the procMainIdMT value */
	GET_PROCESS_ID("tProcessMasterT", &procMainIdMT, &retCode);
	CHECK_CODE(": GET_PROCESS_ID Process Master Test", retCode, sal_code);
    printf("%s\n", sal_code);

	GET_PROCESS_STATUS(procMainIdMT, &status0, &retCode);
	CHECK_CODE(": GET_PROCESS_STATUS Status0", retCode, sal_code);
    printf("%s\n", sal_code);

	GET_TIME(&s_t, &retCode);
	CHECK_CODE(": GET_TIME ", retCode, sal_code);
    printf("%s\n", sal_code);

	budg = status0.DEADLINE_TIME - 1 - s_t;

	r_e = checkreplenish(procMainIdMT, budg, NO_ERROR);

	GET_PROCESS_STATUS(procMainIdMT, &status1, &retCode);
	CHECK_CODE(": GET_PROCESS_STATUS Status1 ", retCode, sal_code);
    printf("%s\n", sal_code);

	if ((s_t + budg) != (status0.DEADLINE_TIME - 1)){
		printf("Deadline time was not decreased by one between the two calls to GET_PROCESS_STATUS \n");
		printf("BUDGET: %ld \n", budg);
		printf("status0: %ld \n", status0.DEADLINE_TIME);
		printf("status1: %ld \n", status1.DEADLINE_TIME);
		printf("current system clock: %ld \n", s_t);
	}

	show_result(r_e);

}

/* -----------------------------------------------------------------------------------*/
/* Entry point */
void main(void){
	RETURN_CODE_TYPE retCode;
	PROCESS_ID_TYPE procMainIdMT;
	char sal_code[MAX_CAD];

	CREATE_PROCESS (&(processTable [0]),       /* process attribute */
                    &procMainIdMT,             /* process Id */
                    &retCode);
	CHECK_CODE(": CREATE_PROCESS Process Master Test ", retCode, sal_code);
    printf("%s\n", sal_code);


	START(procMainIdMT, &retCode);
	CHECK_CODE(": START Process Master Test ", retCode, sal_code);
    printf("%s\n", sal_code);


	/* To set the partition in a Normal State */
	SET_PARTITION_MODE(NORMAL, &retCode);
	CHECK_CODE(": SET_PARTITION_MODE  ", retCode, sal_code);
    printf("%s\n", sal_code);

}
