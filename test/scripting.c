/* fuck dis */

#include "syscall.h"

int
main() {


  char *args[2];

  args[0] = "checkTest";
  args[1] = (char *)0;

  CheckPoint("checkTest");
  Exec("checkTest", args);

}
