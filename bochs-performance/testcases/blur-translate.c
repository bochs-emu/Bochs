/*
 * $Id: blur-translate.c,v 1.4 2002-04-17 22:51:58 bdenney Exp $
 *
 * This function is a proof of concept for dynamic C code generation.  It
 * defines a set of simple opcodes which can be used to implement the blur.c
 * operation.  A block of opcodes can be emulated/interpreted, or translated
 * into C code, compiled as a dynamic library and loaded into the binary for
 * much faster execution.
 *
 * A CodeBlock structure holds a sequence of opcodes to be translated, and
 * has fields for the dynamic library handle and a function pointer to the
 * translated function.  When a block has not been translated, the function
 * pointer is NULL.  The block may be emulated/interpreted by passing its
 * opcode_list to the emulate_opcodes() function.  To translate the block,
 * you pass it into translate_block().
 *
 * The translation process begins by writing C code into a file.  One 
 * CodeBlock is translated as one function.  There is some fixed code that
 * is always printed at the beginning and end of each function.  Each
 * opcode turns into one macro call in the function, like this:
 *       DO_MOVE_REL(-1,-1);
 *       DO_SET_ACCUM(0);
 *       DO_ADD_DATA();
 * The macro definitions are defined in a static header file which is 
 * #included at the top of the translated C code.
 *
 * Once the C code is generated, a helper script called buildshared is 
 * used to compile the shared library.  The script uses GNU libtool to
 * compile the C code into a dynamic library.  When it's done, we can
 * dlopen() the dynamic library and fill in the missing CodeBlock fields:
 * the handle to the dynamic library (so that it could be closed some day)
 * and the function pointer to the translated code.
 *
 * The function execute_code_block() runs translated code.  In fact, if
 * the code block has not been translated, it translated it first and then
 * runs it.
 *
 * Performance:
 *
 * For these simple opcodes, I regularly see a 10x speedup in translated 
 * code.
 *
 * On a 750MHz Athlon:
 *   emulated code: 10.1msec per 2D blur
 *   translated code: 1.08msec per 2D blur
 *   translation takes 484msec
 *
 * To do:
 * - deal with errors. for example if the compile fails, I could just mark
 *   the CodeBlock as not compilable and always emulate it.
 * - there are currently two implementations of each opcode, one in 
 *   translate2-defs.h and one in blur-translate.c.  I should be able to
 *   get it down to just one implementation that is used in both contexts.
 *   This will make it a lot easier to maintain.
 *
 */

#include <stdio.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <unistd.h>
#include <assert.h>
#include <ltdl.h>       // libtool library 
#include "blur-translate.h"

#define BLUR_WINDOW_HALF 1
#define DEFAULT_TIMES 1000

int array[MAX_ARRAY][MAX_ARRAY];
int array2[MAX_ARRAY][MAX_ARRAY];

#define MAX_TIMERS 3
struct timeval start[MAX_TIMERS], stop[MAX_TIMERS];
#define start_timer(T) gettimeofday (&start[T], NULL);
#define stop_timer(T) gettimeofday (&stop[T], NULL);

void report_time (FILE *fp, int T, int iters)
{
  int usec_duration = 
    (stop[T].tv_sec*1000000 + stop[T].tv_usec)
    - (start[T].tv_sec*1000000 + start[T].tv_usec);
  double sec = (double)usec_duration / 1.0e3;
  double sec_per_iter = sec / (double)iters;
  fprintf (fp, "%f msec\n", sec);
  if (iters!=1) {
    fprintf (fp, "Iterations = %d\n", iters);
    fprintf (fp, "Time per iteration = %f msec\n", sec_per_iter);
  }
}

typedef enum {
  OP_MOVE_REL,           // 2 args delta_x and delta_y
  OP_SET_ACCUM,          // 1 arg, sets accum to that arg
  OP_ADD_DATA,           // add data from *load_ptr
  OP_SUBTRACT_DATA,      // sub data from *load_ptr
  OP_MULTIPLY_DATA,      // mul data from *load_ptr
  OP_STORE_DATA,         // store accum to *store_ptr
  OP_END,                // stop
  N_OPS  // must be last
} Opcode;

typedef void (*exec_func)(State *state);

typedef struct {
  int n_opcodes;
  int *opcode_list;
  lt_dlhandle dlhandle;
  exec_func func;
} CodeBlock;

// this opcode sequence implements the blur filter, just like all the others.
int blur_instructions[] = {
  OP_MOVE_REL, -1, -1,
  OP_SET_ACCUM, 0,
  OP_ADD_DATA,
  OP_MOVE_REL, 0, 1,
  OP_ADD_DATA,
  OP_MOVE_REL, 0, 1,
  OP_ADD_DATA,
  OP_MOVE_REL, 1, -2,
  OP_ADD_DATA,
  OP_MOVE_REL, 0, 1,
  OP_ADD_DATA,
  OP_MOVE_REL, 0, 1,
  OP_ADD_DATA,
  OP_MOVE_REL, 1, -2,
  OP_ADD_DATA,
  OP_MOVE_REL, 0, 1,
  OP_ADD_DATA,
  OP_MOVE_REL, 0, 1,
  OP_ADD_DATA,
  OP_MOVE_REL, -1, -1,
  OP_STORE_DATA,
  OP_END
};

CodeBlock blur = {
  sizeof(blur_instructions),
  &blur_instructions[0],
  NULL,
  NULL
};

void print_state (State *state)
{
  printf ("state={x=%d, y=%d, accum=%d, load_ptr=%p, store_ptr=%p\n",
      state->x,
      state->y,
      state->accum,
      state->load_ptr,
      state->store_ptr);
}

// this is the original blur function from blur.c.  I keep it around
// for regression testing.
void blur_reference()
{
  int sum;
  int x,y,x2,y2;
  for (x=1; x<MAX_ARRAY-1; x++)
    for (y=1; y<MAX_ARRAY-1; y++)
    {
      sum = 0;
      for (x2=x-BLUR_WINDOW_HALF; x2<=x+BLUR_WINDOW_HALF; x2++)
	for (y2=y-BLUR_WINDOW_HALF; y2<=y+BLUR_WINDOW_HALF; y2++) {
	  sum += array[x2][y2];
	}
      array2[x][y] = sum;
    }
}

// This is the opcode emulator.  It uses switch statement to decode
// the opcodes and implement them.
//
#define ST(x) (state.x)
void emulate_opcodes (int *opcode_list)
{
  int sum;
  int x,y,x2,y2;
  int arg1,arg2;
  State state;
  for (x=1; x<MAX_ARRAY-1; x++)
    for (y=1; y<MAX_ARRAY-1; y++)
    {
      int *pc;
      int done = 0;
      sum = 0;
      pc = opcode_list;
      // set up state 
      state.x = x;
      state.y = y;
      state.load_ptr = &array[x][y];
      state.store_ptr = &array2[x][y];
      while (!done) {
	//print_state (&state);
	switch (*pc++)
	{
	  case OP_MOVE_REL:
	    arg1=*pc++;
	    arg2=*pc++;
	    DO_MOVE_REL(arg1,arg2);
	    break;
	  case OP_SET_ACCUM:
	    arg1=*pc++;
	    DO_SET_ACCUM(arg1);
	    break;
	  case OP_ADD_DATA:
	    DO_ADD_DATA();
	    break;
	  case OP_SUBTRACT_DATA:
	    DO_SUBTRACT_DATA();
	    break;
	  case OP_MULTIPLY_DATA:
	    DO_MULTIPLY_DATA();
	    break;
	  case OP_STORE_DATA:
	    DO_STORE_DATA();
	    break;
	  case OP_END:
	    done = 1;
	    break;
	  default:
	    assert (0);
	}
      }
    }
}

int gen_header (FILE *out)
{
  fprintf (out, "// code generated by blur-translate.c\n");
  fprintf (out, "#define IN_TRANSLATED_CODE 1\n");
  fprintf (out, "#include \"blur-translate.h\"\n");
  fprintf (out, "\n");
  return 0;
}

int gen_function (int *instructions, FILE *out, int fnid)
{
  int done = 0;
  int *pc = instructions;
  fprintf (out, "\nvoid translate%d (State *state) {\n", fnid);
  // the BEGIN_TRANSLATED_FUNCTION macro is defined in the 
  // blur-translate.h file.  It creates local variables to mirror the
  // frequently used fields in the state struct.
  fprintf (out, "BEGIN_TRANSLATED_FUNCTION()\n");

  // this code is executed before the block begins, each time.
  fprintf (out, "  ST(load_ptr) =  &array[x][y];\n");
  fprintf (out, "  ST(store_ptr) =  &array2[x][y];\n");

  while (!done) {
    switch (*pc++) {
      case OP_MOVE_REL:
	fprintf (out, "DO_MOVE_REL(%d,%d);\n", *pc++, *pc++);
	break;
      case OP_SET_ACCUM:
	fprintf (out, "DO_SET_ACCUM(%d);\n", *pc++);
	break;
      case OP_ADD_DATA:
	fprintf (out, "DO_ADD_DATA();\n");
	break;
      case OP_SUBTRACT_DATA:
	fprintf (out, "DO_SUBTRACT_DATA();\n");
	break;
      case OP_MULTIPLY_DATA:
	fprintf (out, "DO_MULTIPLY_DATA();\n");
	break;
      case OP_STORE_DATA:
	fprintf (out, "DO_STORE_DATA();\n");
	break;
      case OP_END:
	done = 1;
	break;
      default:
	assert (0);
    }
  }

  // loop count updates
  fprintf (out, "  ST(y)++;                   \n");
  fprintf (out, "  if (!(ST(y)<MAX_ARRAY-1)) {      \n");
  fprintf (out, "    ST(y)=1;                 \n");
  fprintf (out, "    ST(x)++;                 \n");
  fprintf (out, "    if (!(ST(x)<MAX_ARRAY-1)) {    \n");
  fprintf (out, "	done=1;                \n");
  fprintf (out, "    }                        \n");
  fprintf (out, "  }                          \n");

  fprintf (out, "END_TRANSLATED_FUNCTION()\n");
  fprintf (out, "}  // end of translate%d\n", fnid);
  return 0;
}

static int unique_num = 1001;
int translate_block (CodeBlock *block)
{
  char buffer[1024];
  FILE *fp;
  int n;
  struct stat st;
  int id = unique_num++;
  if (block->func != NULL) return 0;
  block->dlhandle = NULL;
  block->func = NULL;
  start_timer(1);
  // generate C code
  sprintf (buffer, "translate%d.c", id);
  fprintf (stderr, "building translation function in %s\n", buffer);
  fp = fopen (buffer, "w");
  assert (fp!=NULL);
  n = gen_header (fp);
  assert (n>=0);
  n = gen_function (block->opcode_list, fp, id);
  assert (n>=0);
  fclose (fp);
  // compile into a shared library
  fprintf (stderr, "compiling %s\n", buffer);
  sprintf (buffer, "./buildshared translate%d", id);
  if (system (buffer) < 0) {
    fprintf (stderr, "failed: %s\n", buffer);
    return -1;
  }
  sprintf (buffer, "translate%d.c", id);
  if (stat (buffer, &st) < 0) {
    fprintf (stderr, "stat failed\n");
    return -1;
  }
  // open shared library and get the function pointer
  sprintf (buffer, "libtranslate%d.la", id);
  block->dlhandle = lt_dlopen (buffer);
  if (!block->dlhandle) {
    fprintf (stderr, "can't open the module %s!\n", buffer);
    fprintf (stderr, "error was: %s\n", lt_dlerror());
    return -1;
  }
  sprintf (buffer, "translate%d", id);
  block->func = (void (*)(State *)) lt_dlsym(block->dlhandle, buffer);
  if (!block->func) {
    fprintf (stderr, "can't find symbol %s\n", buffer);
    return -1;
  }
  stop_timer(1);
  fprintf (stderr, "Loaded shared library.\n");
  fprintf (stderr, "Translation took ");
  report_time(stderr, 1, 1);
  return 0;
}

void execute_code_block (CodeBlock *block)
{
  State state;
  if (!block->func) {
    int n = translate_block (block);
    assert (n>=0);
    assert (block->func != NULL);
  }
  state.x = 1;
  state.y = 1;
  (*block->func)(&state);
}

void fill_array()
{
  int x,y;
  for (x=0; x<MAX_ARRAY; x++)
    for (y=0; y<MAX_ARRAY; y++)
      array[x][y] = (x*17+y*31)%29;
}

void dump_array (FILE *fp, int ptr[MAX_ARRAY][MAX_ARRAY])
{
  int x,y;
  for (x=0; x<MAX_ARRAY; x++) {
    for (y=0; y<MAX_ARRAY; y++) {
      fprintf (fp, "%3d ", ptr[x][y]);
    }
    fprintf (fp, "\n");
  }
}

void usage ()
{
  fprintf (stderr, "Usage: blur-translate [-emulate | -reference | -translate] [iterations]\n");
  exit(1);
}

enum {
  METHOD_REFERENCE,
  METHOD_EMULATE,
  METHOD_TRANSLATE
};

int main (int argc, char *argv[])
{
  int i;
  int times = DEFAULT_TIMES;
  FILE *out;
  int arg = 1;
  int method = METHOD_TRANSLATE;
  for (arg=1; arg<argc; arg++) {
    if (!strncmp (argv[arg], "-ref", 4)) {
      fprintf (stderr, "Using reference implementation.\n");
      method = METHOD_REFERENCE;
    } else if (!strncmp (argv[arg], "-emu", 4)) {
      fprintf (stderr, "Using emulation only.\n");
      method = METHOD_EMULATE;
    } else if (!strncmp (argv[arg], "-tra", 4)) {
      fprintf (stderr, "Using translation only.\n");
      method = METHOD_TRANSLATE;
    } else if (argv[arg][0]>='0' && argv[arg][0]<='9') {
      sscanf (argv[arg], "%d", &times);
      fprintf (stderr, "Set iterations to %d\n", times);
    } else {
      usage();
    }
  }
  if (lt_dlinit () != 0) {
    fprintf (stderr, "lt_dlinit() failed\n");
    exit (1);
  }
  fill_array();
  //dump_array (stderr);
  start_timer(0);
  for (i=0; i<times; i++) {
    switch (method) {
    case METHOD_REFERENCE:
      blur_reference ();
      break;
    case METHOD_EMULATE:
      emulate_opcodes (blur.opcode_list);
      break;
    case METHOD_TRANSLATE:
      execute_code_block (&blur);
      break;
    }
  }
  stop_timer(0);
  printf ("Total time elapsed: ");
  report_time (stdout, 0, times);
  //fprintf (stderr, "-----------------------------------\n");
  out = fopen ("blur.out", "w");
  assert (out != NULL);
  dump_array (out, array2);
  fclose (out);
  return 0;
}
