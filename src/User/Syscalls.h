#ifndef SYSCALLS_H
#define SYSCALLS_H

#include"../utility.h"

/**
   @brief This file contains all syscalls in order

   The real implementation may be elsewhere

   In the documentation, a return value of ESOMETHING is be understood as
   following:
   The system call will return the opposite of the error code,
   Then the C handler will return -1 and set errno to this value.
 */

/// Register all syscalls in the handlers table.
void syscallFill();

/**
   @brief Syscall 0, read : read bytes from file descriptor.
   @param fd The file descriptor to be read from.
   @param buf The buffer on which data will be written.
   @param count The size of the data to be read.
   @return The number of bytes read. If it is 0, it means EOF (if their is
   nothing to be read but it is not EOF, the call to read is blocking).
   @retval EFAULT buf is outside user memory.
   @retval EBADF The file descriptor can't be read from

 */
u64 sysread(u64 fd, u64 buf, u64 count, u64,u64,u64);

/**
   @brief Syscall 1, write : write bytes to file descriptor.
   @param fd The file descriptor to be written to.
   @param buf The buffer from which data will be read.
   @param count The size of the data to be written.
   @return The number of bytes write. If it is 0, nothing more can be written.
   @retval EFAULT buf is outside user memory.
   @retval EBADF The file descriptor can't be written to.
 */
u64 syswrite(u64 fd, u64 buf, u64 count, u64,u64,u64);

/**
   @brief Syscall 2, open : open a file.
   @param path The path of the file
   @param flags The opening flags.
        - O_RDONLY : open in read-only mode
        - O_WRONLY : open in write_only mode
        - O_RDWR : open in read write mode
        - O_CREAT : create file if it does not exists
        - O_TRUNC : empty file on opening (currently not supported).
        - O_APPEND : write only after the end (currently not supported)

   Exactly one of the first 3 flags must be set.

   @return The file descriptor to the opened file on success.
   @retval EFAULT path is outside user memory.
   @retval EACCESS file pointed by path does exists or cannot be open in this mode.
*/
u64 sysopen(u64 path, u64 flags, u64,u64,u64,u64);

/**
   @brief Syscall 3, close : close a file descriptor.
   @param fd The file descriptor to be closed.
   Close can currently close anything, windows, directory, files (the two first
   cannot currently be opened from user mode)
   @return 0 on success.
   @retval EBADF The file descriptor was not opened.
*/
u64 sysclose(u64 fd, u64,u64,u64,u64,u64);


/**
   @brief Syscall 7, poll : poll a set of file descriptor.

   @todo implement it.
*/
u64 syspoll(u64 pollfd, u64 nbfds, u64,u64,u64,u64);

/**
   @brief Syscall 8, seek : Change pos in a file descriptor.
   @param fd The file descriptor of which change the offset.
   @param offset The offset to add to the position given by mode.
   @param whence The starting position, it can be :
       - SEEK_SET : From the beginning of the file.
       - SEEK_CUR : From the current position.
       - SEEK_END : From the end position.

   Some @ref SEEKABLE devices are not seekable beyond the end
   (i.e @ref Stream::APPENDABLE)
   (window, block devices, file open in read only mode).
   A seek beyond the end will fail with EINVAL for those file.

   @return The positions seeked on success.
   @retval EBADF : The descriptor is invalid or is not seekable (pipe, directory, ...)
   @retval EINVAL : whence is not valid. new offset is negative or beyond
   end of non @ref APPENDABLE device.
*/
u64 sysseek(u64 fd, u64 offset, u64 whence ,u64,u64,u64);

/**
   @brief Syscall 12, brk : move the position of user's brk.
 */
u64 sysbrk(u64 addr, u64,u64,u64,u64,u64);

/**
   @brief Syscall 22, pipe : Create an anonymous pipe.
   @param fd2 (int*) : Table of integer of size 2 : the read end will be in
   fd2[0] and the write end will be in fd[1].
   @retval 0 success
   @retval EFAULT The pointer fd2 is not valid.
*/
u64 syspipe(u64 fd2, u64,u64,u64,u64,u64);

/**
   @brief Syscall 24, openwin : create a new window.
   @param size The width of the new window on the lower 32 bits
   and the height on the higher 32 bits.
   @param offset The horizontal offset of the new window on the lower 32 bits
   and the vertical offset on the higher 32 bits
   @param workspace The workspace on which to be created

   The window can then be written on by like a file of size
   size.x * size.y *4 (each pixel is on 4 bytes and in format BGRA (A stands
   for Alpha ans is ignored)).

   @return The file descriptor of the window on success
   @retval EINVAL The rectangle is outside the screen or workspace is not valid.
 */
u64 sysopenwin(u64 size, u64 offset, u64 workspace, u64,u64,u64);

/**
   @brief Syscall 25, openwin : create a new text window.
   @param size The width of the new window on the lower 32 bits
   and the height on the higher 32 bits.
   @param offset The horizontal offset of the new window on the lower 32 bits
   and the vertical offset on the higher 32 bits

   The window can then be textually written on like a console.
   The only possible input is console text (it is not possible to get window
   size currently. May be through escape code later).

   @param workspace The workspace on which to be created
   @return The file descriptor of the window on success
   @retval EINVAL The rectangle is outside the screen or workspace is not valid.
*/
u64 sysopentwin(u64 size, u64 offset, u64 workspace, u64,u64,u64);

/**
   @brief Syscall 26, resizewin : resize an already existing window.
   @param fd The window to be resized.
   @param size The new width of the window on the lower 32 bits
   and the new height on the higher 32 bits.
   @param offset The new horizontal offset of the window on the lower 32 bits
   and the new vertical offset on the higher 32 bits

   Apply to both text and graphical windows.

   @return 0 on success
   @return EBADF fd is not a window file descriptor.
   @retval EINVAL The rectangle is outside the screen.
*/
u64 sysresizewin(u64 fd, u64 size, u64 offset, u64,u64,u64);

/**
   @brief Syscall 27, getsize : get the size of a window.
   @param fd The window of with we want to know the size.

   Apply to both text and graphical windows.

   @return a @ref vec_t : the size.
   @return EBADF fd is not a window file descriptor.
*/
u64 sysgetsize(u64 fd, u64, u64, u64,u64,u64);

/**
   @brief Syscall 28, getoff : get the offset of a window.
   @param fd The window of with we want to know the offset.

   Apply to both text and graphical windows.

   @return a @ref vec_t : the offset.
   @return EBADF fd is not a window file descriptor.
*/
u64 sysgetoff(u64 fd, u64, u64, u64,u64,u64);

/**
   @brief Syscall 29, getws : get the workspace of a window.
   @param fd The window of with we want to know the workspace.

   Apply to both text and graphical windows.

   @return The number of the workspace on success.
   @return EBADF fd is not a window file descriptor.
*/
u64 sysgetws(u64 fd, u64, u64, u64,u64,u64);

/**
   @brief Syscall 30, getevt : Get and pop last event of a graphical window.
   @param fd The window of with we want to pop an event

   The window has FIFO for event, when an event happen, it is pushed on
   the windows FIFO.

   @return The event on success. (event may be of type invalid if FIFO is empty)
   @return EBADF fd is not a graphical window file descriptor.
*/
u64 sysgetevt(u64 fd, u64, u64, u64,u64,u64);




/**
   @brief Syscall 32, dup : Duplicate a file descriptor;
   @param oldfd The file descriptor to be copied.
   Copy oldfd to a new, free file descriptor
   @return The new file descriptor on success.
   @retval EBADF oldfd does not exist.
*/
u64 sysdup(u64 oldfd, u64,u64,u64,u64,u64);

/**
   @brief Syscall 33, dup2 : Duplicate a file descriptor;
   @param oldfd The file descriptor to be copied
   @param newfd The file descriptor to be closed and overwritten by oldfd

   @return newfd on success.
   @retval EBADF oldfd does not exist.

*/
u64 sysdup2(u64 oldfd, u64 newfd, u64,u64,u64,u64);

/**
   @brief Syscall 56, clone : create a new thread in the current process.
   @param rip The program counter to be jumped to.
   @param rsp The stack to the new thread.
   Create a new thread in the current process whose stack starts at rsp and
   code starts at rip. it is the user responsibility that those value are valid.
   @return The tid of the created thread.
   @retval EFAULT rip or rsp is outside user address space.
 */
u64 sysclone(u64 rip, u64 rsp, u64,u64,u64,u64);

/**
   @brief Syscall 57, fork : create a new process, a copy from the current one.
   All memory, file descriptors are copied, the new child process starts with only one thread,
   a copy os the one which called fork.
   @return the new pid to the parent and 0 to the child.
 */
u64 sysfork(u64,u64,u64,u64,u64,u64);

/**
   @brief Syscall 58, exit : exit the current process with code rc
   @param rc the return code of the process.
   This system call cannot fail, memory and file descriptors are immediately released.
   Process place in process hierarchy will disappear when waited.

   @return exit never returns.
 */
u64 sysexit(u64 rc, u64,u64,u64,u64,u64);

/**
   @brief Syscall 57, exec : replace current process by a new one.
   @param path path to binary file.
   @param argv arguments : this must be a table of string ended by a nullptr.

   @return exec does not return on success.
   @retval EFAULT argv or one of its member is not in user memory.
   @retval EACCESS path can't be accessed
 */
u64 sysexec(u64 upath, u64 argv, u64,u64,u64,u64);

/**
   @brief Syscall 60, texit : exit the current thread with code rc
   @param rc the return code of the thread.
   This system call cannot fail, Process die when its last thread dies
   with the return code of its main thread.
   @return texit never returns.

 */
u64 systexit(u64 rc, u64,u64,u64,u64,u64);

/**
   @brief Syscall 61, wait : wait for a child process to die.
   @param pid : The pid of the waited child process, 0 means all.
   @param status if this is a non-zero pointer, the return code of the child will be put there.
   @return The pid of the waited process.
   @retval ECHILD : If there is no child process.
 */
u64 syswait(u64 pid, u64 status, u64,u64,u64,u64);

/**
   @brief Syscall 72, opend : open directory
   @param path : path to the directory
   @retval file descriptor
   @retval EFAULT : path is not in usermode memory;
   @retval EACCESS : path resolution failed.
   @retval ENOTDIR : object pointed to by path is not a directory.
*/
u64 sysopend(u64 path, u64,u64,u64,u64,u64);

/**
   @brief Syscall 73, readd : read directory
   @param fd : the file descriptor
   @param thedirent : pointer to dirent structure to fill
   @retval 0 : success
   @retval EFAULT : thedirent is not in usermode memory;
   @retval EBADF : bad file descriptor
*/
u64 sysreadd(u64 fd, u64 thedirent,u64,u64,u64,u64);

/**
   @brief Syscall 80, chdir : change directory
   @param path : path to new directory
   @retval 0 Success;
   @retval EFAULT : path is not in usermode memory;
   @retval EACCESS : path resolution failed.
   @retval ENOTDIR : object pointed to by path is not a directory.
*/
u64 syschdir(u64 path, u64,u64,u64,u64,u64);

/**
   @brief Syscall 82, rename : rename files
   @param path1 : the current path
   @param path2 : the new path
   @retval 0 Success;
   @retval EFAULT : path is not in usermode memory;
   @retval EACCESS : path resolution failed.
   @retval EXDEV : @ref path1 and @ref path2 are on different partitions
   @retval EEXIST : @ref path2 already exists
*/
u64 sysrename(u64 path1, u64 path2,u64,u64,u64,u64);

/**
   @brief Syscall 83, mkdir : create directory.
   @param path : path to new directory
   @retval 0 Success;
   @retval EFAULT : path is not in usermode memory;
   @retval EACCESS : path resolution failed.
   @retval EEXIST : path already exists.
*/
u64 sysmkdir(u64 path, u64,u64,u64,u64,u64);

/**
   @brief Syscall 84, rmdir : remove directory.
   @param path : path to directory to be deleted
   @retval 0 Success;
   @retval EFAULT : path is not in usermode memory;
   @retval EACCESS : path resolution failed.
   @retval EEXIST : directory is not empty
*/
u64 sysrmdir(u64 path, u64,u64,u64,u64,u64);

/**
   @brief Syscall 86, link : create hard links.
   @param path1 : the existing file
   @param path2 : the link to be created
   @retval 0 Success;
   @retval EFAULT : path is not in usermode memory;
   @retval EACCESS : path resolution failed.
   @retval EXDEV : @ref path1 and @ref path2 are on different partitions
   @retval EEXIST : @ref path2 already exists
*/
u64 syslink(u64 path1, u64 path2,u64,u64,u64,u64);

/**
   @brief Syscall 87, unlink : remove link
   @param path : the path
   @retval 0 Success;
   @retval EFAULT : path is not in usermode memory;
   @retval EACCESS : path resolution failed.
   @retval EXDEV : @ref path1 and @ref path2 are on different partitions
   @retval EEXIST : @ref path2 already exists
*/
u64 sysunlink(u64 path, u64,u64,u64,u64,u64);


#endif
