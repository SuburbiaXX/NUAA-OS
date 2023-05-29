#include <utils/root.h>
#include <mm/vm.h>
#include <kernel/pcb.h>
#include <kernel/errno.h>
#include <kernel/file.h>
#include <dev/number.h>
#include "fs.h"
#include "stat.h"

extern file_class_t regular_file_class;

static void open_file(file_t *file, inode_t *inode)
{
    switch (inode->mode & S_IFMT) {
        case S_IFREG:
        case S_IFDIR:
            file->class = &regular_file_class;
            break;

        case S_IFBLK:
        case S_IFCHR:
            open_device_file(inode->real_device, file);
            break;
    }
}

int sys_open(char *path, int flags, mode_t mode)
{
    int error;
    int fd;
    inode_t *inode;
    file_t *file;

    if (error = verify_path(path))
        return error;
    if (error = alloc_fd(&fd))
        return error;
    if (error = namei_open(path, &inode))
        return error;

    file = file_new(FILE_INODE, flags, inode);
    if (!file) {
        inode_free(inode);
        return ENOMEM;
    }
    open_file(file, inode);
    install_file(fd, file);
    return fd;
}

int sys_creat(char *path, mode_t mode)
{
    int error;
    int fd;
    inode_t *dir, *inode;
    file_t *file;
    char base_name[NAME_MAX];

    if (error = verify_path(path))
        return error;
    if (error = alloc_fd(&fd))
        return error;

    if (error = namei_make(path, &dir, base_name))
        return error;
    error = dir_make_regular(dir, base_name, &inode);
    switch (error) {
        case 0:
            inode_free(dir);
            break;

        case EEXIST:
            inode = dir_lookup_inode(dir, base_name);
            assert(inode);
            inode_trunc(inode);
            inode_free(dir);
            break;

        default:
            inode_free(dir);
            return error;
    }

    file = file_new(FILE_INODE, O_WRONLY, inode);
    if (!file) {
        inode_free(inode);
        return ENOMEM;
    }
    open_file(file, inode);
    install_file(fd, file);
    return fd;
}

int sys_umask()
{
    return 0;
}

int sys_stat(char *path, struct stat *stat)
{
    int error;
    inode_t *inode;

    if (error = verify_write(stat, sizeof(struct stat)))
        return error;
    if (error = namei_open(path, &inode))
        return error;
 
    stat->st_dev = inode->device;
    stat->st_ino = inode->ino;
    stat->st_mode = inode->mode;
    stat->st_nlink = inode->link_count;
    stat->st_uid = inode->uid;
    stat->st_gid = inode->gid;
    stat->st_rdev = inode->real_device;
    stat->st_size = inode->size;
    inode_free(inode);
    return 0;
}

int sys_chown(char *path, uid_t uid, gid_t gid)
{
    int error;
    inode_t *inode;

    if (error = verify_path(path))
        return error;
    if (error = namei_open(path, &inode))
        return error;
    if (!super_user())
        raise(EPERM);

    inode->uid = uid;
    inode->gid = gid;

bad:inode_free(inode);
    return error;
}

int sys_chmod(char *path, mode_t mode)
{
    int error;
    inode_t *inode;
    mode_t type;

    if (error = verify_path(path))
        return error;
    if (error = namei_open(path, &inode))
        return error;
    if (this_pcb->euid != inode->uid && !super_user())
        raise(EPERM);

    type = inode->mode & S_IFMT;
    inode->mode = (mode & 07777) | type;

bad:inode_free(inode);
    return error;
}

int sys_mkdir(char *path)
{
    int error;
    inode_t *dir;
    inode_t *leaf;
    char base_name[NAME_MAX];
 
    if (error = verify_path(path))
        return error;

    if (error = namei_make(path, &dir, base_name))
        return error;
    if (error = dir_make_dir(dir, base_name, &leaf)) {
        inode_free(dir);
        return error;
    }

    inode_free(dir);
    inode_free(leaf);
    return 0;
}

int sys_mknod()
{
    return 0;
}

int sys_rmdir()
{
    return 0;
}

int sys_link()
{
    return 0;
}

int sys_unlink(char *path)
{
    int error;
    char base_name[NAME_MAX];
    inode_t *dir;

    if (error = verify_path(path))
        return error;
    if (error = namei_make(path, &dir, base_name))
        return error;
    return dir_remove_entry(dir, base_name);
}

int sys_rename()
{
    return 0;
}

int sys_chroot(char *path)
{
    int error;
    inode_t *inode;

    if (!super_user())
        return EPERM;
    if (error = verify_path(path))
        return error;
    if (error = namei_open(path, &inode))
        return error;
    if (S_ISDIR(inode->mode)) {
        inode_free(inode);
        return ENOTDIR;
    }

    inode_free(this_pcb->root_dir);
    this_pcb->root_dir = inode;
    return 0;
}

int sys_chdir(char *path)
{
    int error;
    inode_t *inode;

    if (error = verify_path(path))
        return error;
    if (error = namei_open(path, &inode))
        return error;
    if (!S_ISDIR(inode->mode)) {
        inode_free(inode);
        return ENOTDIR;
    }

    inode_free(this_pcb->work_dir);
    this_pcb->work_dir = inode;
    return 0;
}

int sys_getcwd(char *buffer, int size)
{
    int error;
    if (error = verify_write(buffer, size))
        return error;

    if (this_pcb->work_dir == this_pcb->root_dir) {
        strcpy(buffer, "/");
        return 0;
    }

    char *p = buffer + size - 1;
    *p = 0;

    inode_t *child = this_pcb->work_dir;
    inode_hold(child);
    while (child != this_pcb->root_dir) {
        inode_t *parent = dir_lookup_inode(child, "..");

        dir_entry_t dir_entry;
        int found = dir_lookup_ino(parent, child->ino, &dir_entry);
        assert(found);

        int len = strlen(dir_entry.name);
        p -= len;
        memcpy(p, dir_entry.name, len);
        p -= 1;
        *p = '/'; 

        inode_free(child);
        child = parent;
    }
    inode_free(child);

    strcpy(buffer, p);
    return 0;
}
