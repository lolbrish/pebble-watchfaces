/*

   Code modified from fuzzy_time example from the Pebble 2.0 SDK
   Circle drawing code from just_a_bit example, also from Pebble 2.0 SDK

*/

#include "pebble.h"
#include "num2words.h"

#define BUFFER_SIZE 86
#define EXTRA_SPACE 1 // I don't know why the text seems to need 1 extra pixel?
#define STRONG_DEFAULT 15

#define CIRCLE_RADIUS 4




// a light and minimal vibe pattern. total length: 82ms
static const uint32_t const light_segments[] = {
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

// a strong a noticable vibe pattern. total length: 480ms
static const uint32_t const strong_segments[] = {
  5, 5, 5, 5,
  10, 5, 10, 5,
  20, 5, 20, 5,
  30, 10, 30, 10,
  50, 10, 50, 10,
  30, 10, 30, 10,
  20, 5, 20, 5,
  10, 5, 10, 5,
  5, 5, 5, 5
};

static VibePattern light_vibe_pattern = {
  .durations = light_segments,
  .num_segments = ARRAY_LENGTH(light_segments),
};


static VibePattern strong_vibe_pattern = {
  .durations = strong_segments,
  .num_segments = ARRAY_LENGTH(strong_segments),
};

// vibe interval in minutes
static int light_interval = 5;
static int strong_interval = STRONG_DEFAULT;
static int strong_counter = 0;
  
static struct CommonWordsData {
  TextLayer *label;
  TextLayer *label_h;
  Layer *circles_layer;
  Window *window;
  char buffer[BUFFER_SIZE];
  char buffer_b[BUFFER_SIZE];
} s_data;


void draw_circle(GContext* ctx) {
  // if strong_counter = 0
  // clear circles
  
  // else
  // draw a new circle up on the screen at location of strong_counter
  
  int16_t circle_x = 5;
  int16_t circle_y = 5;

  GPoint center = GPoint(circle_x, circle_y);
    
  graphics_context_set_fill_color(ctx, GColorWhite);
  graphics_fill_circle(ctx, center, CIRCLE_RADIUS);
}




static void update_time(struct tm* t) {
  fuzzy_time_to_words(t->tm_hour, t->tm_min, s_data.buffer, s_data.buffer_b, BUFFER_SIZE);
  text_layer_set_text(s_data.label, s_data.buffer);
  text_layer_set_text(s_data.label_h, s_data.buffer_b);
  
  // realign margins based on how much space text actually takes up
  
  GRect frame = layer_get_frame(window_get_root_layer(s_data.window));
  GSize minute_size = text_layer_get_content_size( s_data.label);
  GSize hour_size = text_layer_get_content_size(s_data.label_h);
  
  int top_margin = (frame.size.h - minute_size.h - hour_size.h)/2;
  
  layer_set_frame((Layer *)s_data.label, GRect(0, top_margin, frame.size.w, frame.size.h - top_margin));
  layer_set_frame((Layer *)s_data.label_h, GRect(0, minute_size.h + top_margin + EXTRA_SPACE, frame.size.w, frame.size.h - minute_size.h - top_margin - EXTRA_SPACE));
}

static void handle_minute_tick(struct tm *tick_time, TimeUnits units_changed) {
  // min + 2 for better fuzziness.
  // e.g. 12:43 -> "quarter till 1" which reads as such until 12:48
  if ((tick_time->tm_min+2) % light_interval == 0) {
      vibes_enqueue_custom_pattern(light_vibe_pattern);
      update_time(tick_time);
  }
  strong_counter++;
  draw_counter_circles();
  if(strong_counter == strong_interval) {
    vibes_enqueue_custom_pattern(strong_vibe_pattern);
    strong_counter = 14; // want it to go off again in 1 minute
  }
}

void accel_tap_handler(AccelAxisType axis, int32_t direction) {
  // Process tap on ACCEL_AXIS_X, ACCEL_AXIS_Y or ACCEL_AXIS_Z
  // Direction is 1 or -1
  strong_counter = 1; // reset strong vibe interval counter; watch was tapped
  draw_counter_circles();
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
  s_data.label = text_layer_create(GRect(0, 0, frame.size.w, frame.size.h));
  text_layer_set_background_color(s_data.label, GColorBlack);
  text_layer_set_text_color(s_data.label, GColorWhite);
  text_layer_set_font(s_data.label, font);
  layer_add_child(root_layer, text_layer_get_layer(s_data.label));

  // hour text
  s_data.label_h = text_layer_create(GRect(0, 0, frame.size.w, frame.size.h));
  text_layer_set_background_color(s_data.label_h, GColorBlack);
  text_layer_set_text_color(s_data.label_h, GColorWhite);
  text_layer_set_font(s_data.label_h, font_h);
  layer_add_child(root_layer, text_layer_get_layer(s_data.label_h));

  // circle counters
  s_data.circles_layer = layer_create(frame);
  layer_set_update_proc(s_data.circles_layer, &display_layer_update_callback);
  layer_add_child(root_layer, s.data_circles_layer);

  time_t now = time(NULL);
  struct tm *t = localtime(&now);
  update_time(t);

  tick_timer_service_subscribe(MINUTE_UNIT, &handle_minute_tick); 
  accel_tap_service_subscribe(&accel_tap_handler);
}

static void do_deinit(void) {
  window_destroy(s_data.window);
  text_layer_destroy(s_data.label);
  text_layer_destroy(s_data.label_h);
  layer_destroy(s_data.circles_layer);
  accel_tap_service_unsubscribe();
}

int main(void) {
  do_init();
  app_event_loop();
  do_deinit();
}
