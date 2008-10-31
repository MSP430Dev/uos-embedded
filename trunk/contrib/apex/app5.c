/* app5.c - Application number five which uses the APEX services.

Tested Services:
Service CREATE_PROCESS,
		GET_PROCESS_STATUS

Scenario:
  -Process Master_Test is created.
  -Process Proc1 is defined according to the following:
		Proc1.PROCESS_ATTRIBUTE_TYPE.NAME := "PROCESS_1"
		Proc1.PROCESS_ATTRIBUTE_TYPE.ENTRY_POINT:= (function pointer)
		Proc1.PROCESS_ATTRIBUTE_TYPE.STACK_SIZE:= VALID_STACK_SIZE
		Proc1.PROCESS_ATTRIBUTE_TYPE.BASE_PRIORITY:= REGULAR_MASTER_PROCESS_PRIORITY
		Proc1.PROCESS_ATTRIBUTE_TYPE.PERIOD:= VALID_PERIOD
		Proc1.PROCESS_ATTRIBUTE_TYPE.TIME_CAPACITY:= VALID_TIME_CAPACITY
		Proc1.PROCESS_ATTRIBUTE_TYPE.DEADLINE:= SOFT
  -Running during Master Partition initialization

Description:
Call procedure CHECKCREATE_PROCESS with the following arguments:
		Attributes of Process Proc1.
		NO_ERROR as expected error code.
		Check OutProcessStatus ID is DORMANT and OutProcessId is not null

Process created.

Call procedure CHECKCREATE_PROCESS with the following arguments:
		Attributes of Process Proc1.
		NO_ACTION as expected error code.

Expected Results:
  First call to procedure CHECKCREATE_PROCESS returns a CREATE_PROCESS structure confirming
  the process successful creation. Likewise, the +second call to the same procedure confirms
  that NO_ACTION has been taken by the CREATE_PROCESS service once a process with the same
  attributes has previously been created


----------------------------------------------------------------------------------------------------------------
OUTPUT from WindRiver Platform System ARINC 653 using prinf to show information by the standart output:
	Check successful: CREATE_PROCESS Process Master Test
	CREATE_PROCESS test is successful
	CREATE_PROCESS test is successful

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
#define REGULAR_MASTER_PROCESS_PRIORITY 62
#define MAX_CAD 50

/* TRUE =  I find a mistake
   FALSE = I don't find a mistake */
struct create_process_error_type{
	BOOL test_code;
	BOOL test_name;
	BOOL test_entry_p;
	BOOL test_stack_s;
	BOOL test_base_p;
	BOOL test_period;
	BOOL test_time_c;
	BOOL test_deadline;
};

/* Functions */
char * codeToStr (RETURN_CODE_TYPE retCode);
void main(void);
struct create_process_error_type checkcreate_process(PROCESS_ATTRIBUTE_TYPE a, PROCESS_ID_TYPE id, RETURN_CODE_TYPE retCode, RETURN_CODE_TYPE code);
struct create_process_error_type initCreate();
void show_result(struct create_process_error_type c_p_e);
void Process1(void);
void ProcessMT_Master(void);


/* Definition processes	*/
PROCESS_ATTRIBUTE_TYPE processTable[]=
{
	/* NAME ENTRY_POINT STACK_SIZE BASE_PRIORITY PERIOD TIME_CAPACITY DEADLINE */

	/* name,        entry,  stack, prio,    period, t capacity, deadline */
	{"tProcessMasterT", ProcessMT_Master, 4000, REGULAR_MASTER_PROCESS_PRIORITY+1, 0, TIME_4sec, SOFT},
	{"tProc1", Process1, 4000, REGULAR_MASTER_PROCESS_PRIORITY, 0, TIME_4sec, SOFT},
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
void Process1(void){
	int temp = 1;	/* This code is useful to avoid empty body in translations without the 'prinf' function */
	printf("Process 1 starts... \n");
}

/* -----------------------------------------------------------------------------------*/
void ProcessMT_Master(void){
	int temp = 1;	/* This code is useful to avoid empty body in translations without the 'prinf' function */
	printf("Main process starts ... \n");
}

/* -----------------------------------------------------------------------------------*/
struct create_process_error_type initCreate(){
	struct create_process_error_type c_p_e;

	c_p_e.test_code = FALSE;
	c_p_e.test_name = FALSE;
	c_p_e.test_entry_p = FALSE;
	c_p_e.test_stack_s = FALSE;
	c_p_e.test_base_p = FALSE;
	c_p_e.test_period = FALSE;
	c_p_e.test_time_c = FALSE;
	c_p_e.test_deadline = FALSE;

	return c_p_e;
}
/* -----------------------------------------------------------------------------------*/
/*
  1. Invokes the CREATE_PROCESS service, using the process attributes as input parameter.
  2. Checks the returned error code against the expected one.
  3. If the returned error code is ok, call the GET_PROCESS_STATUS service using the returned process
 	 parameter as input parameter.
  4. Check that the return code of the method is equal to NO_ERROR.
  5. If no error occurred, compare the attribute values returned by the method against the expected values
	 for them.
  6. Return the returned processId, the process status returned by the GET_PROCESS_STATUS service,
     the error code and one structure indicating the results of the comparisons between the expected values
     passed as parameters and the actual values, returned by the invoked services.
*/
struct create_process_error_type checkcreate_process(PROCESS_ATTRIBUTE_TYPE a, PROCESS_ID_TYPE id, RETURN_CODE_TYPE retCode, RETURN_CODE_TYPE code){
	struct create_process_error_type c_p_e;
	PROCESS_STATUS_TYPE status;
	char sal_code[MAX_CAD];

	c_p_e = initCreate();

	if (code == retCode){
		GET_PROCESS_STATUS(id, &status, &retCode);
		if (retCode == NO_ERROR){
			if (strcmp(a.NAME, status.ATTRIBUTES.NAME)!=0){
				c_p_e.test_name = TRUE;
			}else if (a.ENTRY_POINT != status.ATTRIBUTES.ENTRY_POINT){
				c_p_e.test_entry_p = TRUE;
			}else if (a.STACK_SIZE != status.ATTRIBUTES.STACK_SIZE){
				c_p_e.test_stack_s = TRUE;
			}else if (a.BASE_PRIORITY != status.ATTRIBUTES.BASE_PRIORITY){
				c_p_e.test_base_p = TRUE;
			}else if (a.PERIOD != status.ATTRIBUTES.PERIOD){
				c_p_e.test_period = TRUE;
			}else if (a.TIME_CAPACITY != status.ATTRIBUTES.TIME_CAPACITY){
				c_p_e.test_time_c = TRUE;
			}else if (a.DEADLINE != status.ATTRIBUTES.DEADLINE){
				c_p_e.test_deadline = TRUE;
			}
		}else{
			CHECK_CODE(": GET_PROCESS_STATUS in subfunction checkcreate_process", retCode, sal_code);
    		printf("%s\n", sal_code);
		}
	}else
		c_p_e.test_code = TRUE;

	return c_p_e;
}

/* -----------------------------------------------------------------------------------*/
void show_result(struct create_process_error_type c_p_s){
	if ((!c_p_s.test_name) && (!c_p_s.test_entry_p) && (!c_p_s.test_stack_s) &&
		(!c_p_s.test_base_p) && (!c_p_s.test_period) && (!c_p_s.test_time_c) &&
		(!c_p_s.test_deadline) && !c_p_s.test_code){
		printf("CREATE_PROCESS test is successful \n");
	}else if (c_p_s.test_name){
		printf("Problem with the process name, it's not the expected one \n");
	}else if (c_p_s.test_entry_p){
		printf("Problem with the entry point, it's not the expected one \n");
	}else if (c_p_s.test_stack_s){
		printf("Problem with the stack size, it's not the expected one \n");
	}else if (c_p_s.test_base_p){
		printf("Problem with base priority , it's not the expected one \n");
	}else if (c_p_s.test_period){
		printf("Problem with the process period, it's not the expected one \n");
	}else if (c_p_s.test_time_c){
		printf("Problem with the process time capacity, it's not the expected one \n");
	}else if (c_p_s.test_deadline){
		printf("Problem with the process deadline, it's not the expected one \n");
	}else if (c_p_s.test_code){
		printf("Problem with the CREATE_PROCESS return code, it's not the expected one \n");
	}

}

/* -----------------------------------------------------------------------------------*/
void main(void){
	struct create_process_error_type c_p_e;
	RETURN_CODE_TYPE retCode;
	PROCESS_ID_TYPE procMainIdMT;
	PROCESS_ID_TYPE procMainId1;
	char sal_code[MAX_CAD];

	/* Create Process Master Test */
	CREATE_PROCESS (&processTable[0],  /* process attribute */
                    &procMainIdMT,    /* process Id */
                    &retCode);

 	CHECK_CODE(": CREATE_PROCESS Process Master Test", retCode, sal_code);
    printf("%s\n", sal_code);

    /* Create Process 1 */
    CREATE_PROCESS (&processTable[1],  /* process attribute */
                    &procMainId1,    /* process Id */
                    &retCode);

	c_p_e = checkcreate_process(processTable[1], procMainId1, retCode, NO_ERROR);

	show_result(c_p_e);

	/* Create Process 1 again */
	CREATE_PROCESS (&processTable[1],  /* process attribute */
                    &procMainId1,    /* process Id */
                    &retCode);


	c_p_e = checkcreate_process(processTable[1], procMainId1, retCode, NO_ACTION);

	show_result(c_p_e);
}

/* -----------------------------------------------------------------------------------*/
