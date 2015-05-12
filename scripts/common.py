import sys
import re

assert len(sys.argv) == 3, "Usage: python %s <itmoutput> <gdboutput>" % sys.argv[0]


# Read output files

with open(sys.argv[1]) as f:
    itmoutput = f.readlines()
    itmoutput = map(lambda line: line.replace('\x01', ''), itmoutput)

with open(sys.argv[2]) as f:
    gdboutput = f.readlines()


# Helper functions

def assertEquals(v1, v2):
    assert v1 == v2, "Expected '%s', got '%s'." % (str(v1), str(v2))

def assertMatch(regex, s):
    assert re.match(regex, s), "Match '%s' not found in '%s'." % (regex, s)

def expect_line(regex, data=itmoutput):
    matching = filter(lambda line: re.match(regex, line), data)
    assert len(matching) >= 1, "Line matching '%s' not found." % needle

def expect_ok(data=itmoutput):
    assertMatch(r'^OK$', data[-1])
