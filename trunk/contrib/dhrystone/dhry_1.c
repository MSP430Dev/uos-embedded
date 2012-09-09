/*
 ****************************************************************************
 *
 *                   "DHRYSTONE" Benchmark Program
 *                   -----------------------------
 *                                                                            
 *  Version:    C, Version 2.1
 *                                                                            
 *  File:       dhry_1.c (part 2 of 3)
 *
 *  Date:       May 25, 1988
 *
 *  Author:     Reinhold P. Weicker
 *
 ****************************************************************************
 */

#include <runtime/lib.h>
#include <kernel/uos.h>
#include <timer/timer.h>
#include "dhry.h"

/* Global Variables: */

Rec_Pointer     Ptr_Glob,
                Next_Ptr_Glob;
int             Int_Glob;
Boolean         Bool_Glob;
char            Ch_1_Glob,
                Ch_2_Glob;
int             Arr_1_Glob [50];
int             Arr_2_Glob [50] [50];


extern timer_t timer;

#ifndef NO_PROTOTYPES
extern char     *malloc ();
#endif
  /* forward declaration necessary since Enumeration may not simply be int */

#ifndef REG
        Boolean Reg = false;
#define REG
        /* REG becomes defined as empty */
        /* i.e. no register variables   */
#else
        Boolean Reg = true;
#endif

/* variables for time measurement: */

#ifdef TIMES
struct tms      time_info;
#ifndef NO_PROTOTYPES
extern  int     times ();
                /* see library function "times" */
#endif
#define Too_Small_Time 120
                /* Measurements should last at least about 2 seconds */
#endif
#ifdef TIME
#ifndef NO_PROTOTYPES
extern long     time();
                /* see library function "time"  */
#endif
#define Too_Small_Time 2
                /* Measurements should last at least 2 seconds */
#endif
#ifdef CORETIME
#define Too_Small_Time 10
#endif

long            Begin_Time,
                End_Time,
                User_Time;
float           Microseconds,
                Dhrystones_Per_Second;

/* end of variables for time measurement */

static Rec_Type npg;
static Rec_Type pg;

//void main_dhr(int argc, char *argv[])
void main_dhr(int argc, char * argv[])
/*****/

  /* main program, corresponds to procedures        */
  /* Main and Proc_0 in the Ada version             */
{

        One_Fifty       Int_1_Loc = 0;
  REG   One_Fifty       Int_2_Loc = 0;
        One_Fifty       Int_3_Loc;
  REG   char            Ch_Index;
        Enumeration     Enum_Loc;
        Str_30          Str_1_Loc;
        Str_30          Str_2_Loc;
  REG   int             Run_Index;
  REG   int             Number_Of_Runs;

  /* Initializations */

//  Next_Ptr_Glob = (Rec_Pointer) malloc (sizeof (Rec_Type));
//  Ptr_Glob = (Rec_Pointer) malloc (sizeof (Rec_Type));

  Next_Ptr_Glob = &npg;
  Ptr_Glob = &pg;

  Ptr_Glob->Ptr_Comp                    = Next_Ptr_Glob;
  Ptr_Glob->Discr                       = Ident_1;
  Ptr_Glob->variant.var_1.Enum_Comp     = Ident_3;
  Ptr_Glob->variant.var_1.Int_Comp      = 40;
  strcpy ((unsigned char *)Ptr_Glob->variant.var_1.Str_Comp, 
          (const unsigned char *)("DHRYSTONE PROGRAM, SOME STRING"));
  strcpy ((unsigned char *)Str_1_Loc, (const unsigned char *)("DHRYSTONE PROGRAM, 1'ST STRING"));

  Arr_2_Glob [8][7] = 10;
        /* Was missing in published program. Without this statement,    */
        /* Arr_2_Glob [8][7] would have an undefined value.             */
        /* Warning: With 16-Bit processors and Number_Of_Runs > 32000,  */
        /* overflow may occur for this array element.                   */

  Number_Of_Runs = 0;
  if ( argc == 2 ) {
      if (atoi((const unsigned char *)argv[1]) > 0) {
          Number_Of_Runs = atoi((const unsigned char *)argv[1]);
      }
  }

  debug_printf("\n");
  debug_printf("Dhrystone Benchmark, Version 2.1 (Language: C)\n");
  debug_printf("\n");
  if (Reg)
  {
    debug_printf("Program compiled with 'register' attribute\n");
    debug_printf("\n");
  }
  else
  {
    debug_printf("Program compiled without 'register' attribute\n");
    debug_printf("\n");
  }

  if (!Number_Of_Runs) {
      debug_printf("Please give the number of runs through the benchmark: ");
  //    fflush (stdout);
      {
          int n = 50000;
 //         scanf ("%d", &n);
          Number_Of_Runs = n; // number of iterations 10000
      }
      debug_printf("\n");
  }

  debug_printf("Execution starts, %d runs through Dhrystone\n", Number_Of_Runs);

  /***************/
  /* Start timer */
  /***************/
 
#ifdef TIMES
  times (&time_info);
  Begin_Time = (long) time_info.tms_utime;
#endif
#ifdef TIME
  Begin_Time = time ( (long *) 0);
#endif
#ifdef CORETIME
	Begin_Time = timer_milliseconds (&timer);
#endif

  for (Run_Index = 1; Run_Index <= Number_Of_Runs; ++Run_Index)
  {

    Proc_5();
    Proc_4();
      /* Ch_1_Glob == 'A', Ch_2_Glob == 'B', Bool_Glob == true */
    Int_1_Loc = 2;
    Int_2_Loc = 3;
    strcpy ((unsigned char*)Str_2_Loc, (const unsigned char*)"DHRYSTONE PROGRAM, 2'ND STRING");
    Enum_Loc = Ident_2;
    Bool_Glob = ! Func_2 (Str_1_Loc, Str_2_Loc);
      /* Bool_Glob == 1 */
    while (Int_1_Loc < Int_2_Loc)  /* loop body executed once */
    {
      Int_3_Loc = 5 * Int_1_Loc - Int_2_Loc;
        /* Int_3_Loc == 7 */
      Proc_7 (Int_1_Loc, Int_2_Loc, &Int_3_Loc);
        /* Int_3_Loc == 7 */
      Int_1_Loc += 1;
    } /* while */
      /* Int_1_Loc == 3, Int_2_Loc == 3, Int_3_Loc == 7 */
    Proc_8 (Arr_1_Glob, Arr_2_Glob, Int_1_Loc, Int_3_Loc);
      /* Int_Glob == 5 */
    Proc_1 (Ptr_Glob);
    for (Ch_Index = 'A'; Ch_Index <= Ch_2_Glob; ++Ch_Index)
                             /* loop body executed twice */
    {
      if (Enum_Loc == Func_1 (Ch_Index, 'C'))
          /* then, not executed */
        {
        Proc_6 (Ident_1, &Enum_Loc);
        strcpy ((unsigned char*)Str_2_Loc, (const unsigned char*)"DHRYSTONE PROGRAM, 3'RD STRING");
        Int_2_Loc = Run_Index;
        Int_Glob = Run_Index;
        }
    }
      /* Int_1_Loc == 3, Int_2_Loc == 3, Int_3_Loc == 7 */
    Int_2_Loc = Int_2_Loc * Int_1_Loc;
    Int_1_Loc = Int_2_Loc / Int_3_Loc;
    Int_2_Loc = 7 * (Int_2_Loc - Int_3_Loc) - Int_1_Loc;
      /* Int_1_Loc == 1, Int_2_Loc == 13, Int_3_Loc == 7 */
    Proc_2 (&Int_1_Loc);
      /* Int_1_Loc == 5 */

  } /* loop "for Run_Index" */

  /**************/
  /* Stop timer */
  /**************/
  
#ifdef TIMES
  times (&time_info);
  End_Time = (long) time_info.tms_utime;
#endif
#ifdef TIME
  End_Time = time ( (long *) 0);
#endif
#ifdef CORETIME
	End_Time = timer_milliseconds (&timer);	
#endif

  debug_printf("Execution ends\n");
  debug_printf("\n");
  debug_printf("Final values of the variables used in the benchmark:\n");
  debug_printf("\n");
  debug_printf("Int_Glob:            %d\n", Int_Glob);
  debug_printf("        should be:   %d\n", 5);
  debug_printf("Bool_Glob:           %d\n", Bool_Glob);
  debug_printf("        should be:   %d\n", 1);
  debug_printf("Ch_1_Glob:           %c\n", Ch_1_Glob);
  debug_printf("        should be:   %c\n", 'A');
  debug_printf("Ch_2_Glob:           %c\n", Ch_2_Glob);
  debug_printf("        should be:   %c\n", 'B');
  debug_printf("Arr_1_Glob[8]:       %d\n", Arr_1_Glob[8]);
  debug_printf("        should be:   %d\n", 7);
  debug_printf("Arr_2_Glob[8][7]:    %d\n", Arr_2_Glob[8][7]);
  debug_printf("        should be:   Number_Of_Runs + 10\n");
  debug_printf("Ptr_Glob->\n");
  debug_printf("  Ptr_Comp:          %d\n", (int) Ptr_Glob->Ptr_Comp);
  debug_printf("        should be:   (implementation-dependent)\n");
  debug_printf("  Discr:             %d\n", Ptr_Glob->Discr);
  debug_printf("        should be:   %d\n", 0);
  debug_printf("  Enum_Comp:         %d\n", Ptr_Glob->variant.var_1.Enum_Comp);
  debug_printf("        should be:   %d\n", 2);
  debug_printf("  Int_Comp:          %d\n", Ptr_Glob->variant.var_1.Int_Comp);
  debug_printf("        should be:   %d\n", 17);
  debug_printf("  Str_Comp:          %s\n", Ptr_Glob->variant.var_1.Str_Comp);
  debug_printf("        should be:   DHRYSTONE PROGRAM, SOME STRING\n");
  debug_printf("Next_Ptr_Glob->\n");
  debug_printf("  Ptr_Comp:          %d\n", (int) Next_Ptr_Glob->Ptr_Comp);
  debug_printf("        should be:   (implementation-dependent), same as above\n");
  debug_printf("  Discr:             %d\n", Next_Ptr_Glob->Discr);
  debug_printf("        should be:   %d\n", 0);
  debug_printf("  Enum_Comp:         %d\n", Next_Ptr_Glob->variant.var_1.Enum_Comp);
  debug_printf("        should be:   %d\n", 1);
  debug_printf("  Int_Comp:          %d\n", Next_Ptr_Glob->variant.var_1.Int_Comp);
  debug_printf("        should be:   %d\n", 18);
  debug_printf("  Str_Comp:          %s\n",
                                Next_Ptr_Glob->variant.var_1.Str_Comp);
  debug_printf("        should be:   DHRYSTONE PROGRAM, SOME STRING\n");
  debug_printf("Int_1_Loc:           %d\n", Int_1_Loc);
  debug_printf("        should be:   %d\n", 5);
  debug_printf("Int_2_Loc:           %d\n", Int_2_Loc);
  debug_printf("        should be:   %d\n", 13);
  debug_printf("Int_3_Loc:           %d\n", Int_3_Loc);
  debug_printf("        should be:   %d\n", 7);
  debug_printf("Enum_Loc:            %d\n", Enum_Loc);
  debug_printf("        should be:   %d\n", 1);
  debug_printf("Str_1_Loc:           %s\n", Str_1_Loc);
  debug_printf("        should be:   DHRYSTONE PROGRAM, 1'ST STRING\n");
  debug_printf("Str_2_Loc:           %s\n", Str_2_Loc);
  debug_printf("        should be:   DHRYSTONE PROGRAM, 2'ND STRING\n");
  debug_printf("\n");

  User_Time = End_Time - Begin_Time;

  if (User_Time < Too_Small_Time)
  {
    debug_printf("Measured time too small to obtain meaningful results [%d]\n",User_Time);
    debug_printf("Please increase number of runs\n");
    debug_printf("\n");
  }
  else
  {
    Microseconds = ((float) User_Time) / (1000 * Number_Of_Runs);
    Dhrystones_Per_Second = ((float) Number_Of_Runs) / (Mic_secs_Per_Second * Microseconds);	

    debug_printf("total time                                :%6.3f \n",(((float) User_Time) / 1000));
    debug_printf("Dhrystones per Second                     :");
    debug_printf("%6.1f \n", Dhrystones_Per_Second);
    debug_printf("DMIPS\\MHz                                 :%6.3f \n", ((float)Dhrystones_Per_Second)/KHZ);

    debug_printf("\n");
  }
  
//  exit(0);
}


void Proc_1 (Ptr_Val_Par)
/******************/

REG Rec_Pointer Ptr_Val_Par;
    /* executed once */
{
  REG Rec_Pointer Next_Record = Ptr_Val_Par->Ptr_Comp;  
                                        /* == Ptr_Glob_Next */
  /* Local variable, initialized with Ptr_Val_Par->Ptr_Comp,    */
  /* corresponds to "rename" in Ada, "with" in Pascal           */
  
  structassign (*Ptr_Val_Par->Ptr_Comp, *Ptr_Glob); 
  Ptr_Val_Par->variant.var_1.Int_Comp = 5;
  Next_Record->variant.var_1.Int_Comp 
        = Ptr_Val_Par->variant.var_1.Int_Comp;
  Next_Record->Ptr_Comp = Ptr_Val_Par->Ptr_Comp;
  Proc_3 (&Next_Record->Ptr_Comp);
    /* Ptr_Val_Par->Ptr_Comp->Ptr_Comp 
                        == Ptr_Glob->Ptr_Comp */
  if (Next_Record->Discr == Ident_1)
    /* then, executed */
  {
    Next_Record->variant.var_1.Int_Comp = 6;
    Proc_6 (Ptr_Val_Par->variant.var_1.Enum_Comp, 
           &Next_Record->variant.var_1.Enum_Comp);
    Next_Record->Ptr_Comp = Ptr_Glob->Ptr_Comp;
    Proc_7 (Next_Record->variant.var_1.Int_Comp, 10, 
           &Next_Record->variant.var_1.Int_Comp);
  }
  else /* not executed */
    structassign (*Ptr_Val_Par, *Ptr_Val_Par->Ptr_Comp);
} /* Proc_1 */


void Proc_2 (Int_Par_Ref)
/******************/
    /* executed once */
    /* *Int_Par_Ref == 1, becomes 4 */

One_Fifty   *Int_Par_Ref;
{
  One_Fifty  Int_Loc;  
  Enumeration   Enum_Loc;

  Int_Loc = *Int_Par_Ref + 10;
  do /* executed once */
    if (Ch_1_Glob == 'A')
      /* then, executed */
    {
      Int_Loc -= 1;
      *Int_Par_Ref = Int_Loc - Int_Glob;
      Enum_Loc = Ident_1;
    } /* if */
  while (Enum_Loc != Ident_1); /* true */
} /* Proc_2 */


void Proc_3 (Ptr_Ref_Par)
/******************/
    /* executed once */
    /* Ptr_Ref_Par becomes Ptr_Glob */

Rec_Pointer *Ptr_Ref_Par;

{
  if (Ptr_Glob != Null)
    /* then, executed */
    *Ptr_Ref_Par = Ptr_Glob->Ptr_Comp;
  Proc_7 (10, Int_Glob, &Ptr_Glob->variant.var_1.Int_Comp);
} /* Proc_3 */


void Proc_4 () /* without parameters */
/*******/
    /* executed once */
{
  Boolean Bool_Loc;

  Bool_Loc = Ch_1_Glob == 'A';
  Bool_Glob = Bool_Loc | Bool_Glob;
  Ch_2_Glob = 'B';
} /* Proc_4 */


void Proc_5 () /* without parameters */
/*******/
    /* executed once */
{
  Ch_1_Glob = 'A';
  Bool_Glob = false;
} /* Proc_5 */


        /* Procedure for the assignment of structures,          */
        /* if the C compiler doesn't support this feature       */
#ifdef  NOSTRUCTASSIGN
memcpy (d, s, l)
register char   *d;
register char   *s;
register int    l;
{
        while (l--) *d++ = *s++;
}
#endif


