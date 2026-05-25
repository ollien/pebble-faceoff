#ifndef FACEOFF_SETTINGS_H
#define FACEOFF_SETTINGS_H

#include <pebble.h>

#define SETTINGS_KEY 4

typedef enum {
  TIME_FORMAT_SYSTEM = 0,
  TIME_FORMAT_12H = 1,
  TIME_FORMAT_24H = 2,
} TimeFormat;

typedef struct Settings {
  GColor top_stripe_color;
  GColor bottom_stripe_color;
  GColor background_color;
  bool fill_corners;
  GColor hour_color;
  GColor minute_color;
  TimeFormat time_format;
  bool show_date;
  GColor wday_color;
  GColor mday_color;
} Settings;

extern Settings g_settings;

void default_settings();
bool update_settings(DictionaryIterator *iterator, void *context);
bool migrate_settings();

#endif
