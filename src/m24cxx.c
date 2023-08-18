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

#include <math.h>

#include "m24cxx.h"

/* Public functions */

M24CXX_StatusTypeDef m24cxx_init(M24CXX_HandleTypeDef *m24cxx, I2C_HandleTypeDef *i2c, uint8_t i2c_address) {

    M24CXXDBG("M24CXX Init type = %s size = %d ", M24CXX_TYPE, M24CXX_SIZE);

    m24cxx->i2c = i2c;
    m24cxx->i2c_address = i2c_address;

#if (_EEPROM_USE_WP_PIN==1)
  HAL_GPIO_WritePin(_EEPROM_WP_GPIO,_EEPROM_WP_PIN,GPIO_PIN_SET);
#endif

    if (m24cxx_isconnected(m24cxx) != M24CXX_Ok) return M24CXX_Err;

    return M24CXX_Ok;
}

M24CXX_StatusTypeDef m24cxx_isconnected(M24CXX_HandleTypeDef *m24cxx) {

    M24CXXDBG("M24CXX isconnected");

    if (HAL_I2C_IsDeviceReady(m24cxx->i2c, m24cxx->i2c_address << 1, 2, HAL_MAX_DELAY) != HAL_OK) {
        M24CXXDBG("M24CXX Is NOT ready!");
        return M24CXX_Err;
    }

    return M24CXX_Ok;
}

M24CXX_StatusTypeDef m24cxx_wait(M24CXX_HandleTypeDef *m24cxx) {
    uint32_t start_time = HAL_GetTick();
    while (m24cxx_isconnected(m24cxx) != M24CXX_Ok) {
        if (HAL_GetTick() - start_time >= M24CXX_WRITE_TIMEOUT) return M24CXX_Err;
    }
    return M24CXX_Ok;
}

M24CXX_StatusTypeDef m24cxx_read(M24CXX_HandleTypeDef *m24cxx, uint32_t address, uint8_t *data, uint16_t len) {
    M24CXXDBG("M24CXX read - address = 0x%04lx len = 0x%04x", address, len);

    uint16_t page_start = address / M24CXX_READ_PAGE_SIZE;
    uint16_t start_offset = address % M24CXX_READ_PAGE_SIZE;
    uint16_t page_end = ((address + len - 1) / M24CXX_READ_PAGE_SIZE);
    uint16_t end_offset = (address + len - 1) % M24CXX_READ_PAGE_SIZE;
    uint32_t data_offset = 0;

    M24CXXDBG("Reading %u pages from %u/%u to %u/%u", 1 + page_end - page_start, page_start, start_offset, page_end, end_offset);

    for (uint8_t page = page_start; page <= page_end; ++ page) {

        uint32_t i2c_address = (m24cxx->i2c_address + page);
        uint16_t read_start, read_len;

        if (page == page_start) { // First page
            read_start = address;
            read_len = M24CXX_READ_PAGE_SIZE - address;
        } else if (page == page_end) { // Last page
            read_start = page * M24CXX_READ_PAGE_SIZE;
            read_len = end_offset;
        } else { // Other pages
            read_start = page * M24CXX_READ_PAGE_SIZE;
            read_len = M24CXX_READ_PAGE_SIZE;
        }

        //uint16_t read_start = page == page_start ? address : page * M24CXX_READ_PAGE_SIZE;
        //uint16_t read_len = page == page_start ? M24CXX_READ_PAGE_SIZE - address : M24CXX_READ_PAGE_SIZE;
        //read_len = page * M24CXX_READ_PAGE_SIZE + read_start > len ? len -  page * M24CXX_READ_PAGE_SIZE : read_len;

        M24CXXDBG("Reading page %d, i2c address = 0x%02x start = 0x%04x len = 0x%04lx offset = 0x%04lx", page, i2c_address, read_start, read_len, data_offset);

        HAL_StatusTypeDef result = HAL_I2C_Mem_Read(m24cxx->i2c, (m24cxx->i2c_address + page) << 1, read_start, M24CXX_ADDRESS_SIZE, data + data_offset, read_len, HAL_MAX_DELAY);

        if (result != HAL_OK) {
            M24CXXDBG("Failed to read memory");
            return M24CXX_Err;
        }

        data_offset += read_len;

    }

    return M24CXX_Ok;
}

M24CXX_StatusTypeDef m24cxx_write(M24CXX_HandleTypeDef *m24cxx, uint32_t address, uint8_t *data, uint16_t len) {

    M24CXXDBG("M24CXX write - address = 0x%04lx len = 0x%04lx", address, len);

    uint16_t page_start = address / M24CXX_READ_PAGE_SIZE;
    uint16_t start_offset = address % M24CXX_READ_PAGE_SIZE;
    uint16_t page_end = ((address + len - 1) / M24CXX_READ_PAGE_SIZE);
    uint16_t end_offset = (address + len - 1) % M24CXX_READ_PAGE_SIZE;
    uint32_t data_offset = 0;

    uint32_t page_start = address / M24CXX_WRITE_PAGE_SIZE;
    uint32_t page_end = (address + len) / M24CXX_WRITE_PAGE_SIZE;
    uint32_t data_offset = 0;

    M24CXXDBG("Writing %lu pages from %lu to %lu", 1 + page_end - page_start, page_start, page_end);

    for (uint8_t page = page_start; page <= page_end; ++ page) {
        uint32_t i2c_address = (m24cxx->i2c_address + page);

        uint16_t read_start = page == 0 ? address : page * M24CXX_WRITE_PAGE_SIZE;
        uint16_t read_len = page == 0 ? M24CXX_WRITE_PAGE_SIZE- address : M24CXX_WRITE_PAGE_SIZE;
        read_len = page * M24CXX_WRITE_PAGE_SIZE + read_start > len ? len -  page * M24CXX_WRITE_PAGE_SIZE : read_len;

        M24CXXDBG("Writing page %d, i2c address = 0x%02lx start = 0x%04lx len = 0x%04lx offset = 0x%04lx", page, i2c_address, read_start, read_len, data_offset);

        //HAL_StatusTypeDef result = HAL_I2C_Mem_Write(m24cxx->i2c, (m24cxx->i2c_address + page) << 1, read_start, M24CXX_ADDRESS_SIZE, data + data_offset, read_len, HAL_MAX_DELAY);
        HAL_StatusTypeDef result = HAL_OK;

        if (result != HAL_OK) {
            M24CXXDBG("Failed to write memory");
            return M24CXX_Err;
        }

        data_offset += read_len;

        if (m24cxx_wait(m24cxx) != M24CXX_Ok) {
            M24CXXDBG("M24Cxx Device never got ready");
            return M24CXX_Err;
        }

    }

    return M24CXX_Ok;
}

