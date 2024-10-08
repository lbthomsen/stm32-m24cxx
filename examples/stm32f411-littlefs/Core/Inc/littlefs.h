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

#ifdef xxxDEBUG
#define LFS_DBG(...) printf(__VA_ARGS__);\
                     printf("\n");
#else
#define LFS_DBG(...) ;
#endif

extern lfs_t littlefs;

int m24cxx_littlefs_init(M24CXX_HandleTypeDef *m24cxx_init);

#endif /* INC_LITTLEFS_H_ */
