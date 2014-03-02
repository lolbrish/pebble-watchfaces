#include "pebble.h"
#include "num2words.h"

#define BUFFER_SIZE 86

static const uint32_t const segments[] = {
  1, 5, 1, 4,
  1, 4, 1, 4,
  1, 4, 1, 3,
  1, 3, 1, 2,
  2, 2, 2, 2,
  1, 2, 1, 3,
  1, 3, 1, 4,
  1, 4, 1, 4,
  1, 4, 1, 5,
};

static VibePattern custom_vibe_pattern = {
  .durations = segments,
  .num_segments = ARRAY_LENGTH(segments),
};

static int vibe_interval = 300;

static struct CommonWordsData {
  TextLayer *label;
  Window *window;
  char buffer[BUFFER_SIZE];
} s_data;

static void update_time(struct tm* t) {
  fuzzy_time_to_words(t->tm_hour, t->tm_min, s_data.buffer, BUFFER_SIZE,
                      vibe_interval);
  text_layer_set_text(s_data.label, s_data.buffer);
}

// convenience method for learning about vibe
static void handle_second_tick(struct tm *tick_time, TimeUnits units_changed) {
  unsigned int seconds = tick_time->tm_sec + (60 * tick_time->tm_min);
  if (seconds % vibe_interval == 0) {
    vibes_enqueue_custom_pattern(custom_vibe_pattern);
  }
  if(tick_time->tm_sec == 0) {
    update_time(tick_time);
  }
}

static void handle_minute_tick(struct tm *tick_time, TimeUnits units_changed) {
  update_time(tick_time);
}

static void do_init(void) {
  s_data.window = window_create();
  const bool animated = true;
  window_stack_push(s_data.window, animated);

  window_set_background_color(s_data.window, GColorBlack);
  GFont font = fonts_get_system_font(FONT_KEY_BITHAM_30_BLACK);

  Layer *root_layer = window_get_root_layer(s_data.window);
  GRect frame = layer_get_frame(root_layer);

  s_data.label = text_layer_create(GRect(0, 20, frame.size.w,
                                         frame.size.h - 20));
  text_layer_set_background_color(s_data.label, GColorBlack);
  text_layer_set_text_color(s_data.label, GColorWhite);
  text_layer_set_font(s_data.label, font);
  layer_add_child(root_layer, text_layer_get_layer(s_data.label));

  time_t now = time(NULL);
  struct tm *t = localtime(&now);
  update_time(t);

  /* tick_timer_service_subscribe(MINUTE_UNIT, &handle_minute_tick); */
  tick_timer_service_subscribe(SECOND_UNIT, &handle_second_tick);
}

static void do_deinit(void) {
  window_destroy(s_data.window);
  text_layer_destroy(s_data.label);
}

int main(void) {
  do_init();
  app_event_loop();
  do_deinit();
}
