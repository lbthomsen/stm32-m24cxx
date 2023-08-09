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

#include <math.h>
#include "main.h"
#include "m24cxx.h"

/* Internal functions */

uint32_t page_size() {
    return pow(2, M24CXX_ADDRESS_BITS);
}

/* Public functions */

M24CXX_StatusTypeDef m24cxx_init(M24CXX_HandleTypeDef *m24cxx, I2C_HandleTypeDef *i2c, uint8_t i2c_address) {

    M24CXXDBG("M24CXX Init type = %s size = %d b page = %lu", M24CXX_TYPE, M24CXX_SIZE, page_size());

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

    uint16_t page_start = address / page_size();
    uint16_t page_end = (address + len) / page_size() - 1;

    M24CXXDBG("Reading %d pages from %d to %d", 1 + page_end - page_start, page_start, page_end);

    for (uint8_t page = page_start; page <= page_end; ++ page) {
        uint16_t i2c_address = (m24cxx->i2c_address + page);

        uint16_t read_start = page == 0 ? address : page * page_size();
        uint16_t read_len = page == 0 ? page_size() - address : page_size();
        read_len = page * page_size() + read_start > len ? len -  page * page_size()  : read_len;

        M24CXXDBG("Reading page %d, i2c address = 0x%02x start = 0x%04x len = 0x%04x", page, i2c_address, read_start, read_len);


    }

    return M24CXX_Ok;
}

