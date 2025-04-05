#include <lvgl.h>
#include <TFT_eSPI.h>
#include <SPI.h>
#include "ui.h"  // SquareLine Studio arayüzü

#define SCREEN_WIDTH    320
#define SCREEN_HEIGHT   240
#define TOUCH_ROTATION  1
#define LV_TICK_MS      5

TFT_eSPI tft = TFT_eSPI();
lv_display_t *disp;
lv_color_t *disp_buf;
unsigned long last_tick = 0;

// Dokunmatik okuma
void touch_read_cb(lv_indev_t *indev, lv_indev_data_t *data) {
  uint16_t x, y;
  if (tft.getTouch(&x, &y)) {
    data->state = LV_INDEV_STATE_PR;
    data->point.x = x;
    data->point.y = y;
  } else {
    data->state = LV_INDEV_STATE_REL;
  }
}

// Ekrana veri çizme
void flush_cb(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map) {
  tft.pushImage(area->x1, area->y1, lv_area_get_width(area), lv_area_get_height(area), (uint16_t *)px_map);
  lv_disp_flush_ready(disp);
}

void setup() {
  Serial.begin(115200);
  tft.begin();
  tft.setRotation(TOUCH_ROTATION);
  tft.setTouch((uint16_t[]){ 289, 3317, 408, 3360, 1 });
  tft.fillScreen(TFT_BLACK);

  lv_init();

  // Görüntü belleği ayarla
  disp_buf = (lv_color_t *)malloc(SCREEN_WIDTH * 40 * sizeof(lv_color_t));
  if (!disp_buf) {
    Serial.println("Ekran tampon belleği ayrılamadı!");
    return;
  }

  // LVGL ekran tanımı
  disp = lv_display_create(SCREEN_WIDTH, SCREEN_HEIGHT);
  lv_display_set_flush_cb(disp, flush_cb);
  lv_display_set_buffers(disp, disp_buf, NULL, SCREEN_WIDTH * 40 * 2, LV_DISPLAY_RENDER_MODE_PARTIAL);

  // Dokunmatik girişi tanımı
  lv_indev_t *indev = lv_indev_create();
  lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
  lv_indev_set_read_cb(indev, touch_read_cb);

  // SquareLine arayüzünü başlat
  ui_init();
}

void loop() {
  if (millis() - last_tick >= LV_TICK_MS) {
    lv_tick_inc(LV_TICK_MS);
    last_tick = millis();
  }
  lv_task_handler();
}
