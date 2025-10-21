//
// Created by yael on 10/21/25.
//
#include "tosfs.h"
#include <sys/mman.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>

#include "tosfs.h"
void display_inode(struct tosfs_inode* pInode,__u32 inodesnmbr);


int main(void) {

    int file_descriptor = open("test_tosfs_files", O_RDONLY);
    printf("errno: %d\n", errno);
    printf("File descriptor is %d\n", file_descriptor);
    void* file_pointer = mmap(0,40960,PROT_READ,MAP_SHARED,file_descriptor,0);

    struct tosfs_superblock* superblock_pointer =(struct tosfs_superblock*) file_pointer;
    if (superblock_pointer->magic !=TOSFS_MAGIC) {
        perror("magic");
        munmap(superblock_pointer,sizeof(struct tosfs_superblock));
    }
    struct tosfs_inode* inode_pointer =(struct tosfs_inode*) file_pointer + TOSFS_BLOCK_SIZE;

    display_inode(inode_pointer,superblock_pointer->inodes);

}

void display_inode(struct tosfs_inode* pInode,__u32 inodesnmbr) {
    printf("%i\n",(int)inodesnmbr);
    while(inodesnmbr > 0) {
        printf("=======================\n");
        printf("inodenumber:%u\n",pInode->inode);
        printf("blocknumber::%u\n",pInode->block_no);
        printf("======================\n");
        inodesnmbr--;
        pInode+=TOSFS_INODE_SIZE;
    }
}