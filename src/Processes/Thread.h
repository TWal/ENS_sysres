#ifndef THREAD_H
#define THREAD_H



#include "Process.h"

/**
   @brief Represents a thread of execution

   A thread is member of a process (@ref _process) but have its own processor
   state in  @ref context.

 */

class Thread : public Waiting {
    friend Process;
    u16 _tid;
    u16 _pid; /// Must be equal to _process->getPid();
    u16 _gid;
public :
    u16 _uid;
private :
    Process* _process; /// Process of the thread, redundant with _pid for speed.
    void* _mallocMMX;
    void* _MMX;
    bool first;
public :
    Context context;
    Thread(u16 tid,u64 rip,Process* process);
    ~Thread();
    [[noreturn]] void run(); /// Launch the thread until the next timer interruption.
    u16 getTid()const{return _tid;}
    u16 getPid()const{return _pid;}
    u16 getGid()const{return _gid;}
    bool isMain()const{return _tid == _pid;}
    /// Close the thread, returnCode is ignored if this thread is not the main thread.
    void terminate(u64 returnCode);
    Process* getProcess(){return _process;}
    /// Execute the wait system call on any child process.
    u64 waitp(u64* status);
    /// Execute the wait system call on the given child process.
    u64 waitp(Process* pro, u64* status);
    /// Same as waitp but return -EBLOCK instead of blocking the thread
    u64 nbwaitp(u64* status);
    /// Same as waitp but return -EBLOCK instead of blocking the thread
    u64 nbwaitp(Process* pro, u64* status);
    /// push the data on the stack of the thread (process memory must be activated);
    void* push(const void*data, size_t size);
    /// Save Current MMX state in this thread.
    void saveMMX();
    /// Load Current MMX state from this thread.
    void loadMMX();
};



#endif
