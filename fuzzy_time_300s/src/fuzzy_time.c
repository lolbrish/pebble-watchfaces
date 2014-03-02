#include "pebble.h"
#include "num2words.h"

#define BUFFER_SIZE 86
#define TOP_MARGIN 15
#define EXTRA_SPACE 1 // I don't know why the text seems to need 1 extra pixel?

// a light and minimal vibe pattern. total length: 82ms
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

// vibe interval in minutes
static int vibe_interval = 5;
  
static struct CommonWordsData {
  TextLayer *label;
  TextLayer *label_h;
  Window *window;
  char buffer[BUFFER_SIZE];
  char buffer_b[BUFFER_SIZE];
} s_data;

static void update_time(struct tm* t) {
  fuzzy_time_to_words(t->tm_hour, t->tm_min, s_data.buffer, s_data.buffer_b, BUFFER_SIZE);
  text_layer_set_text(s_data.label, s_data.buffer);
  
  // resize hour text depending on how much space minute text is taking up
  GRect frame = layer_get_frame(window_get_root_layer(s_data.window));
  GSize minute_size = text_layer_get_content_size( s_data.label);
  
  layer_set_frame((Layer *)s_data.label_h, GRect(0, TOP_MARGIN + minute_size.h + EXTRA_SPACE, frame.size.w, frame.size.h - TOP_MARGIN - minute_size.h - EXTRA_SPACE));
  text_layer_set_text(s_data.label_h, s_data.buffer_b);
}

static void handle_minute_tick(struct tm *tick_time, TimeUnits units_changed) {
  // min + 2 for better fuzziness.
  // e.g. 12:43 -> "quarter till 1" which reads as such until 12:48
  if ((tick_time->tm_min+2) % vibe_interval == 0) {
      vibes_enqueue_custom_pattern(custom_vibe_pattern);
      update_time(tick_time);
  }
}

static void do_init(void) {
  s_data.window = window_create();
  const bool animated = true;
  window_stack_push(s_data.window, animated);

  window_set_background_color(s_data.window, GColorBlack);
  
  GFont font = fonts_get_system_font(FONT_KEY_BITHAM_42_LIGHT);
  GFont font_h = fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD);

  Layer *root_layer = window_get_root_layer(s_data.window);
  
  GRect frame = layer_get_frame(root_layer);
  
  // minute text
  s_data.label = text_layer_create(GRect(0, TOP_MARGIN, frame.size.w,
                                         frame.size.h - TOP_MARGIN));
  text_layer_set_background_color(s_data.label, GColorBlack);
  text_layer_set_text_color(s_data.label, GColorWhite);
  text_layer_set_font(s_data.label, font);
  layer_add_child(root_layer, text_layer_get_layer(s_data.label));


  s_data.label_h = text_layer_create(GRect(0, TOP_MARGIN, frame.size.w,
                                         frame.size.h - TOP_MARGIN));
  text_layer_set_background_color(s_data.label_h, GColorBlack);
  text_layer_set_text_color(s_data.label_h, GColorWhite);
  text_layer_set_font(s_data.label_h, font_h);
  layer_add_child(root_layer, text_layer_get_layer(s_data.label_h));


  time_t now = time(NULL);
  struct tm *t = localtime(&now);
  update_time(t);

  tick_timer_service_subscribe(MINUTE_UNIT, &handle_minute_tick); 
}

static void do_deinit(void) {
  window_destroy(s_data.window);
  text_layer_destroy(s_data.label);
   text_layer_destroy(s_data.label_h);
}

int main(void) {
  do_init();
  app_event_loop();
  do_deinit();
}
