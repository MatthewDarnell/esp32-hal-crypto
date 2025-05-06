//
// Created by Matthew Darnell on 5/4/25.
//

#include <inttypes.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "include/hash.h"
#include <string.h>
#include "soc/dport_access.h"
#include "soc/dport_reg.h"

#define SHA_TEXT_0_REG 0x3FF03000
#define SHA_TEXT_1_REG 0x3FF03004
#define SHA_TEXT_2_REG 0x3FF03008
#define SHA_TEXT_3_REG 0x3FF0300C
#define SHA_TEXT_4_REG 0x3FF03010
#define SHA_TEXT_5_REG 0x3FF03014
#define SHA_TEXT_6_REG 0x3FF03018
#define SHA_TEXT_7_REG 0x3FF0301C
#define SHA_TEXT_8_REG 0x3FF03020
#define SHA_TEXT_9_REG 0x3FF03024
#define SHA_TEXT_10_REG 0x3FF03028
#define SHA_TEXT_11_REG 0x3FF0302C
#define SHA_TEXT_12_REG 0x3FF03030
#define SHA_TEXT_13_REG 0x3FF03034
#define SHA_TEXT_14_REG 0x3FF03038
#define SHA_TEXT_15_REG 0x3FF0303C


#define SHA_SHA256_START_REG 0x3FF03090
#define SHA_SHA256_CONTINUE_REG 0x3FF03094
#define SHA_SHA256_LOAD_REG 0x3FF03098
#define SHA_SHA256_BUSY_REG 0x3FF0309C


#define SHA_SHA512_START_REG 0x3FF03090
#define SHA_SHA512_CONTINUE_REG 0x3FF03094
#define SHA_SHA512_LOAD_REG 0x3FF03098
#define SHA_SHA512_BUSY_REG 0x3FF0309C

/*
  https://www.espressif.com/sites/default/files/documentation/esp32_technical_reference_manual_en.pdf
  Section 23
*/

void read_uint_into_array(const uint32_t REG, uint8_t *message, const size_t offset) {
  uint32_t data = DPORT_REG_READ(REG);
  message[offset] = data >> 24 & 0x000000FF;
  message[offset + 1] = data >> 16 & 0x000000FF;
  message[offset + 2] = data >> 8 & 0x000000FF;
  message[offset + 3] = data & 0x000000FF;
}

void write_array_into_uint(const uint32_t REG, const uint8_t *message, const size_t offset) {
  uint32_t data = 0;
  data = message[offset] & 0xFF;
  data <<= 8;
  data |= message[offset + 1] & 0xFF;
  data <<= 8;
  data |= message[offset + 2] & 0xFF;
  data <<= 8;
  data |= message[offset + 3] & 0xFF;
  DPORT_REG_WRITE(REG, (data));
}

void write_message_block(bool start, uint8_t *block) {
  write_array_into_uint(SHA_TEXT_0_REG, block, 0);
  write_array_into_uint(SHA_TEXT_1_REG, block, 4);
  write_array_into_uint(SHA_TEXT_2_REG, block, 8);
  write_array_into_uint(SHA_TEXT_3_REG, block, 12);
  write_array_into_uint(SHA_TEXT_4_REG, block, 16);
  write_array_into_uint(SHA_TEXT_5_REG, block, 20);
  write_array_into_uint(SHA_TEXT_6_REG, block, 24);
  write_array_into_uint(SHA_TEXT_7_REG, block, 28);
  write_array_into_uint(SHA_TEXT_8_REG, block, 32);
  write_array_into_uint(SHA_TEXT_9_REG, block, 36);
  write_array_into_uint(SHA_TEXT_10_REG, block, 40);
  write_array_into_uint(SHA_TEXT_11_REG, block, 44);
  write_array_into_uint(SHA_TEXT_12_REG, block, 48);
  write_array_into_uint(SHA_TEXT_13_REG, block, 52);
  write_array_into_uint(SHA_TEXT_14_REG, block, 56);
  write_array_into_uint(SHA_TEXT_15_REG, block, 60);
  if (start) {
    DPORT_REG_WRITE(SHA_SHA256_START_REG, 1);
  } else {
    DPORT_REG_WRITE(SHA_SHA256_CONTINUE_REG, 1);
  }
  while (DPORT_REG_READ(SHA_SHA256_BUSY_REG) != 0x00) {}
}

void read_message_digest(uint8_t *message) {
  DPORT_REG_WRITE(SHA_SHA256_LOAD_REG, 1);
  while (DPORT_REG_READ(SHA_SHA256_BUSY_REG) != 0) {}
  read_uint_into_array(SHA_TEXT_0_REG, message, 0);
  read_uint_into_array(SHA_TEXT_1_REG, message, 4);
  read_uint_into_array(SHA_TEXT_2_REG, message, 8);
  read_uint_into_array(SHA_TEXT_3_REG, message, 12);
  read_uint_into_array(SHA_TEXT_4_REG, message, 16);
  read_uint_into_array(SHA_TEXT_5_REG, message, 20);
  read_uint_into_array(SHA_TEXT_6_REG, message, 24);
  read_uint_into_array(SHA_TEXT_7_REG, message, 28);
}

void write_u64_to_array(uint8_t *data, const uint64_t value, const size_t offset) {
  data[offset] = value >> 56 & 0x000000FF;
  data[offset + 1] = value >> 48 & 0x000000FF;
  data[offset + 2] = value >> 40 & 0x000000FF;
  data[offset + 3] = value >> 32 & 0x000000FF;
  data[offset + 4] = value >> 24 & 0x000000FF;
  data[offset + 5] = value >> 16 & 0x000000FF;
  data[offset + 6] = value >> 8 & 0x000000FF;
  data[offset + 7] = value & 0x000000FF;
}

void sha256(uint8_t *hash, unsigned char *message, size_t d_len) {
  DPORT_REG_SET_BIT(DPORT_PERI_CLK_EN_REG, DPORT_PERI_EN_SHA);
  DPORT_REG_CLR_BIT(DPORT_PERI_RST_EN_REG, DPORT_PERI_EN_SHA | DPORT_PERI_EN_SECUREBOOT);
  size_t bit_len = (d_len * 8);
  size_t num_blocks = 1;
  while (bit_len > 448) {
    num_blocks++;
    bit_len -= 448;
  }
  bit_len = (d_len * 8) + 1 + 64;
  size_t num_zeroes = num_blocks * 512 - bit_len;
  num_zeroes -= 7;
  size_t padding_len = num_zeroes;
  size_t pad_bytes = padding_len / 8;
  size_t full_length = d_len + 1 + pad_bytes + 8;
  uint8_t data[full_length];
  memset(data, 0, sizeof(data));
  memcpy(data, message, d_len);
  data[d_len] = 0x80;
  uint64_t len = d_len * 8;
  write_u64_to_array(data, len, full_length-8);
  uint32_t i = 0;
  bool start = true;
  for (i=0; i < num_blocks; i++) {
    write_message_block(start, &data[i*64]);
    start = false;
  }
  read_message_digest(hash);
}
