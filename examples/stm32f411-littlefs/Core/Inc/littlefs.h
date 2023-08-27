/**
 ******************************************************************************
 * @file           : m24cxx.h
 * @brief          : M24Cxx Littlefs Header
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

#ifndef INC_LITTLEFS_H_
#define INC_LITTLEFS_H_

#include "lfs.h"

#ifdef DEBUG
#define LFS_DBG(...)    printf(__VA_ARGS__);\
                        printf("\n");
#else
#define LFS_DBG(...)
#endif

#define LFS_BLOCK_SIZE 0xff
#define LFS_BLOCK_COUNT 0x04 // 1 MB

extern lfs_t littlefs;

FRESULT littlefs_init(void *key);

#endif /* INC_LITTLEFS_H_ */
