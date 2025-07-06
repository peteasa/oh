provides way to read file with mesh packets and send them / display responses..
will replace sw/src/e-access

in theory this could be compiled agains the fast.ldf or the internal.ldf linker scripts. if designed correctly it would work with both.


--user and --dmesg can be omitted from the command line they are only used with the cmd_time_cmd
cmd_time_cmd helps link dmesg time to user side time (it will never be the same time stamp):

bin# ./access.elf --user=378305 --dmesg=473465 ../tests/test_simple_access_parser.emf
...
using time_offset of: 97321 last user_time usec: 513190 correction: 2161

dmesg output:
# dmesg -w
...
[ 3650.609126] epiphany: no users

next run can be improved by using the last user_time usec value and the usec value from the last dmesg as follows:

bin# ./access.elf --user=513190 --dmesg=609126 ../tests/test_simple_access_parser.emf
