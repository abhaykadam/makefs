#ifndef FS_H
#define FS_H

#include <stdint.h>

#define BFS_NDIR_BLOCKS                12
#define BFS_N_BLOCKS                   (BFS_NDIR_BLOCKS)

/*
 * Structure of the superblock
 */

struct bfs_super_block {
	uint32_t	s_inodes_count;			/* Total number of inodes */
	uint32_t	s_blocks_count;			/* Filesystem size in blocks */
	uint32_t	s_free_blocks_count;		/* Free blocks counter */
	uint32_t	s_free_inodes_count;		/* Free inodes counter */
	uint32_t	s_first_data_block;		/* Number of first useful block */
	uint32_t	s_log_block_size;		/* Block size */
	uint32_t	s_blocks_per_group;		/* Number of blocks per group */
	uint32_t	s_inodes_per_group;		/* Number of inodes per group */
	uint16_t	s_inode_size;			/* Size of on-disk inode structure */
	uint8_t		s_uuid[16];			/* 128-bit silesystem identifier */
	char		s_volume_name[16];		/* Volume name */
	uint8_t		s_prealloc_blocks;		/* Number of blocks to preallocate */
	uint8_t		s_prealloc_dir_blocks;		/* Number of blocks to preallocate for directories */
	uint16_t	s_padding_1;			/* Alignment to word */
	uint32_t	s_reserved[238];		/* Padding to the end of the block */
};

/*
 * Structure of a blocks group descriptor
 */

struct bfs_group_desc {
	uint32_t	bg_block_bitmap;		/* Block number of block bitmap */
	uint32_t	bg_inode_bitmap;		/* Block number of inode bitmap */
	uint32_t	bg_inode_table;			/* Block number of first inode table block */
	uint16_t	bg_free_blocks_count;		/* Number of free blocks in the group */
	uint16_t	bg_free_inodes_count;		/* Number of free inodes in the group */
	uint16_t	bg_used_dirs_count;		/* Number of directories in the group */
	uint16_t	bg_pad;				/* Alignment to word */
	uint32_t	s_reserved[251];		/* Padding to the end of the block */	
};

/*
 * Structure of an inode on the disk
 */

struct bfs_inode {
	uint16_t	i_mode;				/* File type and access rights */
	uint16_t	i_uid;				/* Owner identifier */
	uint32_t	i_size;				/* File length in bytes */
	uint32_t	i_atime;			/* Time of last file access */
	uint32_t	i_ctime;			/* Time that inode last changed */
	uint32_t	i_mtime;			/* Time that file contents last changed */
	uint32_t	i_dtime;			/* Time of file deletion */
	uint16_t	i_gid;				/* User group identifier */
	uint16_t	i_padding_1;			/* Alignment to word */	
	uint32_t	i_blocks;			/* Number of data blocks of the file */
	uint32_t	i_block[BFS_N_BLOCKS];		/* Pointers to data blocks */
	uint32_t	s_reserved[12];			/* Padding to make size of bfs_inode 128 */	
};

/*
 * Ext2 directory file types.  Only the low 3 bits are used.
 */

enum {
	BFS_FT_UNKNOWN	= 0,	/* Unkown */
	BFS_FT_REG_FILE	= 1,	/* Regular file */
	BFS_FT_DIR	= 2,	/* Directory */
	BFS_FT_CHRDEV	= 3,	/* Character device */
	BFS_FT_BLKDEV	= 4,	/* Block device */
	BFS_FT_FIFO	= 5,	/* Named pipe */
	BFS_FT_SOCK	= 6,	/* Socket */
	BFS_FT_SYMLINK	= 7,	/* Symbolic Link */
	BFS_FT_MAX
};

/*
 * Structure of directory entry
 */
#define BFS_NAME_LEN 255

struct bfs_dir_entry {
	uint32_t	inode;			/* Inode number */
	uint16_t	rec_len;		/* Directory entry length */
	uint8_t		name_len;		
	uint8_t		file_type;
	char		name[BFS_NAME_LEN];	
};

#endif
