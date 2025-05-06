//
// Created by Matthew Darnell on 5/5/25.
//
#include <stdio.h>
#include <string.h>

#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "../components/hash/include/hash.h"
#include "portmacro.h"
#include "freertos/task.h"

void print_array_as_hex(uint8_t* arr, size_t len) {
  int i;
  for (i = 0; i < len; i++) {
    printf("%02X ", arr[i]);
  }
  printf("\n");
}

void app_main() {

  unsigned char message[512] = { 0 };
  sprintf((char*)message, "The quick brown fox jumps over the lazy dog");
  uint8_t hash[32] = { 0 };
  sha256(hash, message, strlen((char*)message));
  print_array_as_hex(hash, 32);

  for (int i = 20; i >= 0; i--) {
    printf("Restarting in %d seconds...\n", i);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
  fflush(stdout);
  esp_restart();
}