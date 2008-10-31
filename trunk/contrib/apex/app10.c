/* 	Aplication number ten which uses APEX services
	Tested Services: (Blackboards may be used or not. It'll be indifferent, if we want to check the STOP service)

	STOP,
	START, GET_PARTITION_STATUS, STOP_SELF
	UNLOCK_PREEMPTION, LOCK_PREEMPTION, READ_BLACKBOARD, DISPLAY_BLACKBOARD, GET_BLACKBOARD_ID, haven´t been modeled yet

Scenario:
	Process Master_Test is created with base priority = REGULAR_MASTER_PROCESS_PRIORITY and is aperiodic
	Process P1 is created with base priority = REGULAR_P1_PROCESS_PRIORITY and is aperiodic
	Blackboard B1 is created and clear
	Blackboard B2 is created and clear
	Master Partition in NORMAL STATE

Description:
Pseudo-code for process Master_Test:
	Call service UNLOCK_PREEMPTION until the value it outputs for LOCK_LEVEL is equal to 0
	     (preemption is enabled).

	Call service START with the ID of process P1 as argument (process P1 pre-empts Master_Test)

	Call service READ_BLACKBOARD using the following arguments:
	ID of blackboard B1
	Timeout = 0 ms

	Call service READ_BLACKBOARD using the following arguments:
	ID of blackboard B2
	Timeout = 0 ms

	Test succeeded if read messages are all "OK", otherwise failed


Pseudo code for process P1
	Call service START with the ID of process P2 as argument

	Call service START with the ID of process P3 as argument (process P1 pre-empts process P3)


Pseudo code for process P3
	Call procedure CHECK_STOP with the following arguments:
		ID of process P2
		NO_ERROR as expected error code

	Call service DISPLAY_BLACKBOARD using as arguments the following values:
		ID of blackboard B1
		Message is "OK"

	Call service START with the value of process P2 as parameter



Pseudo code for process P2:
	Call service DISPLAY_BLACKBOARD using as arguments the following values:
		ID of blackboard B2
		Message is "OK"
		Size is 3 bytes


----------------------------------------------------------------------------------------------------------------
OUTPUT from WindRiver Platform System ARINC 653 using prinf and global variable (BLACKBOARD_NAME_0, BLACKBOARD_NAME_1) to show information
		by the standart output:
	Check successful: CREATE_PROCESS Process Master Test
	Check successful: CREATE_PROCESS Process 1
	Check successful: CREATE_PROCESS Process 2
	Check successful: CREATE_PROCESS Process 3
	Check successful: CREATE_BLACKBOARD blackboard1
	Check successful: CREAR_BLACKBOARD blackboard1
	Check successful: CREATE_BLACKBOARD blackboard2
	Check successful: CREAR_BLACKBOARD blackboard2
	Check successful: START Process Master Test
	Process Master starts ...
	Check successful: GET_PARTITION_STATUS
	Check successful: GET_PROCESS_ID Process 1
	Process 1 starts ...
	Check successful: GET_PROCESS_ID Process 2
	Check successful: START Process 2
	Check successful: GET_PROCESS_ID Process 3
	Check successful: START Process 3
	Process 3 starts ...
	Check successful: GET_PROCESS_ID Process 2
	Check successful: STOP Process with ID: 673587872
	Check successful: GET_BLACKBOARD_ID_0 by Process3
	Check successful: DISPLAY_BLACKBOARD B1 in Process 3
	Check successful: START Process 2
	STOP test is successful
	Process 2 starts ...
	Check successful: GET_BLACKBOARD_ID_1 by Process2
	Check successful: DISPLAY_BLACKBOARD B2 in Process 2
	Check successful: START Process 1
	Check successful: GET_BLACKBOARD_ID_0 by ProcessMT_Master
	Check successful: READ_BLACKBOARD B1 Read data: OK
	Check successful: GET_BLACKBOARD_ID_1 by ProcessMT_Master
	Check successful: READ_BLACKBOARD B2 Read data: OK

Sintax of output:
	Check successful: <ARINC653 Call>  			 -> This lines show us that the ARINC call returned NO_ERROR
	<RETURN_CODE_TYPE errori> : <ARINC653 Call>  -> In case that the call returned any error, this one is printing (typical RETURN_CODE_TYPE error)
	<ProcessN> starts ...              			 -> It is the first line on the top code to the process body
	Rest of printing 				   			 -> Some of them are only to debug and other depends on the Pedro´s model printing.

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
2nd Update : October 17th, 2007
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
#define REGULAR_P2_PROCESS_PRIORITY 21
#define REGULAR_P3_PROCESS_PRIORITY 30
#define NUM_BLACKBOARDS 2
#define MAX_LENGTH 10
#define MAX_CAD 50

/* typedefs */
typedef struct {
    BLACKBOARD_NAME_TYPE        name;
    MESSAGE_SIZE_TYPE           msgSize;
} BLACKBOARD_DEFINITION;

/* TRUE =  I find a mistake
   FALSE = I don't find a mistake */
struct stop_error_type{
	BOOL test_code_exp;
	BOOL test_state;
	BOOL test_code_get;
};

/* Functions */
char * codeToStr (RETURN_CODE_TYPE retCode);
void main(void);
void ProcessMT_Master(void);
void Process1(void);
void Process2(void);
void Process3(void);
struct stop_error_type initStruct(void);
struct stop_error_type check_stop(PROCESS_ID_TYPE id, RETURN_CODE_TYPE code);
void show_results(struct stop_error_type c_s);


/* Definition processes	*/
PROCESS_ATTRIBUTE_TYPE processTable[]=
{
	/* NAME ENTRY_POINT STACK_SIZE BASE_PRIORITY PERIOD TIME_CAPACITY DEADLINE */

	/* name,        entry,  stack, prio,    period, t capacity, deadline */
	{"tProcessMasterT", ProcessMT_Master, 4000, REGULAR_MASTER_PROCESS_PRIORITY, 0, TIME_4sec, SOFT},
	{"tProc1", Process1, 4000, REGULAR_P1_PROCESS_PRIORITY, 0, TIME_4sec, SOFT},
	{"tProc2", Process2, 4000, REGULAR_P2_PROCESS_PRIORITY, 0, TIME_4sec, SOFT},
	{"tProc3", Process3, 4000, REGULAR_P3_PROCESS_PRIORITY, 0, TIME_4sec, SOFT},
	/* Don't touch or move next line */
	{""         ,    NULL,      0,    0,          0,         0, SOFT}
};

/* Global variables */
/* blackboard Identifiers */
BLACKBOARD_NAME_TYPE BLACKBOARD_NAME_0;
BLACKBOARD_NAME_TYPE BLACKBOARD_NAME_1;

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
struct stop_error_type initStruct(void){
	struct stop_error_type c_s;

	c_s.test_code_exp = FALSE;
	c_s.test_state= FALSE;
	c_s.test_code_get= FALSE;

	return c_s;
}

/* -----------------------------------------------------------------------------------*/
/*
	1. Call STOP service, using as argument the received value for the process ID.
	2. Check the output return code against the expected return code, received as argument.
	3. If service call gave no error, call service GET_PROCESS_STATUS, using as argument the
	   received value for the process ID.
	4. If no error occurred, check that process state is equal to Dormant.
	5. Return a bit structure holding information about any of the following errors:
		a. Expected error code different from the output one.
		b. Process state different from Dormant.
		c. GET_PROCESS_STATUS invocation gave an error.
	6. Output the process status returned from the GET_PROCESS_STATUS service and the return code
	   returned from the STOP service.
*/
struct stop_error_type check_stop(PROCESS_ID_TYPE id, RETURN_CODE_TYPE code){
	struct stop_error_type c_s;
	RETURN_CODE_TYPE retCode;
	PROCESS_STATUS_TYPE status;
	char sal_code[MAX_CAD];

	c_s = initStruct();

	STOP(id, &retCode);
	CHECK_CODE(": STOP", retCode, sal_code);
    printf("%s Process with ID: %d\n", sal_code, id);

	if (code==retCode){
		GET_PROCESS_STATUS(id, &status, &retCode);
		if (retCode==NO_ERROR){
			if (status.PROCESS_STATE != DORMANT){
				c_s.test_state = TRUE;
			}
		}else
			c_s.test_code_get = TRUE;
	}else
		c_s.test_code_exp = TRUE;

	return c_s;
}


/* -----------------------------------------------------------------------------------*/
/* Entry point */
void main(void){
	int cnt;
	char sal_code[MAX_CAD];
	RETURN_CODE_TYPE retCode;
	PROCESS_ID_TYPE procMainIdMT;
	PROCESS_ID_TYPE procMainId1;
	PROCESS_ID_TYPE procMainId2;
	PROCESS_ID_TYPE procMainId3;

	/* Blackboard definitions and identifier */
	BLACKBOARD_DEFINITION 	blackboardDefinitions[NUM_BLACKBOARDS];
	BLACKBOARD_ID_TYPE  bbId[NUM_BLACKBOARDS];


	/* Process Master_Test is created */
	CREATE_PROCESS (&(processTable [0]),       /* process attribute */
                    &procMainIdMT,             /* process Id */
                    &retCode);
    CHECK_CODE(": CREATE_PROCESS Process Master Test", retCode, sal_code);
    printf("%s\n", sal_code);

	/* Process P1 is created */
	CREATE_PROCESS (&(processTable [1]),       /* process attribute */
                    &procMainId1,             /* process Id */
                    &retCode);
    CHECK_CODE(": CREATE_PROCESS Process 1", retCode, sal_code);
    printf("%s\n", sal_code);

	/* Process P2 is created*/
	CREATE_PROCESS (&(processTable [2]),       /* process attribute */
                    &procMainId2,             /* process Id */
                    &retCode);
    CHECK_CODE(": CREATE_PROCESS Process 2", retCode, sal_code);
    printf("%s\n", sal_code);

	/* Process P2 is created*/
	CREATE_PROCESS (&(processTable [3]),       /* process attribute */
                    &procMainId3,             /* process Id */
                    &retCode);
	CHECK_CODE(": CREATE_PROCESS Process 3", retCode, sal_code);
    printf("%s\n", sal_code);

    /* To init blackboards */
	strcpy (BLACKBOARD_NAME_0, "blackboard1");
    strcpy(blackboardDefinitions[0].name, BLACKBOARD_NAME_0);
	blackboardDefinitions[0].msgSize = 3;

	strcpy(BLACKBOARD_NAME_1, "blackboard2");
    strcpy(blackboardDefinitions[1].name, BLACKBOARD_NAME_1);
	blackboardDefinitions[1].msgSize = 3;


	/* Blackboard B1 and B2 are created and clear */
	for (cnt = 0; cnt < NUM_BLACKBOARDS; cnt++){
		bbId [cnt] = -1;
		retCode = -1;
	    CREATE_BLACKBOARD (blackboardDefinitions[cnt].name,
                       	   blackboardDefinitions[cnt].msgSize,
                           &bbId[cnt],
                       	   &retCode);
		CHECK_CODE(": CREATE_BLACKBOARD", retCode, sal_code);
    	printf("%s %s\n", sal_code, blackboardDefinitions [cnt].name);

		CLEAR_BLACKBOARD(bbId [cnt], &retCode);
		CHECK_CODE(": CREAR_BLACKBOARD", retCode, sal_code);
    	printf("%s %s\n", sal_code, blackboardDefinitions [cnt].name);
	}

	/* Start main process ... */
	START(procMainIdMT, &retCode);
	CHECK_CODE(": START Process Master Test", retCode, sal_code);
    printf("%s\n", sal_code);

	/* To set the partition in a Normal State */
	SET_PARTITION_MODE(NORMAL, &retCode);
	CHECK_CODE(": SET_PARTITION_MODE ", retCode, sal_code);
    printf("%s\n", sal_code);

	return;

}

/* -----------------------------------------------------------------------------------*/
void show_results(struct stop_error_type c_s){
	if ((!c_s.test_code_exp) && (!c_s.test_state) && (!c_s.test_code_get)){
		printf("STOP test is successful \n");
	}else if (c_s.test_code_exp){
		printf("Expected error code different from the output one \n");
	}else if (c_s.test_state){
		printf("Process state different from Dormant \n");
	}else if (c_s.test_code_get){
		printf("GET_PROCESS_STATUS invocation gave an error \n");
	}
}

/* -----------------------------------------------------------------------------------*/
void ProcessMT_Master(void){
	LOCK_LEVEL_TYPE lock;
	APEX_BYTE data[MAX_LENGTH];
	long int len;
	RETURN_CODE_TYPE retCode;
	PROCESS_ID_TYPE procMainId1;
	char sal_code[MAX_CAD];
	PARTITION_STATUS_TYPE statusp;
	BLACKBOARD_ID_TYPE bbId;

	printf("Process Master starts ...\n");

	GET_PARTITION_STATUS(&statusp, &retCode);
	CHECK_CODE(": GET_PARTITION_STATUS", retCode, sal_code);
	printf("%s\n", sal_code);

	/* Make sure the preemption is enabled, lock == 0 */
	if (statusp.LOCK_LEVEL>0){
		UNLOCK_PREEMPTION(&lock, &retCode);
		while (lock!=0){
			UNLOCK_PREEMPTION(&lock, &retCode);
		}
	}else if (statusp.LOCK_LEVEL<0){
		LOCK_PREEMPTION(&lock, &retCode);
		while (lock!=0){
			LOCK_PREEMPTION(&lock, &retCode);
		}
	}

	/* To get the procMainId1 value */
	GET_PROCESS_ID("tProc1", &procMainId1, &retCode);
	CHECK_CODE(": GET_PROCESS_ID Process 1", retCode, sal_code);
    printf("%s\n", sal_code);

	START(procMainId1 ,&retCode);
	CHECK_CODE(": START Process 1 ", retCode, sal_code);
    printf("%s\n", sal_code);

	/* Process P1 pre-empts Master_Test */

	/* Call service READ_BLACKBOARD B1 */
	GET_BLACKBOARD_ID(BLACKBOARD_NAME_0, &bbId, &retCode);
	CHECK_CODE(": GET_BLACKBOARD_ID_0 by ProcessMT_Master", retCode, sal_code);
	printf("%s\n", sal_code);

	READ_BLACKBOARD (bbId, 0, data, &len, &retCode);
	CHECK_CODE(": READ_BLACKBOARD B1", retCode, sal_code);
    printf("%s Read data: %s\n", sal_code, data);

	/* Call service READ_BLACKBOARD B2 */
	GET_BLACKBOARD_ID(BLACKBOARD_NAME_1, &bbId, &retCode);
	CHECK_CODE(": GET_BLACKBOARD_ID_1 by ProcessMT_Master", retCode, sal_code);
	printf("%s\n", sal_code);

	READ_BLACKBOARD (bbId, 0, data, &len, &retCode);
	CHECK_CODE(": READ_BLACKBOARD B2", retCode, sal_code);
	printf("%s Read data: %s\n", sal_code, data);

	STOP_SELF();
}

/* -----------------------------------------------------------------------------------*/
void Process1(void){
	char sal_code[MAX_CAD];
	RETURN_CODE_TYPE retCode;
	PROCESS_ID_TYPE procMainId2;
	PROCESS_ID_TYPE procMainId3;

	printf("Process 1 starts ... \n");

	/* To get the procMainId2 value */
	GET_PROCESS_ID("tProc2", &procMainId2, &retCode);
	CHECK_CODE(": GET_PROCESS_ID Process 2", retCode, sal_code);
    printf("%s\n", sal_code);

	START(procMainId2, &retCode);
	CHECK_CODE(": START Process 2", retCode, sal_code);
    printf("%s\n", sal_code);

	/* To get the procMainId3 value */
	GET_PROCESS_ID("tProc3", &procMainId3, &retCode);
	CHECK_CODE(": GET_PROCESS_ID Process 3", retCode, sal_code);
    printf("%s\n", sal_code);

	START(procMainId3, &retCode);
	CHECK_CODE(": START Process 3", retCode, sal_code);
    printf("%s\n", sal_code);

	/* Process P1 pre-empts Process P3 */

	STOP_SELF();

}

/* -----------------------------------------------------------------------------------*/
void Process2(void){
	APEX_BYTE data[MAX_LENGTH];
	RETURN_CODE_TYPE retCode;
	char sal_code[MAX_CAD];
	BLACKBOARD_ID_TYPE bbId;

	printf("Process 2 starts ... \n");

	strcpy(data, "OK");

	GET_BLACKBOARD_ID(BLACKBOARD_NAME_1, &bbId, &retCode);
	CHECK_CODE(": GET_BLACKBOARD_ID_1 by Process2", retCode, sal_code);
	printf("%s\n", sal_code);

	DISPLAY_BLACKBOARD (bbId, data, 3, &retCode);
	CHECK_CODE(": DISPLAY_BLACKBOARD B2 in Process 2", retCode, sal_code);
    printf("%s\n", sal_code);

	STOP_SELF();
}

/* -----------------------------------------------------------------------------------*/
void Process3(void){
	struct stop_error_type c_s;
	APEX_BYTE data[MAX_LENGTH];
	char sal_code[MAX_CAD];
	PROCESS_ID_TYPE procMainId2;
	RETURN_CODE_TYPE retCode;
	BLACKBOARD_ID_TYPE bbId;

	printf("Process 3 starts ... \n");

	strcpy(data, "OK");

	/* To get the procMainId2 value */
	GET_PROCESS_ID("tProc2", &procMainId2, &retCode);
	CHECK_CODE(": GET_PROCESS_ID Process 2", retCode, sal_code);
    printf("%s\n", sal_code);

	/* To check stop process 2 */
	c_s = check_stop(procMainId2, NO_ERROR);

	GET_BLACKBOARD_ID(BLACKBOARD_NAME_0, &bbId, &retCode);
	CHECK_CODE(": GET_BLACKBOARD_ID_0 by Process3", retCode, sal_code);
	printf("%s\n", sal_code);

	DISPLAY_BLACKBOARD (bbId, data, 3, &retCode);
	CHECK_CODE(": DISPLAY_BLACKBOARD B1 in Process 3", retCode, sal_code);
    printf("%s\n", sal_code);

	START(procMainId2, &retCode);
	CHECK_CODE(": START Process 2", retCode, sal_code);
    printf("%s\n", sal_code);

	show_results(c_s);

    STOP_SELF ();
}
