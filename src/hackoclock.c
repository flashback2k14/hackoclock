#include <pebble.h>

#define FIRST_ROW_WT_RECT GRect(0, 7, 144, 60)
#define SECOND_ROW_WT_RECT GRect(0, 55, 144, 60)
#define THIRD_ROW_WT_RECT GRect(0, 105, 144, 60)
#define FIRST_ROW_WOT_RECT GRect(0, 25, 144, 60)
#define SECOND_ROW_WOT_RECT GRect(0, 75, 144, 60)
	
static Window* s_window;
static TextLayer *s_hack_layer, *s_beer_layer, *s_oclock_layer, *s_time_layer;

const int NEEDED_TAPS = 4;
int tapCounter = 0;
// != 0 --> true
bool isTimeLayerShown = 1;
bool isBeerOclock = 1;

/**
 * Initialisation TextLayer
 */
static TextLayer* init_text_layer(GRect location, const char *text, GColor textColor, GColor backgroundColor, const char *textFont, GTextAlignment alignment, bool isHidden) {
  TextLayer *layer = text_layer_create(location);
	text_layer_set_text(layer, text);
  text_layer_set_text_color(layer, textColor);
  text_layer_set_background_color(layer, backgroundColor);
  text_layer_set_font(layer, fonts_get_system_font(textFont));
  text_layer_set_text_alignment(layer, alignment);
	layer_set_hidden((Layer *) layer, isHidden);
  return layer;
}

/**
 * Change TextLayer
 */
static void change_text_layer_to(TextLayer *layer, GRect location, bool isHidden) {
  layer_set_frame((Layer *) layer, location);
	layer_set_hidden((Layer *) layer, isHidden);
}

/**
 * show time layer after specific tapCounter
 */
static void show_time_layer() {
	if (isBeerOclock == 0) {
		change_text_layer_to(s_hack_layer, FIRST_ROW_WT_RECT, true);
		change_text_layer_to(s_beer_layer, FIRST_ROW_WT_RECT, false);
	} else {
		change_text_layer_to(s_hack_layer, FIRST_ROW_WT_RECT, false);
		change_text_layer_to(s_beer_layer, FIRST_ROW_WT_RECT, true);
	}
	change_text_layer_to(s_oclock_layer, SECOND_ROW_WT_RECT, false);
	change_text_layer_to(s_time_layer, THIRD_ROW_WT_RECT, false);
}

/**
 * hide time layer after specific tapCounter
 */
static void hide_time_layer() {
	if (isBeerOclock == 0) {
		change_text_layer_to(s_hack_layer, FIRST_ROW_WOT_RECT, true);
		change_text_layer_to(s_beer_layer, FIRST_ROW_WOT_RECT, false);
	} else {
		change_text_layer_to(s_hack_layer, FIRST_ROW_WOT_RECT, false);
		change_text_layer_to(s_beer_layer, FIRST_ROW_WOT_RECT, true);
	}
	change_text_layer_to(s_oclock_layer, SECOND_ROW_WOT_RECT, false);
	change_text_layer_to(s_time_layer, THIRD_ROW_WT_RECT, true);
}

/**
 * Handle Accelerometer
 */
static void tap_handler(AccelAxisType axis, int32_t direction) {
	switch (axis) {
  case ACCEL_AXIS_X:
    if (direction > 0) {
			tapCounter = tapCounter + 1;
			if (tapCounter == NEEDED_TAPS) {
				if (isTimeLayerShown == 0) {
					hide_time_layer();
					tapCounter = 0;
					isTimeLayerShown = 1;
				} else {
					show_time_layer();
					tapCounter = 0;
					isTimeLayerShown = 0;
				}				
			}
      //APP_LOG(APP_LOG_LEVEL_INFO, "X axis positive. %d", tapCounter);
    } else {
			tapCounter = tapCounter + 1;
			if (tapCounter == NEEDED_TAPS) {
				if (isTimeLayerShown == 0) {
					hide_time_layer();
					tapCounter = 0;
					isTimeLayerShown = 1;
				} else {
					show_time_layer();
					tapCounter = 0;
					isTimeLayerShown = 0;
				}				
			}
      //APP_LOG(APP_LOG_LEVEL_INFO, "X axis negative. %d", tapCounter);
    }
    break;
  case ACCEL_AXIS_Y:
    if (direction > 0) {
			tapCounter = tapCounter + 1;
			if (tapCounter == NEEDED_TAPS) {
				if (isTimeLayerShown == 0) {
					hide_time_layer();
					tapCounter = 0;
					isTimeLayerShown = 1;
				} else {
					show_time_layer();
					tapCounter = 0;
					isTimeLayerShown = 0;
				}				
			}
      //APP_LOG(APP_LOG_LEVEL_INFO, "Y axis positive. %d", tapCounter);
    } else {
			tapCounter = tapCounter + 1;
			if (tapCounter == NEEDED_TAPS) {
				if (isTimeLayerShown == 0) {
					hide_time_layer();
					tapCounter = 0;
					isTimeLayerShown = 1;
				} else {
					show_time_layer();
					tapCounter = 0;
					isTimeLayerShown = 0;
				}				
			}
      //APP_LOG(APP_LOG_LEVEL_INFO, "Y axis negative. %d", tapCounter);
    }
    break;
  case ACCEL_AXIS_Z:
    if (direction > 0) {
			tapCounter = tapCounter + 1;
			if (tapCounter == NEEDED_TAPS) {
				if (isTimeLayerShown == 0) {
					hide_time_layer();
					tapCounter = 0;
					isTimeLayerShown = 1;
				} else {
					show_time_layer();
					tapCounter = 0;
					isTimeLayerShown = 0;
				}				
			}
      //APP_LOG(APP_LOG_LEVEL_INFO, "Z axis positive. %d", tapCounter);
    } else {
			tapCounter = tapCounter + 1;
			if (tapCounter == NEEDED_TAPS) {
				if (isTimeLayerShown == 0) {
					hide_time_layer();
					tapCounter = 0;
					isTimeLayerShown = 1;
				} else {
					show_time_layer();
					tapCounter = 0;
					isTimeLayerShown = 0;
				}				
			}
      //APP_LOG(APP_LOG_LEVEL_INFO, "Z axis negative. %d", tapCounter);
    }
    break;
  }
}

/**
 * Handle Time changes
 */
static void handle_minute_tick(struct tm *tick_time, TimeUnits units_changed) {
	static char s_time_text[] = "00:00";

	if(clock_is_24h_style() == true) {
		// Use 24 hour format
		strftime(s_time_text, sizeof("00:00"), "%H:%M", tick_time);
		// Change TextLayer
		int h = tick_time->tm_hour;
		if((h >= 9) == (h < 17)) {
			layer_set_hidden((Layer *) s_hack_layer, false);
			layer_set_hidden((Layer *) s_beer_layer, true);
			isBeerOclock = 1;
		} else {
			layer_set_hidden((Layer *) s_hack_layer, true);
			layer_set_hidden((Layer *) s_beer_layer, false);
			isBeerOclock = 0;
		}
	} else {
		//use 12 hour format
		strftime(s_time_text, sizeof("00:00"), "%I:%M", tick_time);
		//change TextLayer
		int h = tick_time->tm_hour;
    if((h >= 9) == (h < 17)) {
			layer_set_hidden((Layer *) s_hack_layer, false);
			layer_set_hidden((Layer *) s_beer_layer, true);
			isBeerOclock = 1;
		} else {
			layer_set_hidden((Layer *) s_hack_layer, true);
			layer_set_hidden((Layer *) s_beer_layer, false);
			isBeerOclock = 0;
		}
	}	
	
	text_layer_set_text(s_time_layer, s_time_text);
}

static void window_load(Window *window) {
 	#ifdef PBL_COLOR
		s_hack_layer = init_text_layer(FIRST_ROW_WOT_RECT, "HACK", GColorJaegerGreen, GColorBlack, FONT_KEY_BITHAM_42_BOLD, GTextAlignmentCenter, false);
		s_beer_layer = init_text_layer(FIRST_ROW_WOT_RECT, "BEER", GColorRed, GColorBlack, FONT_KEY_BITHAM_42_BOLD, GTextAlignmentCenter, true);
		s_oclock_layer = init_text_layer(SECOND_ROW_WOT_RECT, "o'clock", GColorWhite, GColorBlack, FONT_KEY_BITHAM_42_LIGHT, GTextAlignmentCenter, false);
		s_time_layer = init_text_layer(THIRD_ROW_WT_RECT, "00:00", GColorWhite, GColorBlack, FONT_KEY_BITHAM_42_LIGHT, GTextAlignmentCenter, true);
	#else
		s_hack_layer = init_text_layer(FIRST_ROW_WOT_RECT, "HACK", GColorWhite, GColorClear, FONT_KEY_BITHAM_42_BOLD, GTextAlignmentCenter, false);
		s_beer_layer = init_text_layer(FIRST_ROW_WOT_RECT, "BEER", GColorWhite, GColorClear, FONT_KEY_BITHAM_42_BOLD, GTextAlignmentCenter, true);
		s_oclock_layer = init_text_layer(SECOND_ROW_WOT_RECT, "o'clock", GColorWhite, GColorClear, FONT_KEY_BITHAM_42_LIGHT, GTextAlignmentCenter, false);
		s_time_layer = init_text_layer(THIRD_ROW_WT_RECT, "00:00", GColorWhite, GColorClear, FONT_KEY_BITHAM_42_LIGHT, GTextAlignmentCenter, true);
	#endif	
		
	#ifdef PBL_COLOR
		window_set_background_color(s_window, GColorBlack);
	#else
		window_set_background_color(s_window, GColorBlack);
	#endif
		
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_hack_layer));
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_beer_layer));
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_oclock_layer));
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_layer));
	
	tick_timer_service_subscribe(MINUTE_UNIT, &handle_minute_tick);
	accel_tap_service_subscribe(tap_handler);
}
 
static void window_unload(Window *window) {
	text_layer_destroy(s_hack_layer);
	text_layer_destroy(s_beer_layer);
	text_layer_destroy(s_oclock_layer);
	text_layer_destroy(s_time_layer);
}
 
static void init() {
  s_window = window_create();
  WindowHandlers handlers = {
    .load = window_load,
    .unload = window_unload
  };
  window_set_window_handlers(s_window, (WindowHandlers) handlers);
  window_stack_push(s_window, true);
}
 
static void deinit() {
  window_destroy(s_window);
	accel_tap_service_unsubscribe();
}
 
int main(void) {
  init();
  app_event_loop();
  deinit();
}