#include "settings.h"
#include "gcolor_definitions.h"
#include "message_keys.auto.h"

Settings g_settings;

void default_settings() {
  g_settings.top_stripe_color = GColorJazzberryJam;
  g_settings.bottom_stripe_color = GColorVeryLightBlue;
  g_settings.background_color = GColorBlack;
  g_settings.hour_color = GColorWhite;
  g_settings.minute_color = GColorWhite;
  g_settings.show_date = true;
  g_settings.wday_color = GColorWhite;
  g_settings.mday_color = GColorWhite;
}

bool update_settings(DictionaryIterator *iterator, void *context) {
  bool dirty = false;

  Tuple *top_stripe_color_tuple =
      dict_find(iterator, MESSAGE_KEY_TOP_STRIPE_COLOR);

  if (top_stripe_color_tuple) {
    g_settings.top_stripe_color =
        GColorFromHEX(top_stripe_color_tuple->value->int32);
    dirty = true;
  }

  Tuple *bottom_stripe_color_tuple =
      dict_find(iterator, MESSAGE_KEY_BOTTOM_STRIPE_COLOR);
  if (bottom_stripe_color_tuple) {
    g_settings.bottom_stripe_color =
        GColorFromHEX(bottom_stripe_color_tuple->value->int32);
    dirty = true;
  }

  Tuple *background_color_tuple =
      dict_find(iterator, MESSAGE_KEY_BACKGROUND_COLOR);
  if (background_color_tuple) {
    g_settings.background_color =
        GColorFromHEX(background_color_tuple->value->int32);
    dirty = true;
  }

  Tuple *hour_color_tuple = dict_find(iterator, MESSAGE_KEY_HOUR_COLOR);
  if (hour_color_tuple) {
    g_settings.hour_color = GColorFromHEX(hour_color_tuple->value->int32);
    dirty = true;
  }

  Tuple *minute_color_tuple = dict_find(iterator, MESSAGE_KEY_MINUTE_COLOR);
  if (minute_color_tuple) {
    g_settings.minute_color = GColorFromHEX(minute_color_tuple->value->int32);
    dirty = true;
  }

  Tuple *show_date_tuple = dict_find(iterator, MESSAGE_KEY_SHOW_DATE);
  if (show_date_tuple) {
    g_settings.show_date = show_date_tuple->value->int32 == 1;
    dirty = true;
  }

  Tuple *wday_color = dict_find(iterator, MESSAGE_KEY_WDAY_COLOR);
  if (wday_color) {
    g_settings.wday_color = GColorFromHEX(wday_color->value->int32);
    dirty = true;
  }

  Tuple *mday_color_tuple = dict_find(iterator, MESSAGE_KEY_MDAY_COLOR);
  if (mday_color_tuple) {
    g_settings.mday_color = GColorFromHEX(mday_color_tuple->value->int32);
    dirty = true;
  }

  return dirty;
}
