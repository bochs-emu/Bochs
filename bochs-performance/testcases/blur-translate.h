#define MAX_ARRAY 128
extern int array[MAX_ARRAY][MAX_ARRAY];
extern int array2[MAX_ARRAY][MAX_ARRAY];

typedef struct {
  int x, y;
  int accum;
  int *load_ptr;
  int *store_ptr;
  int done;
} State;

#if defined(IN_TRANSLATED_CODE)

#define ST(n) (n)

#define BEGIN_TRANSLATED_FUNCTION() \
  int x=state->x, y=state->y, accum=state->accum; \
  int *load_ptr=state->load_ptr, *store_ptr=state->store_ptr; \
  int done = 0; \
  while (!done) {


#define END_TRANSLATED_FUNCTION() \
  } /* end of while block started in BEGIN_TRANSLATED_FUNCTION() */ \
  state->x=x; state->y=y; state->accum=accum; \
  state->load_ptr=load_ptr; state->store_ptr=store_ptr;

#endif // defined IN_TRANSLATED_CODE


#define DO_MOVE_REL(delta_x,delta_y) do { \
	    ST(x) += delta_x; \
	    ST(y) += delta_y; \
	    ST(load_ptr) = &array[ST(x)][ST(y)]; \
  } while (0)
#define DO_SET_ACCUM(x) ST(accum) = x
#define DO_ADD_DATA() ST(accum) += *ST(load_ptr)
#define DO_SUBTRACT_DATA() ST(accum) -= *ST(load_ptr)
#define DO_MULTIPLY_DATA() ST(accum) *= *ST(load_ptr)
#define DO_STORE_DATA() *ST(store_ptr) = ST(accum)
#define DO_END() done = 1


