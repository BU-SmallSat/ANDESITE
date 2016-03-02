#include <stdio.h>      /* printf */
#include <stdlib.h>     /* system, NULL, EXIT_FAILURE */
#include <unistd.h>

using namespace std;

int main ()
{
  sleep(5);
  printf ("Child process is executing...");
  return 0;
}
