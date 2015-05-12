from common import *

expect_line(r'.*__asm volatile\("bkpt"\);.*', gdboutput)
expect_line(r'^#1  0x[0-9a-f]{8} in assert .*', gdboutput)
expect_line(r'^#2  0x[0-9a-f]{8} in task_low_stack_save .*', gdboutput)
