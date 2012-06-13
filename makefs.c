#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
	
#include <fs.h>
	
#define DISK_SIZE 10485760
	
	
int main(int argc, char *argv[]) {
	if ( argc < 2 ) {
		perror ( "syntax: createnformat volume_name \n");
		exit(0);
	}

	char volume_name[16];
	strcpy(volume_name, argv[1]);
	mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
	
	int disk = open(argv[1], O_CREAT | O_EXCL | O_WRONLY, mode);
	if (disk < 0) {
		perror("Can't open the disk\n");
		exit(1);
	} else {
		if (ftruncate(disk, BLOCK_SIZE) == 0){
			printf("Volume of size %d is created successfully.\n", DISK_SIZE);
		} else {
			perror("Couldn't create the volume.\n");
		}
	}
	
	/* Format the volume */
	
	/*section offsets in terms of blocks */
	int superblock_off	= 1; /* 0th is for the bootblock */
	int group_desc_off	= superblock_off + 1;
	int blk_bitmap_off	= group_desc_off + 1;
	int inode_bitmap_off	= blk_bitmap_off + 1;
	int inode_table_off	= inode_bitmap_off + 1;
	int data_block_off	= inode_table_off + sizeof(struct bfs_inode) * 8192;
	
	/* Intialize superblock */
	struct bfs_super_block bfs_sb = {
		.s_inodes_count = 8192,				/* Total number of inodes */
		.s_blocks_count = 10240,			/* Filesystem size in blocks */
		.s_free_blocks_count = 8192,			/* Free blocks counter */
		.s_free_inodes_count = 8192,			/* Free inodes counter */
		.s_first_data_block = 0,			/* Number of first useful block */
		.s_log_block_size = 0,				/* Block size: 0 being 1024 bytes */
		.s_inode_size = sizeof(struct bfs_inode),	/* Size of on-disk inode structure */
		.s_prealloc_blocks = 1,				/* Number of blocks to preallocate */
		.s_prealloc_dir_blocks = 1 			/* Number of blocks to preallocate for directories */
	};
	
	memcpy ( (void*)&(bfs_sb.s_uuid), "be2d14026c6246dfbb45215a002dd473",
		strlen("be2d14026c6246dfbb45215a002dd473") ); /* 128-bit filesystem identifier */
	strcpy ( bfs_sb.s_volume_name, volume_name );
	
	pwrite (disk, &bfs_sb,
		sizeof(bfs_sb), BLOCK_SIZE * superblock_off); /* Superblock starts at 1st block of the volume.
									0th is the bootblock. */
	
	
	/* Initialize group descriptor */
	
	struct bfs_group_desc bfs_gd = {
		.bg_block_bitmap = blk_bitmap_off,			/* Block number of block bitmap */
		.bg_inode_bitmap = inode_bitmap_off,			/* Block number of inode bitmap */
		.bg_inode_table = inode_table_off,			/* Block number of first inode table block */
		.bg_free_blocks_count = bfs_sb.s_free_blocks_count,	/* Number of free blocks in the group */
		.bg_free_inodes_count = bfs_sb.s_free_inodes_count,	/* Number of free inodes in the group */
		.bg_used_dirs_count = 0,				/* Number of directories in the group */
	};
	
	/* Write block descriptor to the volume */
	pwrite (disk, &bfs_gd,
		sizeof(bfs_gd), BLOCK_SIZE * group_desc_off );
	
	
	/* Create root directory */
	
	
	struct bfs_inode root_dir = { /* initializing inode for directory */
		.i_mode = BFS_FT_DIR,
		.i_uid = 0,
		.i_gid = 0,
		.i_blocks = 1,
		.i_block[0] = data_block_off + bfs_sb.s_first_data_block
	};
	
	/* Write directory entry into inode table */
	pwrite (disk, &root_dir, sizeof(struct bfs_inode),
		BLOCK_SIZE * inode_table_off);
	
	/* Set inode bitmap for root directory */
	uint8_t bit_inode_first = 0b10000000;
	pwrite (disk, &bit_inode_first, sizeof(bit_inode_first),
		BLOCK_SIZE * inode_bitmap_off);
	
	
		
	static struct bfs_dir_entry dot = {
		.inode		= 0,
		.rec_len	= 12,
		.name_len	= 1,
		.file_type	= BFS_FT_DIR,
		.name[4]	= ".\0\0"
	};
	
	pwrite(disk, &dot, dot.name_len, 
		BLOCK_SIZE * data_block_off);

	static struct bfs_dir_entry dotdot = {
		.inode		= 0,
		.rec_len	= 12,
		.name_len	= 2,
		.file_type	= BFS_FT_DIR,
		.name[4]	= "..\0"
	};

	pwrite(disk, &dotdot, dotdot.name_len, 
		BLOCK_SIZE * data_block_off + dot.name_len);
	
	
	return 0;
}
