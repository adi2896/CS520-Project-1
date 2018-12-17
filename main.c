#include <stdio.h>
#include <stdlib.h>

#include "cpu.h"

int
main(int argc, char const* argv[])
{
  if (argc != 4) {
    fprintf(stderr, "APEX_Help : Usage %s <input_file>\n", argv[0]);
    exit(1);
  }

  APEX_CPU* cpu = APEX_cpu_init(argv[1]);
  if (!cpu) {
    fprintf(stderr, "APEX_Error : Unable to initialize CPU\n");
    exit(1);
  }
  
  cpu->sim=argv[2];
  cpu->no_cycles=atoi(argv[3]);

  APEX_cpu_run(cpu);
  APEX_cpu_stop(cpu);
  return 0;
}