#ifndef FACEOFF_SETTINGS_H
#define FACEOFF_SETTINGS_H

#include <pebble.h>

#define SETTINGS_KEY 3
#define SETTINGS_KEY_V2 2

typedef struct Settings {
  GColor top_stripe_color;
  GColor bottom_stripe_color;
  GColor background_color;
  bool fill_corners;
  GColor hour_color;
  GColor minute_color;
  bool show_24h_time;
  bool show_date;
  GColor wday_color;
  GColor mday_color;
} Settings;

typedef struct SettingsV2 {
  GColor top_stripe_color;
  GColor bottom_stripe_color;
  GColor background_color;
  bool fill_corners;
  GColor hour_color;
  GColor minute_color;
  bool show_date;
  GColor wday_color;
  GColor mday_color;
} SettingsV2;

extern Settings g_settings;

void default_settings();
bool update_settings(DictionaryIterator *iterator, void *context);
void from_v2_settings(SettingsV2 v2);

#endif
