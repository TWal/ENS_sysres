#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include "Partition.h"
#include <vector>
#include <string>
#include "../Streams/Stream.h"
#include <memory>

//TODO: move this in libc?
struct timespec {
    u32 tv_sec;  ///< seconds
    u32 tv_nsec; ///< nanoseconds
};

/**
    @brief Represents a directory entry

    It is used by Directory::read
*/
struct dirent {
    u32 d_ino;        ///< inode
    char d_name[256]; ///< name
};

/**
    @brief Represents the status of a file/directory

    It is used in File::getStatus
*/
struct stat {
    u32    st_dev;     ///< ID of device containing file
    u32    st_ino;     ///< inode number
    u16    st_mode;    ///< file type and mode
    u16    st_nlink;   ///< number of hard links
    u16    st_uid;     ///< user ID of owner
    u16    st_gid;     ///< group ID of owner
    u32    st_rde;     ///< device ID (if special file) //TODO: what is this?
    size_t st_size;    ///< total size, in bytes
    u32    st_blksize; ///< blocksize for filesystem I/O
    u32    st_blocks;  ///< number of 512B blocks allocated

    timespec st_atim;  ///< time of last access
    timespec st_mtim;  ///< time of last modification
    timespec st_ctim;  ///< time of last status change
    // Backward compatibility
#define st_atime st_atim.tv_sec
#define st_mtime st_mtim.tv_sec
#define st_ctime st_ctim.tv_sec
};

enum FileMode {
    S_IFMT = 0xF000,   ///< format mask
    S_IFSOCK = 0xA000, ///< socket
    S_IFLNK = 0xC000,  ///< symbolic link
    S_IFREG = 0x8000,  ///< regular file
    S_IFBLK = 0x6000,  ///< block device
    S_IFDIR = 0x4000,  ///< directory
    S_IFCHR = 0x2000,  ///< character device
    S_IFIFO = 0x1000,  ///< fifo

    S_ISUID = 0x0800,  ///< SUID
    S_ISGID = 0x0400,  ///< SGID
    S_ISVTX = 0x0200,  ///< sticky bit

    S_IRWXU = 0x01C0,  ///< user mask
    S_IRUSR = 0x0100,  ///< read
    S_IWUSR = 0x0080,  ///< write
    S_IXUSR = 0x0040,  ///< execute

    S_IRWXG = 0x0038,  ///< group mask
    S_IRGRP = 0x0020,  ///< read
    S_IWGRP = 0x0010,  ///< write
    S_IXGRP = 0x0008,  ///< execute

    S_IRWXO = 0x0007,  ///< other mask
    S_IROTH = 0x0004,  ///< read
    S_IWOTH = 0x0002,  ///< write
    S_IXOTH = 0x0001   ///< execute
};

#define S_ISREG(m)  (((m) & S_IFMT) == S_IFREG)
#define S_ISDIR(m)  (((m) & S_IFMT) == S_IFDIR)
#define S_ISCHR(m)  (((m) & S_IFMT) == S_IFCHR)
#define S_ISBLK(m)  (((m) & S_IFMT) == S_IFBLK)
#define S_ISFIFO(m) (((m) & S_IFMT) == S_IFIFO)
#define S_ISLNK(m)  (((m) & S_IFMT) == S_IFLNK)
#define S_ISSOCK(m) (((m) & S_IFMT) == S_IFSOCK)

namespace HDD {

enum class FileType {
    RegularFile, Directory, BlockDevice, CharacterDevice
};

/**
    @brief Represents a file / directory / ... in the filesystem
*/
class File {
    public:
        virtual ~File(){}
        /// Returns the type of the file
        virtual FileType getType() const = 0;

        /// get status of the file
        virtual void getStats(stat* buf) const = 0;
        /// get inode number using getStats
        virtual u32 getInode() const;
};

/**
    @brief Represents a regular in the filesystem
*/
class RegularFile : public File, public HDDBytes {
    public:
        virtual FileType getType() const;
        /// Resize the file
        virtual void resize(size_t size) = 0;
        /// A regular file is appendable.
        bool appendable(){return true;}
};

/**
    @brief Represents a directory in the filesystem
*/
class Directory : public File {
    public:
        virtual FileType getType() const;
        ///Get a file in the directory. Returns nullptr when it does not exists
        virtual std::unique_ptr<File> operator[](const std::string& name) = 0;
        ///Get the file at `path`
        std::unique_ptr<File> resolvePath(const std::string& path);

        virtual void* open() = 0; ///< Like opendir
        virtual dirent* read(void* d) = 0; ///< Like readdir
        virtual long int tell(void* d) = 0; ///< Like telldir
        virtual void seek(void* d, long int loc) = 0; ///< Like seekdir
        virtual void close(void* d) = 0; ///< Like closedir

        ///Add an entry in the directory, creating the file/directory
        virtual std::unique_ptr<File> addEntry(const std::string& name, u16 uid, u16 gid, u16 mode) = 0;
        ///Add an existing entry in the directory (it may be used to create hard links)
        virtual void addEntry(const std::string& name, File* file) = 0;
        ///Remove and delete file in a directory
        virtual void removeFile(const std::string& name) = 0;
        ///Remove and delete directory in a directory
        virtual void removeDirectory(const std::string& name) = 0;
        ///Remove an entry. This is the reciprocal of addEntry(const std::string&, File*)
        virtual void removeEntry(const std::string& name) = 0;

        ///Check if the directory is empty
        virtual bool isEmpty();
};

/**
    @brief Represents a block device in the filesystem
*/
class BlockDevice : public File, public HDDBytes {
    public:
        virtual FileType getType() const;
};

/**
    @brief Represents a character device in the filesystem
*/
class CharacterDevice : public File, public Stream {
    public:
        virtual FileType getType() const;
};

/**
    @brief Represents a filesystem

    This may be some filesystem implementation (FAT, Ext2), some special filesystem (procfs, devfs),
    or the implementation of a VFS
*/
class FileSystem {
    public:
        virtual std::unique_ptr<Directory> getRoot() = 0;
};

} //end of namespace HDD

#endif
