//
// snprintf.cc
// 
// Provide definition of snprintf, since it's sometimes not in libc.
// This could have been a .c file, but then you have to do all your
// includes conditional on each OS having them.  Easier to just let
// bochs.h do it.
//

#include "bochs.h"

#if !BX_HAVE_SNPRINTF
/* if they don't have snprintf, just use sprintf */
int bx_snprintf (char *s, size_t maxlen, const char *format, ...)
{
  va_list arg;
  int done;

  va_start (arg, format);
  done = vsprintf (s, format, arg);
  va_end (arg);

  return done;
}
#endif  /* !BX_HAVE_SNPRINTF */
