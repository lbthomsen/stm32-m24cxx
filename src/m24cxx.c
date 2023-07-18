/**
 ******************************************************************************
 * @file           : m24cxx.c
 * @brief          : M24Cxx Library Source
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
#include "m24cxx.h"

M24CXX_StatusTypeDef m24cxx_init(M24CXX_HandleTypeDef *m24cxx, I2C_HandleTypeDef *i2c, uint8_t i2c_address) {

    M24CXXDBG("M24CXX Init");

    m24cxx->i2c = i2c;
    m24cxx->i2c_address = i2c_address;

    if (m24cxx_isconnected(m24cxx) != M24CXX_Ok) return M24CXX_Err;

    return M24CXX_Ok;
}

M24CXX_StatusTypeDef m24cxx_isconnected(M24CXX_HandleTypeDef *m24cxx) {

    M24CXXDBG("M24CXX isconnected");

#if (_EEPROM_USE_WP_PIN==1)
  HAL_GPIO_WritePin(_EEPROM_WP_GPIO,_EEPROM_WP_PIN,GPIO_PIN_SET);
#endif

    if (HAL_I2C_IsDeviceReady(m24cxx->i2c, m24cxx->i2c_address << 1, 2, 100) != HAL_OK) {
        M24CXXDBG("M24CXX Is NOT ready!");
        return M24CXX_Err;
    }

    return M24CXX_Ok;
}

M24CXX_StatusTypeDef m24cxx_read(M24CXX_HandleTypeDef *m24cxx, uint16_t address, uint8_t *data, uint16_t len) {
    M24CXXDBG("M24CXX read - address = 0x%04x len = 0x%04x", address, len);



    return M24CXX_Ok;
}

