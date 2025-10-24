//
// Created by yael on 10/21/25.
//
#include "tosfs.h"
#include <sys/mman.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/stat.h>
#define SYSTEM_CALL_ERROR (-1)
#include "tosfs.h"
void display_inode(struct tosfs_inode* pInode,__u32 inodesnmbr);
void display_superblock(struct tosfs_superblock * superblock_pointer);
void display_root_block(struct tosfs_dentry* root_block);
struct mapped_file_struct* map_file(char* path);

struct mapped_file_struct {
    int fd;
    void* mapped_file;
    struct stat file_info;

    struct tosfs_superblock* superblock;
    struct tosfs_inode* inodes;
    struct tosfs_dentry* root_block;
    char* data_blocks;
};



int main(void) {
    struct mapped_file_struct * file = map_file("test_tosfs_files");
    display_superblock(file->superblock);
    display_inode(file->inodes,file->superblock->inodes);
    display_root_block(file->root_block);

}




void display_superblock(struct tosfs_superblock* superblock_pointer) {
    printf("=======================\n");
    printf("magicnumber: %d\n",superblock_pointer->magic);
    printf("blockBitmap: %d\n",superblock_pointer->block_bitmap);
    printf("inodeBitmap: %d\n",superblock_pointer->inode_bitmap);
    printf("blockSize: %d\n",superblock_pointer->block_size);
    printf("blocks: %d\n",superblock_pointer->blocks);
    printf("inodes: %d\n",superblock_pointer->inodes);
    printf("=======================\n");
}

void display_inode(struct tosfs_inode* pInode,__u32 inodesnmbr) {
    printf("%i\n",(int)inodesnmbr);
    while(inodesnmbr > 0) {
        printf("=======================\n");
        printf("inodenumber:%d\n",pInode->inode);
        printf("blocknumber:%d\n",pInode->block_no);
        printf("uid:%d\n",pInode->uid);
        printf("gid:%d\n",pInode->gid);
        printf("size:%d\n",pInode->size);
        printf("nlink:%d\n",pInode->nlink);
        printf("=======================\n");
        inodesnmbr--;
        pInode+=TOSFS_INODE_SIZE;
    }
}

void display_root_block(struct tosfs_dentry* root_block) {
    printf("========================\n");
    printf("root block:\n");
    printf("inode:%d\n",root_block->inode);
    printf("name:%s\n",root_block->name);
}



struct mapped_file_struct* map_file(char* path) {


    struct mapped_file_struct* file=malloc(sizeof(struct mapped_file_struct));
    file->fd = open(path, O_RDONLY);
    if (file->fd ==SYSTEM_CALL_ERROR) {
        perror("test_tosfs_files: open");
        exit(EXIT_FAILURE);
    }


    if (stat(path, &file->file_info)==SYSTEM_CALL_ERROR) {
        perror("test_tosfs_files: stat");
        exit(EXIT_FAILURE);
    }


    file->mapped_file = mmap(0,file->file_info.st_size,PROT_READ,MAP_SHARED,file->fd,0);
    if ((int) file->mapped_file == SYSTEM_CALL_ERROR) {
        perror("test_tosfs_files: mmap");
    }


    file->superblock =(struct tosfs_superblock*) file->mapped_file;
    if ((int) file->superblock== SYSTEM_CALL_ERROR) {
        perror("test_tosfs_files: superblock");
    }


    if (file->superblock->magic !=TOSFS_MAGIC) {
        perror("magic");
    }

    //I Don't understand why the ptr operation gets me wrong inode ptr
    // compared to what's expected, testing is if it's any better
    struct tosfs_superblock* tempptr = file->superblock;
    tempptr++;
    file->inodes = (file->mapped_file+TOSFS_BLOCK_SIZE);
    if (file->inodes->block_no==0){
        perror("unexpected inodes ptr");
        file->inodes=(struct tosfs_inode*) tempptr;
        printf("inodes: %p\n",file->inodes);
    }
    file->root_block = (struct tosfs_dentry*) (file->inodes+TOSFS_BLOCK_SIZE);
    file->data_blocks =( char*) (file->root_block+TOSFS_BLOCK_SIZE*file->superblock->blocks) ;
    return file;
}