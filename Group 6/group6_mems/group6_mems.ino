#include <driver/i2s.h>

#define I2S_WS 25
#define I2S_SD 33
#define I2S_SCK 26
#define I2S_PORT I2S_NUM_0

#define BUFFER_LEN 64

int32_t sBuffer[BUFFER_LEN];

void setup() {

  Serial.begin(115200);
  delay(1000);

  Serial.println("I2S microphone test");

  const i2s_config_t i2s_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
    .sample_rate = 44100,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT,
    .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
    .communication_format = I2S_COMM_FORMAT_I2S,
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = 8,
    .dma_buf_len = BUFFER_LEN,
    .use_apll = false
  };

  i2s_driver_install(I2S_PORT, &i2s_config, 0, NULL);

  const i2s_pin_config_t pin_config = {
    .bck_io_num = I2S_SCK,
    .ws_io_num = I2S_WS,
    .data_out_num = -1,
    .data_in_num = I2S_SD
  };

  i2s_set_pin(I2S_PORT, &pin_config);

  i2s_zero_dma_buffer(I2S_PORT);
}

void loop() {

  size_t bytes_read;

  i2s_read(
    I2S_PORT,
    sBuffer,
    sizeof(sBuffer),
    &bytes_read,
    portMAX_DELAY
  );

  int samples = bytes_read / 4;

  float level = 0;

  for (int i = 0; i < samples; i++) {
    int32_t sample = sBuffer[i] >> 8;
    level += abs(sample);
  }

  level /= samples;

  Serial.println(level);
}