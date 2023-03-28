#include "trap.h"

typedef char    Str_30 [31];
char buf[128];
Str_30  Str_1_Loc;
Str_30  Str_2_Loc;
int main() {
	strcpy (Str_1_Loc, "DHRYSTONE PROGRAM, 1'ST STRING");
	strcpy (Str_2_Loc, "DHRYSTONE PROGRAM, 2'ND STRING");
	check(strcmp(Str_1_Loc, Str_2_Loc) < 0);
	sprintf(buf, "%s", "Hello world!\n");
	check(strcmp(buf, "Hello world!\n") == 0);

	sprintf(buf, "%d + %d = %d\n", 1, 1, 2);
	check(strcmp(buf, "1 + 1 = 2\n") == 0);

	sprintf(buf, "%d + %d = %d\n", 2, 10, 12);
	check(strcmp(buf, "2 + 10 = 12\n") == 0);

	return 0;
}
