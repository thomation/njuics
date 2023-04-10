#include "trap.h"

typedef char    Str_30 [31];
char buf[128];
Str_30  Str_1_Loc;
Str_30  Str_2_Loc;
int Func_2 (Str_1_Par_Ref, Str_2_Par_Ref)
/*************************************************/
    /* executed once */
    /* Str_1_Par_Ref == "DHRYSTONE PROGRAM, 1'ST STRING" */
    /* Str_2_Par_Ref == "DHRYSTONE PROGRAM, 2'ND STRING" */

Str_30  Str_1_Par_Ref;
Str_30  Str_2_Par_Ref;
{
	// printf("s1:%s\n", Str_1_Par_Ref);
	// printf("s2:%s\n", Str_2_Par_Ref);
  if (strcmp (Str_1_Par_Ref, Str_2_Par_Ref) > 0)
    return 1;
  return 0;
}
int main() {
	strcpy (Str_1_Loc, "DHRYSTONE PROGRAM, 1'ST STRING");
	strcpy (Str_2_Loc, "DHRYSTONE PROGRAM, 2'ND STRING");
	check(!Func_2(Str_1_Loc, Str_2_Loc));
	check(strcmp(Str_1_Loc, Str_2_Loc) < 0);
	sprintf(buf, "%s", "Hello world!\n");
	check(strcmp(buf, "Hello world!\n") == 0);

	sprintf(buf, "%d + %d = %d\n", 1, 1, 2);
	check(strcmp(buf, "1 + 1 = 2\n") == 0);

	sprintf(buf, "%d + %d = %d\n", 2, 10, 12);
	check(strcmp(buf, "2 + 10 = 12\n") == 0);
	sprintf(buf, "%x,%x,%x,%x\n", 0, 1, 15, 16);
	check(strcmp(buf, "0x0,0x1,0xf,0x10\n") == 0);

	return 0;
}
