/**
 ******************************************************************************
 * @file           : m24cxx.h
 * @brief          : M24Cxx Library Header
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

#ifndef M24CXX_H_
#define M24CXX_H_

#define M24C08 1
#define M24C01 2

#if M24CXX_MODEL == M24C08
#define M24CXX_TYPE            "24C08"
#define M24CXX_SIZE              1024
#define M24CXX_PAGE_ADDRESS_BITS    2
#define M24CXX_ADDRESS_BITS         8
#elif M24CXX_MODEL == M24C01
#define M24CXX_TYPE            "24C01"
#define M24CXX_SIZE              1024
#define M24CXX_PAGE_ADDRESS_BITS    2
#define M24CXX_ADDRESS_BITS         8
#else
#error "M24CXX_MODEL must be defined"
#endif

#ifdef DEBUG
#define M24CXXDBG(...) printf(__VA_ARGS__);\
                       printf("\n")
#else
#define M24CXXDBG(...)
#endif

typedef struct {
    I2C_HandleTypeDef *i2c;
    uint8_t i2c_address;
} M24CXX_HandleTypeDef;

typedef enum {
    M24CXX_Ok,
    M24CXX_Err
} M24CXX_StatusTypeDef;

M24CXX_StatusTypeDef m24cxx_init(M24CXX_HandleTypeDef *m24cxx, I2C_HandleTypeDef *i2c, uint8_t i2c_address);
M24CXX_StatusTypeDef m24cxx_isconnected(M24CXX_HandleTypeDef *m24cxx);
M24CXX_StatusTypeDef m24cxx_read(M24CXX_HandleTypeDef *m24cxx, uint16_t address, uint8_t *data, uint16_t len);

#endif /* M24CXX_H_ */
