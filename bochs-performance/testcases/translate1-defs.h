typedef struct {
  int x, y;
  int accum;
  int *load_ptr;
  int *store_ptr;
  int done;
} State;

#define MAX 128
extern int array[MAX][MAX];
extern int array2[MAX][MAX];

//#define ST(n) (state->n)
#define ST(n) (n)

#define BEGIN_TRANSLATED_FUNCTION() \
  int x=state->x, y=state->y, accum=state->accum; \
  int *load_ptr=state->load_ptr, *store_ptr=state->store_ptr;

#define END_TRANSLATED_FUNCTION() \
  do { \
    state->x=x; state->y=y; state->accum=accum; \
    state->load_ptr=load_ptr; state->store_ptr=store_ptr; \
  } while (0)


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
#define DO_END() ST(done) = 1
