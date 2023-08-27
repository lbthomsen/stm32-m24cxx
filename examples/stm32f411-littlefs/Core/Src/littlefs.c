/**
 ******************************************************************************
 * @file           : m24cxx.h
 * @brief          : M24Cxx Littlefs Source
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2023 Lars Boegild Thomsen <lbthomsen@gmail.com>
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */

#include "main.h"
#include "lfs.h"
#include "m24cxx.h"
#include "littlefs.h"

int littlefs_read(const struct lfs_config *c, lfs_block_t block, lfs_off_t off, void *buffer, lfs_size_t size);
int littlefs_prog(const struct lfs_config *c, lfs_block_t block, lfs_off_t off, const void *buffer, lfs_size_t size);
int littlefs_erase(const struct lfs_config *c, lfs_block_t block);
int littlefs_sync(const struct lfs_config *c);

struct lfs_config littlefs_config = {
    // block device operations
    .read  = littlefs_read,
    .prog  = littlefs_prog,
    .erase = littlefs_erase,
    .sync  = littlefs_sync,

    // block device configuration
    .read_size = 32,
    .prog_size = 32,
    .block_size = 128,
    .block_count = 8,
    .cache_size = 32,
    .lookahead_size = 32,
    .block_cycles = 10,
};

lfs_t littlefs;
M24CXX_HandleTypeDef *m24cxx_handle;

int m24cxx_littlefs_init(M24CXX_HandleTypeDef *m24cxx_init) {
        LFS_DBG("LittleFS Init");
        m24cxx_handle = m24cxx_init;

        //littlefs_config.block_size = w25qxx_handle->sector_size;
        //littlefs_config.block_count = w25qxx_handle->sectors_in_block * w25qxx_handle->block_count;

        int err = lfs_mount(&littlefs, &littlefs_config);

    // reformat if we can't mount the filesystem
    // this should only happen on the first boot
    if (err) {
        lfs_format(&littlefs, &littlefs_config);
        lfs_mount(&littlefs, &littlefs_config);
    }

    return 0;

}

int littlefs_read(const struct lfs_config *c, lfs_block_t block, lfs_off_t off, void *buffer, lfs_size_t size) {
        LFS_DBG("LittleFS Read b = 0x%04lx o = 0x%04lx s = 0x%04lx", block, off, size);
        if (m24cxx_read(m24cxx_handle, block * littlefs_config.block_size + off, buffer, size) != M24CXX_Ok) return -1;
        return 0;
}

int littlefs_prog(const struct lfs_config *c, lfs_block_t block, lfs_off_t off, const void *buffer, lfs_size_t size) {
        LFS_DBG("LittleFS Prog b = 0x%04lx o = 0x%04lx s = 0x%04lx", block, off, size);
        if (m24cxx_write(m24cxx_handle, block * littlefs_config.block_size + off, (void *)buffer, size) != M24CXX_Ok) return -1;
        return 0;
}

int littlefs_erase(const struct lfs_config *c, lfs_block_t block) {
        LFS_DBG("LittleFS Erase b = 0x%04lx", block);
        if (m24cxx_erase(m24cxx_handle, block * littlefs_config.block_size, littlefs_config.block_size) != M24CXX_Ok) return -1;
        return 0;
}

int littlefs_sync(const struct lfs_config *c) {
        LFS_DBG("LittleFS Sync");
        return 0;
}


