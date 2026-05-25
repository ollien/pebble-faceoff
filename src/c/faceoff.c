#include "settings.h"
#include <pebble-fctx/fctx.h>
#include <pebble-fctx/ffont.h>
#include <pebble.h>
#include <time.h>

// -22.5 deg
#define TEXT_ANGLE (-45 * TRIG_MAX_ANGLE / 720)
// 67.5 deg
#define SLANT_ANGLE (TEXT_ANGLE + 90 * TRIG_MAX_ANGLE / 360)

static Window *s_window;
static FFont *s_font;
static Layer *s_time_layer;
static Layer *s_background_layer;

static char *s_wdays[] = {"SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT"};

static int32_t prv_f_time_font_height(GRect bounds) {
  return INT_TO_FIXED(bounds.size.h / 3);
}

static int32_t prv_f_date_font_height(GRect bounds) {
  return INT_TO_FIXED(bounds.size.h / 10);
}

static int32_t prv_f_time_font_radius(GRect bounds) {
  return INT_TO_FIXED(bounds.size.h) / 3 - INT_TO_FIXED(bounds.size.h) / 9;
}

static int32_t prv_f_date_font_radius(GRect bounds) {
  return prv_f_time_font_radius(bounds) - prv_f_time_font_height(bounds) / 2 +
         prv_f_date_font_height(bounds) / 2;
}

static int32_t prv_f_date_font_offset(GRect bounds) {
  return INT_TO_FIXED(bounds.size.h) / 2 - INT_TO_FIXED(bounds.size.h) / 5;
}

static void prv_f_draw_text(FContext *fctx, FPoint f_center, const char *text,
                            GColor color) {
  fctx_set_rotation(fctx, TEXT_ANGLE);

  fctx_begin_fill(fctx);
  fctx_set_offset(fctx, f_center);
  fctx_set_fill_color(fctx, color);
  fctx_draw_string(fctx, text, s_font, GTextAlignmentCenter,
                   FTextAnchorCapMiddle);
  fctx_end_fill(fctx);
}

static void prv_draw_time(Layer *layer, GContext *ctx, tm *time) {
  FContext fctx;
  GRect bounds = layer_get_unobstructed_bounds(layer);
  fctx_init_context(&fctx, ctx);

  fctx_set_text_cap_height(&fctx, s_font,
                           FIXED_TO_INT(prv_f_time_font_height(bounds)));

  int32_t f_radius = prv_f_time_font_radius(bounds);

  FPoint f_center =
      FPoint(INT_TO_FIXED(bounds.size.w / 2), INT_TO_FIXED(bounds.size.h / 2));

  FPoint f_hour_center =
      FPoint(f_center.x + sin_lookup(TEXT_ANGLE) * f_radius / TRIG_MAX_RATIO,
             f_center.y - cos_lookup(TEXT_ANGLE) * f_radius / TRIG_MAX_RATIO);

  FPoint f_min_center =
      FPoint(f_center.x - sin_lookup(TEXT_ANGLE) * f_radius / TRIG_MAX_RATIO,
             f_center.y + cos_lookup(TEXT_ANGLE) * f_radius / TRIG_MAX_RATIO);

  static char s_hour_buffer[3];
  static char s_min_buffer[3];
  strftime(s_hour_buffer, sizeof(s_hour_buffer), "%I", time);
  strftime(s_min_buffer, sizeof(s_min_buffer), "%M", time);

  prv_f_draw_text(&fctx, f_hour_center, s_hour_buffer, g_settings.hour_color);
  prv_f_draw_text(&fctx, f_min_center, s_min_buffer, g_settings.minute_color);

  fctx_deinit_context(&fctx);
}

static void prv_draw_date(Layer *layer, GContext *ctx, tm *time) {
  FContext fctx;
  GRect bounds = layer_get_unobstructed_bounds(layer);
  fctx_init_context(&fctx, ctx);

  fctx_set_text_cap_height(&fctx, s_font,
                           FIXED_TO_INT(prv_f_date_font_height(bounds)));

  int32_t f_radius = prv_f_date_font_radius(bounds);
  int32_t f_offset = prv_f_date_font_offset(bounds);
  int32_t perp_angle = TEXT_ANGLE + TRIG_MAX_ANGLE * 90 / 360;

  FPoint f_center =
      FPoint(INT_TO_FIXED(bounds.size.w / 2), INT_TO_FIXED(bounds.size.h / 2));

  FPoint f_wday_center =
      FPoint(f_center.x + sin_lookup(TEXT_ANGLE) * f_radius / TRIG_MAX_RATIO -
                 sin_lookup(perp_angle) * f_offset / TRIG_MAX_RATIO,
             f_center.y - cos_lookup(TEXT_ANGLE) * f_radius / TRIG_MAX_RATIO +
                 cos_lookup(perp_angle) * f_offset / TRIG_MAX_RATIO);

  FPoint f_mday_center =
      FPoint(f_center.x - sin_lookup(TEXT_ANGLE) * f_radius / TRIG_MAX_RATIO +
                 sin_lookup(perp_angle) * f_offset / TRIG_MAX_RATIO,
             f_center.y + cos_lookup(TEXT_ANGLE) * f_radius / TRIG_MAX_RATIO -
                 cos_lookup(perp_angle) * f_offset / TRIG_MAX_RATIO);

  static char s_mday_buffer[3];
  strftime(s_mday_buffer, sizeof(s_mday_buffer), "%d", time);

  prv_f_draw_text(&fctx, f_wday_center, s_wdays[time->tm_wday],
                  g_settings.wday_color);
  prv_f_draw_text(&fctx, f_mday_center, s_mday_buffer, g_settings.mday_color);

  fctx_deinit_context(&fctx);
}

static void prv_draw_background_stripe(Layer *layer, GContext *ctx,
                                       GColor color, bool flip) {
  FContext fctx;
  GRect bounds = layer_get_unobstructed_bounds(layer);
  fctx_init_context(&fctx, ctx);

  FPoint f_center =
      FPoint(INT_TO_FIXED(bounds.size.w / 2), INT_TO_FIXED(bounds.size.h / 2));

  FPoint f_bounds =
      FPoint(INT_TO_FIXED(bounds.size.w), INT_TO_FIXED(bounds.size.h));

  int32_t f_stripe_width = prv_f_time_font_height(bounds);
  int32_t f_padding = prv_f_time_font_radius(bounds) - f_stripe_width / 4;

  int32_t f_center_to_stripe_bottom_left_offset =
      f_center.x * cos_lookup(SLANT_ANGLE) / sin_lookup(SLANT_ANGLE);

  int32_t f_stripe_vertical_height =
      f_bounds.x * cos_lookup(SLANT_ANGLE) / sin_lookup(SLANT_ANGLE);

  int32_t f_height_offset = -f_stripe_vertical_height - f_padding;
  if (flip) {
    f_height_offset *= -1;
  }

  FPoint lower_left_point =
      FPoint(0, f_center.y + f_center_to_stripe_bottom_left_offset);

  FPoint upper_left_point = FPoint(0, lower_left_point.y + f_height_offset);

  FPoint lower_right_point =
      FPoint(f_bounds.x, lower_left_point.y - f_stripe_vertical_height);

  FPoint upper_right_point =
      FPoint(f_bounds.x, lower_right_point.y + f_height_offset);

  if (g_settings.fill_corners) {
    if (flip) {
      upper_right_point =
          FPoint(f_bounds.x, f_bounds.y + abs(upper_right_point.y));
    } else {
      upper_left_point = FPoint(0, upper_right_point.y);
    }
  }

  FPoint points[] = {upper_left_point, lower_left_point, lower_right_point,
                     upper_right_point};

  fctx_begin_fill(&fctx);
  fctx_set_fill_color(&fctx, color);
  fctx_draw_path(&fctx, points, sizeof(points) / sizeof(FPoint));
  fctx_end_fill(&fctx);

  fctx_deinit_context(&fctx);
}

static void prv_draw_background_layer(Layer *layer, GContext *ctx) {
  prv_draw_background_stripe(layer, ctx, g_settings.top_stripe_color, false);
  prv_draw_background_stripe(layer, ctx, g_settings.bottom_stripe_color, true);
}

static void prv_draw_time_layer(Layer *layer, GContext *ctx) {
  time_t now = time(NULL);
  struct tm *time = localtime(&now);

  prv_draw_time(layer, ctx, time);
  if (g_settings.show_date) {
    prv_draw_date(layer, ctx, time);
  }
}

static void prv_tick_handler(tm *_tick_time, TimeUnits _units_changed) {
  layer_mark_dirty(s_time_layer);
}

static void prv_invalidate_layers() {
  window_set_background_color(s_window, g_settings.background_color);
  layer_mark_dirty(s_time_layer);
  layer_mark_dirty(s_background_layer);
}

static void prv_window_load(Window *window) {
  layer_set_update_proc(s_time_layer, prv_draw_time_layer);
  layer_set_update_proc(s_background_layer, prv_draw_background_layer);
}

static void prv_window_unload(Window *window) {
  ffont_destroy(s_font);
  layer_destroy(s_time_layer);
  layer_destroy(s_background_layer);
}

static void prv_save_settings() {
  persist_write_data(SETTINGS_KEY, &g_settings, sizeof(g_settings));
}

static void prv_load_settings() {
  default_settings();
  persist_read_data(SETTINGS_KEY, &g_settings, sizeof(g_settings));
}

static void prv_inbox_received_callback(DictionaryIterator *iterator,
                                        void *context) {

  bool dirty = update_settings(iterator, context);
  if (dirty) {
    prv_save_settings();
    prv_invalidate_layers();
  }
}

static void
prv_unobstructed_will_change_callback(GRect final_unobstructed_screen_area,
                                      void *context) {}

static void prv_unobstructed_change_callback(AnimationProgress progress,
                                             void *context) {
  prv_invalidate_layers();
}

static void prv_unobstructed_did_change_callback(void *context) {
  prv_invalidate_layers();
}

static void prv_init(void) {
  prv_load_settings();

  app_message_register_inbox_received(prv_inbox_received_callback);

  app_message_open(256, 0);

  s_window = window_create();
  window_set_window_handlers(s_window, (WindowHandlers){
                                           .load = prv_window_load,
                                           .unload = prv_window_unload,
                                       });

  s_font = ffont_create_from_resource(RESOURCE_ID_TIMEFONT);

  Layer *window_layer = window_get_root_layer(s_window);
  GRect bounds = layer_get_bounds(window_layer);

  s_background_layer = layer_create(bounds);
  layer_add_child(window_layer, s_background_layer);

  s_time_layer = layer_create(bounds);
  layer_add_child(window_layer, s_time_layer);

  window_set_background_color(s_window, g_settings.background_color);

  UnobstructedAreaHandlers handlers = {
      .will_change = prv_unobstructed_will_change_callback,
      .change = prv_unobstructed_change_callback,
      .did_change = prv_unobstructed_did_change_callback};
  unobstructed_area_service_subscribe(handlers, NULL);

  tick_timer_service_subscribe(MINUTE_UNIT, prv_tick_handler);

  const bool animated = true;
  window_stack_push(s_window, animated);
}

static void prv_deinit(void) { window_destroy(s_window); }

int main(void) {
  prv_init();

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window: %p",
          s_window);

  app_event_loop();
  prv_deinit();
}
