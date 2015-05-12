from common import *

for line in itmoutput[1:-2]:
    tokens = line.split(' ')
    assert int(tokens[1]) <= int(tokens[0]) * int(itmoutput[-2])
    assert int(tokens[2]) <= int(tokens[0]) * int(itmoutput[-2])
    assertEquals(int(tokens[0]) * int(itmoutput[-2]), int(tokens[3]))
expect_ok()
