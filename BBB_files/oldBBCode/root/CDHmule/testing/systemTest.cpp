#include <stdio.h> /* printf */ 
#include <stdlib.h> /* system, NULL, EXIT_FAILURE */

int main ()
{
  printf("in parent process");
  printf("Executing system call...\n");
  system("./childTest &");
  system("ps");
  printf("parent process continues and closes");
  return 0;
}
