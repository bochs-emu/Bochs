#ifndef _LINUX_LINKAGE_H
#define _LINUX_LINKAGE_H

#ifdef __cplusplus
#define CPP_ASMLINKAGE extern "C"
#else
#define CPP_ASMLINKAGE
#endif

#if defined __i386__ && (__GNUC__ > 2 || __GNUC_MINOR__ > 7)
#define asmlinkage CPP_ASMLINKAGE __attribute__((regparm(0)))
#else
#define asmlinkage CPP_ASMLINKAGE
#endif

#define SYMBOL_NAME_STR(X) #X
#define SYMBOL_NAME(X) X
#ifdef __STDC__
#define SYMBOL_NAME_LABEL(X) X##:
#else
#define SYMBOL_NAME_LABEL(X) X/**/:
#endif

#ifdef __arm__
#define __ALIGN .align 0
#define __ALIGN_STR ".align 0"
#else
#ifdef __mc68000__
#define __ALIGN .align 4
#define __ALIGN_STR ".align 4"
#else
#if !defined(__i486__) && !defined(__i586__)
#define __ALIGN .align 4,0x90
#define __ALIGN_STR ".align 4,0x90"
#else  /* __i486__/__i586__ */
#define __ALIGN .align 16,0x90
#define __ALIGN_STR ".align 16,0x90"
#endif /* __i486__/__i586__ */
#endif /* __mc68000__ */
#endif /* __arm__ */

#ifdef __ASSEMBLY__

#define ALIGN __ALIGN
#define ALIGN_STR __ALIGN_STR

#define ENTRY(name) \
  .globl SYMBOL_NAME(name); \
  ALIGN; \
  SYMBOL_NAME_LABEL(name)

#endif

#endif
