typedef struct {
  unsigned long dr0;
  unsigned long dr1;
  unsigned long dr2;
  unsigned long dr3;
  unsigned long dr6;
  unsigned long dr7;
  } bx_loader_misc_t;

extern bx_loader_misc_t *bx_loader_misc_ptr;
