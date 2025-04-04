#include <lvgl.h>
#include <TFT_eSPI.h>
#include <SPI.h>
#include "ui.h"  // SquareLine UI başlatıcısı

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240
#define TOUCH_ROTATION 1

TFT_eSPI tft = TFT_eSPI();

uint32_t screenWidth;
uint32_t screenHeight;
uint32_t bufSize;
lv_display_t *disp;
lv_color_t *disp_draw_buf;
unsigned long last_tick = 0;

void my_touchpad_read(lv_indev_t *indev, lv_indev_data_t *data) {
  uint16_t touchX, touchY;
  bool touched = tft.getTouch(&touchX, &touchY);
  if (touched) {
    data->state = LV_INDEV_STATE_PR;
    data->point.x = touchX;
    data->point.y = touchY;
  } else {
    data->state = LV_INDEV_STATE_REL;
  }
}

void my_disp_flush(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map) {
  uint32_t w = lv_area_get_width(area);
  uint32_t h = lv_area_get_height(area);
  tft.pushImage(area->x1, area->y1, w, h, (uint16_t *)px_map);
  lv_disp_flush_ready(disp);
}

void setup() {
  Serial.begin(115200);
  Serial.println("TFT_eSPI + SquareLine");

  tft.begin();
  tft.setRotation(TOUCH_ROTATION);
  uint16_t calData[5] = { 289, 3317, 408, 3360, 1 };
  tft.setTouch(calData);
  tft.fillScreen(TFT_BLACK);

  lv_init();

  screenWidth = SCREEN_WIDTH;
  screenHeight = SCREEN_HEIGHT;
  bufSize = screenWidth * 40;

  disp_draw_buf = (lv_color_t *)malloc(bufSize * 2);
  if (!disp_draw_buf) {
    Serial.println("LVGL disp_draw_buf allocate failed!");
    return;
  }

  disp = lv_display_create(screenWidth, screenHeight);
  lv_display_set_flush_cb(disp, my_disp_flush);
  lv_display_set_buffers(disp, disp_draw_buf, NULL, bufSize * 2, LV_DISPLAY_RENDER_MODE_PARTIAL);

  lv_indev_t *indev = lv_indev_create();
  lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
  lv_indev_set_read_cb(indev, my_touchpad_read);
  ui_init();
}

void loop() {
  unsigned long now = millis();
  if (now - last_tick >= 5) {
    lv_tick_inc(5);
    last_tick = now;
  }
  lv_task_handler();
}
