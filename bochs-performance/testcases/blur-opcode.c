/*
 *
 * $Id: blur-opcode.c,v 1.4 2002-04-17 07:15:02 bdenney Exp $
 *
 */

#include <stdio.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <unistd.h>
#include <assert.h>

#define MAX 128
int array[MAX][MAX];
int array2[MAX][MAX];
#define BLUR_SIZE 3
#define BLUR_WINDOW_HALF 1

#define DEFAULT_TIMES 1000

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

typedef struct {
  int x, y;
  int accum;
  int *load_ptr;
  int *store_ptr;
  int done;
} State;

void print_state (State *state)
{
  printf ("state={x=%d, y=%d, accum=%d, load_ptr=%p, store_ptr=%p\n",
      state->x,
      state->y,
      state->accum,
      state->load_ptr,
      state->store_ptr);
}

void blur_simple()
{
  int sum;
  int x,y,x2,y2;
  for (x=1; x<MAX-1; x++)
    for (y=1; y<MAX-1; y++)
    {
      sum = 0;
      for (x2=x-BLUR_WINDOW_HALF; x2<=x+BLUR_WINDOW_HALF; x2++)
	for (y2=y-BLUR_WINDOW_HALF; y2<=y+BLUR_WINDOW_HALF; y2++) {
	  sum += array[x2][y2];
	}
      array2[x][y] = sum;
    }
}

#ifdef BLUR_USE_SWITCH
void blur_opcode_switch ()
{
  int sum;
  int x,y,x2,y2;
  Opcode *opc;
  State state;
  for (x=1; x<MAX-1; x++)
    for (y=1; y<MAX-1; y++)
    {
      int *pc;
      int done = 0;
      sum = 0;
      pc = &blur_instructions[0];
      // set up state 
      state.x = x;
      state.y = y;
      state.accum = 0xfab28342;   // start with trash in accum
      state.load_ptr = &array[x][y];
      state.store_ptr = &array2[x][y];
      while (!done) {
	//print_state (&state);
	switch (*pc++)
	{
	  case OP_MOVE_REL:
	    state.x += *pc++;
	    state.y += *pc++;
	    state.load_ptr = &array[state.x][state.y];
	    break;
	  case OP_SET_ACCUM:
	    state.accum = *pc++;
	    break;
	  case OP_ADD_DATA:
	    state.accum += *state.load_ptr;
	    break;
	  case OP_SUBTRACT_DATA:
	    state.accum -= *state.load_ptr;
	    break;
	  case OP_MULTIPLY_DATA:
	    state.accum *= *state.load_ptr;
	    break;
	  case OP_STORE_DATA:
	    *state.store_ptr = state.accum;
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
#endif // ifdef BLUR_USE_SWITCH

#ifdef BLUR_USE_FNPTR
typedef void (*opfunc)(State *state);

int *pc = NULL;

void op_move_rel (State *state) {
  state->x += *pc++;
  state->y += *pc++;
  state->load_ptr = &array[state->x][state->y];
}
void op_set_accum (State *state) {
  state->accum = *pc++;
}
void op_add_data (State *state) {
  state->accum += *state->load_ptr;
}
void op_subtract_data (State *state) {
  state->accum -= *state->load_ptr;
}
void op_multiply_data (State *state) {
  state->accum *= *state->load_ptr;
}
void op_store_data (State *state) {
  *state->store_ptr = state->accum;
}
void op_end (State *state) {
  state->done = 1;
}

opfunc op_table[N_OPS] = {
  op_move_rel, op_set_accum, op_add_data, op_subtract_data, op_multiply_data, op_store_data, op_end
};

void blur_opcode_fnptr ()
{
  int x,y;
  opfunc fnptr;
  State state;
  for (x=1; x<MAX-1; x++)
    for (y=1; y<MAX-1; y++)
    {
      // pc is global here
      pc = &blur_instructions[0];
      // set up state 
      state.x = x;
      state.y = y;
      state.accum = 0xfab28342;   // start with trash in accum
      state.load_ptr = &array[x][y];
      state.store_ptr = &array2[x][y];
      state.done = 0;
      while (!state.done) {
	//print_state (&state);
	fnptr = op_table[*pc++];
	(*fnptr)(&state);
      }
    }
}
#endif // ifdef BLUR_USE_FNPTR

#ifdef BLUR_TRANSLATED1
// in BLUR_TRANSLATED1, I pasted the implementation of each opcode into
// the source (substituting arguments).  This is significantly better than
// having to go through a switch statement or an indirect function call.
// In fact it is about 5x faster than the switch, and about 9x faster than
// the indirect function call.  However, because all the statements operate
// on the "state" object which is in memory, the compiler will not assign
// fields of state into registers.  Also, the translated function is called
// many times, creating noticeable overhead.
void blur_instructions_translated (State *state)
{
//  OP_MOVE_REL, -1, -1,
	    state->x += -1;
	    state->y += -1;
	    state->load_ptr = &array[state->x][state->y];
//  OP_SET_ACCUM, 0,
	    state->accum = 0;
//  OP_ADD_DATA,
	    state->accum += *state->load_ptr++;
//  OP_MOVE_REL, 0, 1,
	    state->x += 0;
	    state->y += 1;
	    state->load_ptr = &array[state->x][state->y];
//  OP_ADD_DATA,
	    state->accum += *state->load_ptr++;
//  OP_MOVE_REL, 0, 1,
	    state->x += 0;
	    state->y += 1;
	    state->load_ptr = &array[state->x][state->y];
//  OP_ADD_DATA,
	    state->accum += *state->load_ptr++;
//  OP_MOVE_REL, 1, -2,
	    state->x += 1;
	    state->y += -2;
	    state->load_ptr = &array[state->x][state->y];
//  OP_ADD_DATA,
	    state->accum += *state->load_ptr++;
//  OP_MOVE_REL, 0, 1,
	    state->x += 0;
	    state->y += 1;
	    state->load_ptr = &array[state->x][state->y];
//  OP_ADD_DATA,
	    state->accum += *state->load_ptr++;
//  OP_MOVE_REL, 0, 1,
	    state->x += 0;
	    state->y += 1;
	    state->load_ptr = &array[state->x][state->y];
//  OP_ADD_DATA,
	    state->accum += *state->load_ptr++;
//  OP_MOVE_REL, 1, -2,
	    state->x += 1;
	    state->y += -2;
	    state->load_ptr = &array[state->x][state->y];
//  OP_ADD_DATA,
	    state->accum += *state->load_ptr++;
//  OP_MOVE_REL, 0, 1,
	    state->x += 0;
	    state->y += 1;
	    state->load_ptr = &array[state->x][state->y];
//  OP_ADD_DATA,
	    state->accum += *state->load_ptr++;
//  OP_MOVE_REL, 0, 1,
	    state->x += 0;
	    state->y += 1;
	    state->load_ptr = &array[state->x][state->y];
//  OP_ADD_DATA,
	    state->accum += *state->load_ptr++;
//  OP_STORE_DATA,
	    *(state->store_ptr++) = state->accum;
//  OP_END
}

void blur_opcode_translated ()
{
  int sum;
  int x,y,x2,y2;
  State state;
  for (x=1; x<MAX-1; x++)
    for (y=1; y<MAX-1; y++)
    {
      int *pc;
      int done = 0;
      sum = 0;
      // set up state
      state.x = x;
      state.y = y;
      state.accum = 0xfab28342;   // start with trash in accum
      state.load_ptr = &array[x][y];
      state.store_ptr = &array2[x][y];
      // call translated function
      blur_instructions_translated (&state);
    }
}

#endif // ifdef BLUR_TRANSLATED1


#ifdef BLUR_TRANSLATED2
// in BLUR_TRANSLATED2, I have rewritten the code that calls the translated
// function, so that the code can be pasted into the translated function 
// itself.  This is a very slight improvement.  Still, all operations
// require a load and store, and no state fields are optimized into registers.
#define BEFORE_EXEC() do { \
      /* set up state */ \
      state->accum = 0xfab28342;   /* start with trash in accum */ \
      state->load_ptr = &array[state->x][state->y]; \
      state->store_ptr = &array2[state->x][state->y]; \
  } while (0)

#define AFTER_EXEC() do { \
    /* post-loop */ \
    state->y++; \
    if (!(state->y<MAX-1)) { \
      state->y=1; \
      state->x++; \
      if (!(state->x<MAX-1)) { \
	/* printf ("DONE!\n"); */ \
	done=1; \
      } \
    } \
  } while (0)

void blur_instructions_translated2 (State *state)
{
  int done = 0;
  while (!done) {
    BEFORE_EXEC();
//  OP_MOVE_REL, -1, -1,
	    state->x += -1;
	    state->y += -1;
	    state->load_ptr = &array[state->x][state->y];
//  OP_SET_ACCUM, 0,
	    state->accum = 0;
//  OP_ADD_DATA,
	    state->accum += *state->load_ptr++;
//  OP_MOVE_REL, 0, 1,
	    state->x += 0;
	    state->y += 1;
	    state->load_ptr = &array[state->x][state->y];
//  OP_ADD_DATA,
	    state->accum += *state->load_ptr++;
//  OP_MOVE_REL, 0, 1,
	    state->x += 0;
	    state->y += 1;
	    state->load_ptr = &array[state->x][state->y];
//  OP_ADD_DATA,
	    state->accum += *state->load_ptr++;
//  OP_MOVE_REL, 1, -2,
	    state->x += 1;
	    state->y += -2;
	    state->load_ptr = &array[state->x][state->y];
//  OP_ADD_DATA,
	    state->accum += *state->load_ptr++;
//  OP_MOVE_REL, 0, 1,
	    state->x += 0;
	    state->y += 1;
	    state->load_ptr = &array[state->x][state->y];
//  OP_ADD_DATA,
	    state->accum += *state->load_ptr++;
//  OP_MOVE_REL, 0, 1,
	    state->x += 0;
	    state->y += 1;
	    state->load_ptr = &array[state->x][state->y];
//  OP_ADD_DATA,
	    state->accum += *state->load_ptr++;
//  OP_MOVE_REL, 1, -2,
	    state->x += 1;
	    state->y += -2;
	    state->load_ptr = &array[state->x][state->y];
//  OP_ADD_DATA,
	    state->accum += *state->load_ptr++;
//  OP_MOVE_REL, 0, 1,
	    state->x += 0;
	    state->y += 1;
	    state->load_ptr = &array[state->x][state->y];
//  OP_ADD_DATA,
	    state->accum += *state->load_ptr++;
//  OP_MOVE_REL, 0, 1,
	    state->x += 0;
	    state->y += 1;
	    state->load_ptr = &array[state->x][state->y];
//  OP_ADD_DATA,
	    state->accum += *state->load_ptr++;
//  OP_STORE_DATA,
	    *(state->store_ptr++) = state->accum;
//  OP_MOVE_REL, -1, -1
	    state->x += -1;
	    state->y += -1;
	    state->load_ptr = &array[state->x][state->y];
//  OP_END
    AFTER_EXEC();
  }
}

void blur_opcode_translated2 ()
{
  State state;
  state.x = 1;
  state.y = 1;
  blur_instructions_translated2 (&state);
}

#endif // ifdef BLUR_TRANSLATED2

#ifdef BLUR_TRANSLATED3
// In BLUR_TRANSLATED3, I added local variable assignments for the most
// frequently used state fields (well, actually all of them).  Then at the end
// of the function it sets the state field back to the final value of the local
// variable.  This technique encourages the compiler to load the state fields
// into registers and keep them there as long as it is useful.
#define BEFORE_EXEC() do { \
      /* set up state */ \
      accum = 0xfab28342;   /* start with trash in accum */ \
      load_ptr = &array[x][y]; \
      store_ptr = &array2[x][y]; \
  } while (0)

#define AFTER_EXEC() do { \
    /* post-loop */ \
    y++; \
    if (!(y<MAX-1)) { \
      y=1; \
      x++; \
      if (!(x<MAX-1)) { \
	/* printf ("DONE!\n"); */ \
	done=1; \
      } \
    } \
  } while (0)

void blur_instructions_translated3 (State *state)
{
  int done = 0;
  int x=state->x;
  int y=state->y;
  int accum=state->accum;
  int *load_ptr=state->load_ptr;
  int *store_ptr=state->store_ptr;
  while (!done) {
    BEFORE_EXEC();
//  OP_MOVE_REL, -1, -1,
	    x += -1;
	    y += -1;
	    load_ptr = &array[x][y];
//  OP_SET_ACCUM, 0,
	    accum = 0;
//  OP_ADD_DATA,
	    accum += *load_ptr++;
//  OP_MOVE_REL, 0, 1,
	    x += 0;
	    y += 1;
	    load_ptr = &array[x][y];
//  OP_ADD_DATA,
	    accum += *load_ptr++;
//  OP_MOVE_REL, 0, 1,
	    x += 0;
	    y += 1;
	    load_ptr = &array[x][y];
//  OP_ADD_DATA,
	    accum += *load_ptr++;
//  OP_MOVE_REL, 1, -2,
	    x += 1;
	    y += -2;
	    load_ptr = &array[x][y];
//  OP_ADD_DATA,
	    accum += *load_ptr++;
//  OP_MOVE_REL, 0, 1,
	    x += 0;
	    y += 1;
	    load_ptr = &array[x][y];
//  OP_ADD_DATA,
	    accum += *load_ptr++;
//  OP_MOVE_REL, 0, 1,
	    x += 0;
	    y += 1;
	    load_ptr = &array[x][y];
//  OP_ADD_DATA,
	    accum += *load_ptr++;
//  OP_MOVE_REL, 1, -2,
	    x += 1;
	    y += -2;
	    load_ptr = &array[x][y];
//  OP_ADD_DATA,
	    accum += *load_ptr++;
//  OP_MOVE_REL, 0, 1,
	    x += 0;
	    y += 1;
	    load_ptr = &array[x][y];
//  OP_ADD_DATA,
	    accum += *load_ptr++;
//  OP_MOVE_REL, 0, 1,
	    x += 0;
	    y += 1;
	    load_ptr = &array[x][y];
//  OP_ADD_DATA,
	    accum += *load_ptr++;
//  OP_STORE_DATA,
	    *(store_ptr++) = accum;
//  OP_MOVE_REL, -1, -1
	    x += -1;
	    y += -1;
	    load_ptr = &array[state->x][y];
//  OP_END
    AFTER_EXEC();
  }
  state->x = x;
  state->y = y;
  state->accum = accum;
  state->load_ptr = load_ptr;
  state->store_ptr = store_ptr;
}

void blur_opcode_translated3 ()
{
  State state;
  state.x = 1;
  state.y = 1;
  blur_instructions_translated3 (&state);
}

#endif // ifdef BLUR_TRANSLATED3

#ifdef BLUR_DYNAMIC_TRANSLATE1

static int unique_fn_id = 283473;

void blur_dynamic_translate1 ()
{
  // csrc is the main text file containing the translated code.
  FILE *csrc = fopen ("translate1.c", "w");
  State state;
  int done = 0;
  int *pc;
  assert (csrc!=NULL);
  fprintf (csrc, "// code generated by blur-opcode.c, blur_dynamic_translate1()\n");
  fprintf (csrc, "#include \"translate1-defs.h\"\n");
  fprintf (csrc, "void translate%d (State *state) {\n", unique_fn_id);
  fprintf (csrc, "BEGIN_TRANSLATED_FUNCTION();\n");
  pc = &blur_instructions[0];
  while (!done) {
    switch (*pc++) {
      case OP_MOVE_REL:
	fprintf (csrc, "DO_MOVE_REL(%d,%d);\n", *pc++, *pc++);
	break;
      case OP_SET_ACCUM:
	fprintf (csrc, "DO_SET_ACCUM(%d);\n", *pc++);
	break;
      case OP_ADD_DATA:
	fprintf (csrc, "DO_ADD_DATA();\n");
	break;
      case OP_SUBTRACT_DATA:
	fprintf (csrc, "DO_SUBTRACT_DATA();\n");
	break;
      case OP_MULTIPLY_DATA:
	fprintf (csrc, "DO_MULTIPLY_DATA();\n");
	break;
      case OP_STORE_DATA:
	fprintf (csrc, "DO_STORE_DATA();\n");
	break;
      case OP_END:
	done = 1;
	break;
      default:
	assert (0);
    }
  }
  fprintf (csrc, "END_TRANSLATED_FUNCTION();\n");
  fprintf (csrc, "} // end of translate%d\n", unique_fn_id);
  unique_fn_id++;
  fclose (csrc);
}
#endif

#ifdef BLUR_DYNAMIC_TRANSLATE1_TEST
extern void translate284472 (State *state);

void blur_dynamic_translate1_test ()
{
  int sum;
  int x,y,x2,y2;
  State state;
  for (x=1; x<MAX-1; x++)
    for (y=1; y<MAX-1; y++)
    {
      int *pc;
      int done = 0;
      sum = 0;
      // set up state
      state.x = x;
      state.y = y;
      state.accum = 0xfab28342;   // start with trash in accum
      state.load_ptr = &array[x][y];
      state.store_ptr = &array2[x][y];
      // call translated function
      translate284472 (&state);
    }
}
#endif

#if defined(BLUR_DYNAMIC_TRANSLATE2) || defined(BLUR_DYNAMIC_TRANSLATE3)

static int unique_fn_id = 283473;

void blur_dynamic_translate2 (char *filename)
{
  // csrc is the main text file containing the translated code.
  FILE *csrc = fopen (filename, "w");
  State state;
  int done = 0;
  int *pc;
  assert (csrc!=NULL);
  fprintf (csrc, "// code generated by blur-opcode.c, blur_dynamic_translate1()\n");
  fprintf (csrc, "#include \"translate2-defs.h\"\n");
  fprintf (csrc, "\n");
  fprintf (csrc, "void translate%d (State *state) {\n", unique_fn_id);
  // the BEGIN_TRANSLATED_FUNCTION macro is defined in the 
  // translate2-defs.h file.  It creates local variables to mirror the
  // frequently used fields in the state struct.
  fprintf (csrc, "BEGIN_TRANSLATED_FUNCTION()\n");
  // this code is executed before the block begins, each time.
  fprintf (csrc, "  ST(load_ptr) =  &array[x][y];\n");
  fprintf (csrc, "  ST(store_ptr) =  &array2[x][y];\n");
  pc = &blur_instructions[0];
  while (!done) {
    switch (*pc++) {
      case OP_MOVE_REL:
	fprintf (csrc, "DO_MOVE_REL(%d,%d);\n", *pc++, *pc++);
	break;
      case OP_SET_ACCUM:
	fprintf (csrc, "DO_SET_ACCUM(%d);\n", *pc++);
	break;
      case OP_ADD_DATA:
	fprintf (csrc, "DO_ADD_DATA();\n");
	break;
      case OP_SUBTRACT_DATA:
	fprintf (csrc, "DO_SUBTRACT_DATA();\n");
	break;
      case OP_MULTIPLY_DATA:
	fprintf (csrc, "DO_MULTIPLY_DATA();\n");
	break;
      case OP_STORE_DATA:
	fprintf (csrc, "DO_STORE_DATA();\n");
	break;
      case OP_END:
	done = 1;
	break;
      default:
	assert (0);
    }
  }

  // loop count updates
  fprintf (csrc, "  ST(y)++;                   \n");
  fprintf (csrc, "  if (!(ST(y)<MAX-1)) {      \n");
  fprintf (csrc, "    ST(y)=1;                 \n");
  fprintf (csrc, "    ST(x)++;                 \n");
  fprintf (csrc, "    if (!(ST(x)<MAX-1)) {    \n");
  fprintf (csrc, "	done=1;                \n");
  fprintf (csrc, "    }                        \n");
  fprintf (csrc, "  }                          \n");

  fprintf (csrc, "END_TRANSLATED_FUNCTION()\n");
  fprintf (csrc, "}  // end of translate%d\n", unique_fn_id);
  unique_fn_id++;
  fclose (csrc);
}
#endif

#ifdef BLUR_DYNAMIC_TRANSLATE2_TEST
extern void translate284472 (State *state);

void blur_dynamic_translate2_test ()
{
  State state;
  state.x = 1;
  state.y = 1;
  translate284472 (&state);
}
#endif

#ifdef BLUR_DYNAMIC_TRANSLATE3
#include <ltdl.h>

void (*func)(State *) = NULL;

int load_dl_and_execute ()
{
  static int first_time = 1;
  State state;
  int n;
  if (first_time) {
    struct stat st;
    first_time=0;
    fprintf (stderr, "building translation function in translate3.c\n");
    blur_dynamic_translate2 ("translate3.c");
    fprintf (stderr, "compiling translate3.c\n");
    if (system ("./buildshared translate3") < 0) {
      fprintf (stderr, "buildshared failed\n");
      exit (1);
    }
    if (stat ("translate3.c", &st) < 0) {
      fprintf (stderr, "stat failed\n");
      exit (1);
    }
    fprintf (stderr, "shared library is %d bytes\n", (int)st.st_size);
    fprintf (stderr, "opening libtranslate3.la\n");
    if (lt_dlinit () != 0) {
      fprintf (stderr, "lt_dlinit() failed\n");
      exit (1);
    }
    if (!func) {
      char *filename = "libtranslate3.la";
      lt_dlhandle handle;
      handle = lt_dlopen (filename);
      if (!handle) {
	fprintf (stderr, "can't open the module %s!\n", filename);
	fprintf (stderr, "error was: %s\n", lt_dlerror());
	exit (1);
      }
      func = (void (*)(State *)) lt_dlsym(handle, "translate283473");
      if (!func) {
	fprintf (stderr, "can't find translate283473()\n");
	exit (1);
      }
    }
    fprintf (stderr, "Ready to use translated function.\n");
  }
  state.x = 1;
  state.y = 1;
  (*func)(&state);
}
#endif

void fill_array()
{
  int x,y;
  for (x=0; x<MAX; x++)
    for (y=0; y<MAX; y++)
      array[x][y] = (x*17+y*31)%29;
}

void dump_array (FILE *fp, int ptr[MAX][MAX])
{
  int x,y;
  for (x=0; x<MAX; x++) {
    for (y=0; y<MAX; y++) {
      fprintf (fp, "%3d ", ptr[x][y]);
    }
    fprintf (fp, "\n");
  }
}

struct timeval start, stop;
#define start_timer() gettimeofday (&start, NULL);
#define stop_timer() gettimeofday (&stop, NULL);

void report_time (FILE *fp, int iters)
{
  int usec_duration = 
    (stop.tv_sec*1000000 + stop.tv_usec)
    - (start.tv_sec*1000000 + start.tv_usec);
  double sec = (double)usec_duration / 1.0e3;
  double sec_per_iter = sec / (double)iters;
  fprintf (fp, "Total time elapsed = %f msec\n", sec);
  fprintf (fp, "Iterations = %d\n", iters);
  fprintf (fp, "Time per iteration = %f msec\n", sec_per_iter);
}

int main (int argc, char *argv[])
{
  int i;
  int times = 0;
  FILE *out;
  if (argc>1) {
    assert (sscanf (argv[1], "%d", &times) == 1);
  } else {
    times = DEFAULT_TIMES;
  }
  fill_array();
  //dump_array (stderr);
  start_timer();
  for (i=0; i<times; i++)  {
#if defined BLUR_USE_SWITCH
    blur_opcode_switch();
#elif defined BLUR_USE_FNPTR
    blur_opcode_fnptr();
#elif defined BLUR_TRANSLATED1
    blur_opcode_translated();
#elif defined BLUR_TRANSLATED2
    blur_opcode_translated2();
#elif defined BLUR_TRANSLATED3
    blur_opcode_translated3();
#elif defined BLUR_DYNAMIC_TRANSLATE1
    blur_dynamic_translate1 ();
#elif defined BLUR_DYNAMIC_TRANSLATE2
    blur_dynamic_translate2 ("translate2.c");
#elif defined BLUR_DYNAMIC_TRANSLATE3
    load_dl_and_execute ("translate2");
#elif defined BLUR_DYNAMIC_TRANSLATE1_TEST
    blur_dynamic_translate1_test ();
#elif defined BLUR_DYNAMIC_TRANSLATE2_TEST
    blur_dynamic_translate2_test ();
#else
    blur_simple();
#endif
  }
  stop_timer();
  report_time (stdout, times);
  //fprintf (stderr, "-----------------------------------\n");
  out = fopen ("blur.out", "w");
  assert (out != NULL);
  dump_array (out, array2);
  fclose (out);
  return 0;
}
