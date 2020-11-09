/****************************************************************
 *
 * Author: Justin Bradley
 * Title: hello_world.c
 * Date: Thursday, January 16, 2020
 * Description: Sample super-simple file in C
 *
 ****************************************************************/

#include <stdio.h>

#include "foo.h"


int main(int argc, char** argv)
{
		printf("Hello World\n");
		printf("foobar\n");
		// now print a message using foo
		print_msg("message from print_msg\n");
		return 0;
}
