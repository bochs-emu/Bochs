/////////////////////////////////////////////////////////////////////////
//
// test-access-check.cc
// $Id: test-access-check.cc,v 1.3 2001-10-03 13:10:38 bdenney Exp $
// by Bryce Denney
//
// This whole program is intended to test just a few lines in
// cpu/access.cc, where it checks if a memory access goes over the
// segment limits.
//
// NOTE: The variable called limit is actually "limit_scaled" in the
// bochs code.  Limit_scaled is equal to the limit when granularity is 0,
// and equal to (limit << 12) | 0xfff when granularity is 1.
//
// Compile with:
//   cc -I. -Iinstrument/stubs -o test-access-check misc/test-access-check.cc
// Then run "test-access-check" and see how it goes.  If mismatches=0,
// the rule is good.
//
///////////////////////////////////////////////////////////////////////////////

#include <bochs.h>

// rule1 is the original rule used in Bochs from 2000-03-25.  It has bad
// overflow problems.
int rule1 (Bit32u limit, Bit32u length, Bit32u offset);

// rule2 is Bryce's first try, which turned out to have bad underflow problems 
// instead.
int rule2 (Bit32u limit, Bit32u length, Bit32u offset);

// rule3 is Bryce's second try, which as far as he knows works for all 
// cases.
int rule3 (Bit32u limit, Bit32u length, Bit32u offset);

// edit this to test other rules.
#define TEST_RULE rule3

typedef struct {
  Bit32u limit, length, offset, the_answer;
} TestStruct;

TestStruct tests[] = {
// limit  length   offset   exception
{0x0a,            4,            0,            0},            
{0x0a,            4,            1,            0},            
{0x0a,            4,            2,            0},            
{0x0a,            4,            3,            0},            
{0x0a,            4,            4,            0},            
{0x0a,            4,            5,            0},            
{0x0a,            4,            6,            0},            
{0x0a,            4,            7,            0},            
{0x0a,            4,            8,            1},            
{0x0a,            4,            9,            1},            
{0x0a,            4,            0x0a,         1},            
{0x0a,            4,            0x0b,         1},            
{0x0a,            4,            0x0c,         1},            
{0x0a,            4,            0xfffffffd,   1},            
{0x0a,            4,            0xfffffffe,   1},            
{0x0a,            4,            0xffffffff,   1},            
{0x0a,            2,            0,            0},            
{0x0a,            2,            1,            0},            
{0x0a,            2,            2,            0},            
{0x0a,            2,            3,            0},            
{0x0a,            2,            4,            0},            
{0x0a,            2,            5,            0},            
{0x0a,            2,            6,            0},            
{0x0a,            2,            7,            0},            
{0x0a,            2,            8,            0},            
{0x0a,            2,            9,            0},            
{0x0a,            2,            0x0a,         1},            
{0x0a,            2,            0x0b,         1},            
{0x0a,            2,            0x0c,         1},            
{0x0a,            2,            0xfffffffd,   1},            
{0x0a,            2,            0xfffffffe,   1},            
{0x0a,            2,            0xffffffff,   1},            
{0x0a,            1,            0,            0},            
{0x0a,            1,            1,            0},            
{0x0a,            1,            2,            0},            
{0x0a,            1,            3,            0},            
{0x0a,            1,            4,            0},            
{0x0a,            1,            5,            0},            
{0x0a,            1,            6,            0},            
{0x0a,            1,            7,            0},            
{0x0a,            1,            8,            0},            
{0x0a,            1,            9,            0},            
{0x0a,            1,            0x0a,         0},            
{0x0a,            1,            0x0b,         1},            
{0x0a,            1,            0x0c,         1},            
//
// try a few near the common 0xffff boundary
//
{0xffff,            1,            0,          0},            
{0xffff,            1,            0x100,      0},            
{0xffff,            1,            0x1000,     0},            
{0xffff,            1,            0x8000,     0},            
{0xffff,            1,            0xf000,     0},            
{0xffff,            1,            0xfff0,     0},            
{0xffff,            1,            0xfff1,     0},            
{0xffff,            1,            0xfff2,     0},            
{0xffff,            1,            0xfff3,     0},            
{0xffff,            1,            0xfff4,     0},            
{0xffff,            1,            0xfff5,     0},            
{0xffff,            1,            0xfff6,     0},            
{0xffff,            1,            0xfff7,     0},            
{0xffff,            1,            0xfff8,     0},            
{0xffff,            1,            0xfff9,     0},            
{0xffff,            1,            0xfffa,     0},            
{0xffff,            1,            0xfffb,     0},            
{0xffff,            1,            0xfffc,     0},            
{0xffff,            1,            0xfffd,     0},            
{0xffff,            1,            0xfffe,     0},            
{0xffff,            1,            0xffff,     0},            
{0xffff,            1,            0x10000,    1},            
{0xffff,            1,            0x10001,    1},            
{0xffff,            1,            0x10002,    1},            
{0xffff,            1,            0x10021,    1},            
{0xffff,            1,            0x40001,    1},            
{0xffff,            1,            0xffffffff, 1},            
//
// near 0xffff,             length 2
//
{0xffff,            2,            0,          0},            
{0xffff,            2,            0x100,      0},            
{0xffff,            2,            0x1000,     0},            
{0xffff,            2,            0x8000,     0},            
{0xffff,            2,            0xf000,     0},            
{0xffff,            2,            0xfff0,     0},            
{0xffff,            2,            0xfff1,     0},            
{0xffff,            2,            0xfff2,     0},            
{0xffff,            2,            0xfff3,     0},            
{0xffff,            2,            0xfff4,     0},            
{0xffff,            2,            0xfff5,     0},            
{0xffff,            2,            0xfff6,     0},            
{0xffff,            2,            0xfff7,     0},            
{0xffff,            2,            0xfff8,     0},            
{0xffff,            2,            0xfff9,     0},            
{0xffff,            2,            0xfffa,     0},            
{0xffff,            2,            0xfffb,     0},            
{0xffff,            2,            0xfffc,     0},            
{0xffff,            2,            0xfffd,     0},            
{0xffff,            2,            0xfffe,     0},            
{0xffff,            2,            0xffff,     1},            
{0xffff,            2,            0x10000,    1},            
{0xffff,            2,            0x10001,    1},            
{0xffff,            2,            0x10002,    1},            
{0xffff,            2,            0x10021,    1},            
{0xffff,            2,            0x40001,    1},            
{0xffff,            2,            0xffffffff, 1},            
//
// near 0xffff,             length 4
//
{0xffff,            4,            0,          0},            
{0xffff,            4,            0x100,      0},            
{0xffff,            4,            0x1000,     0},            
{0xffff,            4,            0x8000,     0},            
{0xffff,            4,            0xf000,     0},            
{0xffff,            4,            0xfff0,     0},            
{0xffff,            4,            0xfff1,     0},            
{0xffff,            4,            0xfff2,     0},            
{0xffff,            4,            0xfff3,     0},            
{0xffff,            4,            0xfff4,     0},            
{0xffff,            4,            0xfff5,     0},            
{0xffff,            4,            0xfff6,     0},            
{0xffff,            4,            0xfff7,     0},            
{0xffff,            4,            0xfff8,     0},            
{0xffff,            4,            0xfff9,     0},            
{0xffff,            4,            0xfffa,     0},            
{0xffff,            4,            0xfffb,     0},            
{0xffff,            4,            0xfffc,     0},            
{0xffff,            4,            0xfffd,     1},            
{0xffff,            4,            0xfffe,     1},            
{0xffff,            4,            0xffff,     1},            
{0xffff,            4,            0x10000,    1},            
{0xffff,            4,            0x10001,    1},            
{0xffff,            4,            0x10002,    1},            
{0xffff,            4,            0x10021,    1},            
{0xffff,            4,            0x40001,    1},            
{0xffff,            4,            0xffffffff, 1},            
//
// now a few near the max limit 0xffffffff
//
{0xffffffff,            1,       0,           0},            
{0xffffffff,            1,       0x100,       0},            
{0xffffffff,            1,       0x1000,      0},            
{0xffffffff,            1,       0x8000,      0},            
{0xffffffff,            1,       0xf000,      0},            
{0xffffffff,            1,       0xfff0,      0},            
{0xffffffff,            1,       0xfff1,      0},            
{0xffffffff,            1,       0xfff2,      0},            
{0xffffffff,            1,       0xfff3,      0},            
{0xffffffff,            1,       0xfff4,      0},            
{0xffffffff,            1,       0xfff5,      0},            
{0xffffffff,            1,       0xfff6,      0},            
{0xffffffff,            1,       0xfff7,      0},            
{0xffffffff,            1,       0xfff8,      0},            
{0xffffffff,            1,       0xfff9,      0},            
{0xffffffff,            1,       0xfffa,      0},            
{0xffffffff,            1,       0xfffb,      0},            
{0xffffffff,            1,       0xfffc,      0},            
{0xffffffff,            1,       0xfffd,      0},            
{0xffffffff,            1,       0xfffe,      0},            
{0xffffffff,            1,       0xffff,      0},            
{0xffffffff,            1,       0x10000,     0},            
{0xffffffff,            1,       0x10001,     0},            
{0xffffffff,            1,       0x10002,     0},            
{0xffffffff,            1,       0x10021,     0},            
{0xffffffff,            1,       0x40001,     0},            
{0xffffffff,            1,       0xfffffff0,  0},            
{0xffffffff,            1,       0xfffffff9,  0},            
{0xffffffff,            1,       0xfffffffa,  0},            
{0xffffffff,            1,       0xfffffffb,  0},            
{0xffffffff,            1,       0xfffffffc,  0},            
{0xffffffff,            1,       0xfffffffd,  0},            
{0xffffffff,            1,       0xfffffffe,  0},            
{0xffffffff,            1,       0xffffffff,  0},            
{0xffffffff,            1,       0x00000000,  0},            
{0xffffffff,            1,       0x00000001,  0},            
{0xffffffff,            1,       0x00000002,  0},            
{0xffffffff,            1,       0x00000003,  0},            
{0xffffffff,            1,       0x00000004,  0},            
{0xffffffff,            1,       0x00000005,  0},            
//
// repeat with length 2
//
{0xffffffff,            2,       0,           0},            
{0xffffffff,            2,       0x100,       0},            
{0xffffffff,            2,       0x1000,      0},            
{0xffffffff,            2,       0x8000,      0},            
{0xffffffff,            2,       0xf000,      0},            
{0xffffffff,            2,       0xfff0,      0},            
{0xffffffff,            2,       0xfff1,      0},            
{0xffffffff,            2,       0xfff2,      0},            
{0xffffffff,            2,       0xfff3,      0},            
{0xffffffff,            2,       0xfff4,      0},            
{0xffffffff,            2,       0xfff5,      0},            
{0xffffffff,            2,       0xfff6,      0},            
{0xffffffff,            2,       0xfff7,      0},            
{0xffffffff,            2,       0xfff8,      0},            
{0xffffffff,            2,       0xfff9,      0},            
{0xffffffff,            2,       0xfffa,      0},            
{0xffffffff,            2,       0xfffb,      0},            
{0xffffffff,            2,       0xfffc,      0},            
{0xffffffff,            2,       0xfffd,      0},            
{0xffffffff,            2,       0xfffe,      0},            
{0xffffffff,            2,       0xffff,      0},            
{0xffffffff,            2,       0x10000,     0},            
{0xffffffff,            2,       0x10001,     0},            
{0xffffffff,            2,       0x10002,     0},            
{0xffffffff,            2,       0x10021,     0},            
{0xffffffff,            2,       0x40001,     0},            
{0xffffffff,            2,       0xfffffff0,  0},            
{0xffffffff,            2,       0xfffffff9,  0},            
{0xffffffff,            2,       0xfffffffa,  0},            
{0xffffffff,            2,       0xfffffffb,  0},            
{0xffffffff,            2,       0xfffffffc,  0},            
{0xffffffff,            2,       0xfffffffd,  0},            
{0xffffffff,            2,       0xfffffffe,  0},            
{0xffffffff,            2,       0xffffffff,  1},            
{0xffffffff,            2,       0x00000000,  0},            
{0xffffffff,            2,       0x00000001,  0},            
{0xffffffff,            2,       0x00000002,  0},            
{0xffffffff,            2,       0x00000003,  0},            
{0xffffffff,            2,       0x00000004,  0},            
{0xffffffff,            2,       0x00000005,  0},            
//
// repeat with length 4
//
{0xffffffff,            4,       0,           0},            
{0xffffffff,            4,       0x100,       0},            
{0xffffffff,            4,       0x1000,      0},            
{0xffffffff,            4,       0x8000,      0},            
{0xffffffff,            4,       0xf000,      0},            
{0xffffffff,            4,       0xfff0,      0},            
{0xffffffff,            4,       0xfff1,      0},            
{0xffffffff,            4,       0xfff2,      0},            
{0xffffffff,            4,       0xfff3,      0},            
{0xffffffff,            4,       0xfff4,      0},            
{0xffffffff,            4,       0xfff5,      0},            
{0xffffffff,            4,       0xfff6,      0},            
{0xffffffff,            4,       0xfff7,      0},            
{0xffffffff,            4,       0xfff8,      0},            
{0xffffffff,            4,       0xfff9,      0},            
{0xffffffff,            4,       0xfffa,      0},            
{0xffffffff,            4,       0xfffb,      0},            
{0xffffffff,            4,       0xfffc,      0},            
{0xffffffff,            4,       0xfffd,      0},            
{0xffffffff,            4,       0xfffe,      0},            
{0xffffffff,            4,       0xffff,      0},            
{0xffffffff,            4,       0x10000,     0},            
{0xffffffff,            4,       0x10001,     0},            
{0xffffffff,            4,       0x10002,     0},            
{0xffffffff,            4,       0x10021,     0},            
{0xffffffff,            4,       0x40001,     0},            
{0xffffffff,            4,       0xfffffff0,  0},            
{0xffffffff,            4,       0xfffffff9,  0},            
{0xffffffff,            4,       0xfffffffa,  0},            
{0xffffffff,            4,       0xfffffffb,  0},            
{0xffffffff,            4,       0xfffffffc,  0},            
{0xffffffff,            4,       0xfffffffd,  1},            
{0xffffffff,            4,       0xfffffffe,  1},            
{0xffffffff,            4,       0xffffffff,  1},            
{0xffffffff,            4,       0x00000000,  0},            
{0xffffffff,            4,       0x00000001,  0},            
{0xffffffff,            4,       0x00000002,  0},            
{0xffffffff,            4,       0x00000003,  0},            
{0xffffffff,            4,       0x00000004,  0},            
{0xffffffff,            4,       0x00000005,  0},            
//
// now make some underflow cases to disprove my new rule
//
{1,            1,            0,                     0},            
{1,            1,            1,                     0},            
{1,            1,            2,                     1},            
{1,            1,            3,                     1},            
{1,            1,            4,                     1},            
{1,            1,            0xfffffffc,            1},            
{1,            1,            0xfffffffd,            1},            
{1,            1,            0xfffffffe,            1},            
{1,            1,            0xffffffff,            1},            
{1,            2,            0,                     0},            
{1,            2,            1,                     1},            
{1,            2,            2,                     1},            
{1,            2,            3,                     1},            
{1,            2,            4,                     1},            
{1,            2,            0xfffffffc,            1},            
{1,            2,            0xfffffffd,            1},            
{1,            2,            0xfffffffe,            1},            
{1,            2,            0xffffffff,            1},            
{1,            4,            0,                     1},            
{1,            4,            1,                     1},            
{1,            4,            2,                     1},            
{1,            4,            3,                     1},            
{1,            4,            4,                     1},            
{1,            4,            0xfffffffc,            1},            
{1,            4,            0xfffffffd,            1},            
{1,            4,            0xfffffffe,            1},            
{1,            4,            0xffffffff,            1}
};

int main () {
  int total=0, mismatches=0;
  int t;
  for (t=0; t<sizeof(tests)/sizeof(tests[0]); t++) {
    TestStruct *ts = &tests[t];
    int my_answer = TEST_RULE (ts->limit, ts->length, ts->offset);
    printf ("limit=%x len=%x offset=%x exception=%x %s\n",
	ts->limit,
	ts->length,
	ts->offset,
	my_answer,
        (ts->the_answer==my_answer) ? "" : "MISMATCH");
    if (ts->the_answer!=my_answer) mismatches++;
    total++;
  }
  printf ("mismatches=%d\n", mismatches);
  printf ("total=%d\n", total);
}

int rule1 (Bit32u limit, Bit32u length, Bit32u offset)
{
  if ((offset + length - 1) > limit) return 1;
  return 0;
}

int rule2 (Bit32u limit, Bit32u length, Bit32u offset)
{
  if (offset > (limit - length + 1)) return 1;
  return 0;
}

int rule3 (Bit32u limit, Bit32u length, Bit32u offset)
{
  if (offset > (limit - length + 1) || (length-1 > limit)) {
    return 1;
  }
  return 0;
}
