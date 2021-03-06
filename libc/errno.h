#ifndef __SUPOS_ERRNO_H
#define __SUPOS_ERRNO_H

/**
   @brief error number of standard library.

   @todo thread specific version.
 */
extern int errno;

// #define errors numbers
#define EBADF 1
#define ECHILD 2
#define EPIPE 3 ///< When writing on a closed pipe
#define EACCESS 4 ///< When accessing an file that do not exists
#define EFAULT 5 ///< user give a pointer to kernel space
#define ENOMEM 6 ///< when no more heap memory is available
#define EBLOCK 7 ///< Internal error code, normally should not arrive to user mode
#define EINVAL 8 ///< Invalid value (parameter)
#define ENOTDIR 9 ///< Not a directory.
#define EEXIST 10 ///< Object already exists.
#define EXDEV 11 ///< Objects are not on the same partition


#ifdef __cplusplus
extern "C" {
#endif

    void perror(const char *s);

#ifdef __cplusplus
}
#endif


#endif
