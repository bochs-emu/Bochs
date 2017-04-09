#define NO_ASSEMBLER
#define USE_WITH_CPU_SIM
#define PARANOID

#define fileno(A) 0
#ifdef __cplusplus
#include <cstddef>
inline long read(int fd, void *buf, std::size_t nbytes);
long read(int fd, void *buf, std::size_t nbytes) {return read(fd, (char*)buf, nbytes);}
inline long write(int fd, const void *buf, std::size_t nbytes);
long write(int fd, const void *buf, std::size_t nbytes) {return write(fd, (const char*)buf, nbytes);}
#endif
