#ifndef FACEOFF_SETTINGS_H
#define FACEOFF_SETTINGS_H

#include <pebble.h>

#define SETTINGS_KEY 2

typedef struct Settings {
  GColor top_stripe_color;
  GColor bottom_stripe_color;
  GColor background_color;
  bool fill_corners;
  GColor hour_color;
  GColor minute_color;
  bool show_date;
  GColor wday_color;
  GColor mday_color;
} Settings;

extern Settings g_settings;

void default_settings();
bool update_settings(DictionaryIterator *iterator, void *context);

#endif
