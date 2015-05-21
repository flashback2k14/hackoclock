#include <pebble.h>

#define FIRST_ROW_WT_RECT GRect(0, 7, 144, 60)
#define SECOND_ROW_WT_RECT GRect(0, 55, 144, 60)
#define THIRD_ROW_WT_RECT GRect(0, 105, 144, 60)
#define FIRST_ROW_WOT_RECT GRect(0, 25, 144, 60)
#define SECOND_ROW_WOT_RECT GRect(0, 75, 144, 60)
	
enum {
	KEY_SHOW_ALWAYS_TIME = 0,
	KEY_CUSTOM_NEEDED_TAPS = 1,
	KEY_BG_BEER = 2,
	KEY_BG_HACK = 3,
	KEY_BG_OCLOCK = 4,
	KEY_BG_TIME = 5,
	KEY_BG_WINBG = 6,
	KEY_TC_BEER = 7,
	KEY_TC_HACK = 8,
	KEY_TC_OCLOCK = 9,
	KEY_TC_TIME = 10,
	KEY_BG_PBL_BEER = 11,
	KEY_BG_PBL_HACK = 12,
	KEY_BG_PBL_OCLOCK = 13,
	KEY_BG_PBL_TIME = 14,
	KEY_BG_PBL_WINBG = 15,
	KEY_TC_PBL_BEER = 16,
	KEY_TC_PBL_HACK = 17,
	KEY_TC_PBL_OCLOCK = 18,
	KEY_TC_PBL_TIME = 19,
	KEY_SHOW_ALWAYS_BEER_OR_HACK = 20
};

enum FirstRowText {
	DEFAULT = 0,
	ALWAYS_BEER = 1,
	ALWAYS_HACK = 2
};
	
static Window* s_window;
static TextLayer *s_hack_layer, *s_beer_layer, *s_oclock_layer, *s_time_layer;

int NEEDED_TAPS = 4;
int tapCounter = 0;
// 0 --> false
// 1 --> true
bool isAlwaysShownTimeActive = 0;
bool isTimeLayerShown = 0;
bool isBeerOclock = 0;
enum FirstRowText isAlwaysShownBeerHackDefault = DEFAULT;

/**
 * initialisation TextLayer
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
 * change TextLayer
 */
static void change_text_layer_to(TextLayer *layer, GRect location, bool isHidden) {
  layer_set_frame((Layer *) layer, location);
	layer_set_hidden((Layer *) layer, isHidden);
}

/**
 * switch TextLayer
 */
static void switch_text_layers(bool showMeTime, bool showMeHack, bool showMeBeer) {
	if (showMeTime) {
		change_text_layer_to(s_hack_layer, FIRST_ROW_WT_RECT, showMeHack);
		change_text_layer_to(s_beer_layer, FIRST_ROW_WT_RECT, showMeBeer);
	} else {
		change_text_layer_to(s_hack_layer, FIRST_ROW_WOT_RECT, showMeHack);
		change_text_layer_to(s_beer_layer, FIRST_ROW_WOT_RECT, showMeBeer);
	}
}

/**
 * show time layer after specific tapCounter
 */
static void show_time_layer() {
	if (isBeerOclock == 1) {
		//change_text_layer_to(s_hack_layer, FIRST_ROW_WT_RECT, true);
		//change_text_layer_to(s_beer_layer, FIRST_ROW_WT_RECT, false);
		switch_text_layers(true, true, false);
	} else {
		//change_text_layer_to(s_hack_layer, FIRST_ROW_WT_RECT, false);
		//change_text_layer_to(s_beer_layer, FIRST_ROW_WT_RECT, true);
		switch_text_layers(true, false, true);
	}
	change_text_layer_to(s_oclock_layer, SECOND_ROW_WT_RECT, false);
	change_text_layer_to(s_time_layer, THIRD_ROW_WT_RECT, false);
}

/**
 * hide time layer after specific tapCounter
 */
static void hide_time_layer() {
	if (isBeerOclock == 1) {
		//change_text_layer_to(s_hack_layer, FIRST_ROW_WOT_RECT, true);
		//change_text_layer_to(s_beer_layer, FIRST_ROW_WOT_RECT, false);
		switch_text_layers(false, true, false);
	} else {
		//change_text_layer_to(s_hack_layer, FIRST_ROW_WOT_RECT, false);
		//change_text_layer_to(s_beer_layer, FIRST_ROW_WOT_RECT, true);
		switch_text_layers(false, false, true);
	}
	change_text_layer_to(s_oclock_layer, SECOND_ROW_WOT_RECT, false);
	change_text_layer_to(s_time_layer, THIRD_ROW_WT_RECT, true);
}

/**
 * get color from string
 */
static GColor getColor(char *colorName) {
	#ifdef PBL_COLOR
		if (strcmp(colorName, "GColorInchworm") == 0) return GColorInchworm;
		else if (strcmp(colorName, "GColorBrightGreen") == 0) return GColorBrightGreen;
		else if (strcmp(colorName, "GColorSpringBud") == 0) return GColorSpringBud;
		else if (strcmp(colorName, "GColorGreen") == 0) return GColorGreen;
		else if (strcmp(colorName, "GColorMintGreen") == 0) return GColorMintGreen;
		else if (strcmp(colorName, "GColorScreaminGreen") == 0) return GColorScreaminGreen;
		else if (strcmp(colorName, "GColorMalachite") == 0) return GColorMalachite;
		else if (strcmp(colorName, "GColorIslamicGreen") == 0) return GColorIslamicGreen;
		else if (strcmp(colorName, "GColorKellyGreen") == 0) return GColorKellyGreen;
		else if (strcmp(colorName, "GColorBrass") == 0) return GColorBrass;
		else if (strcmp(colorName, "GColorLimerick") == 0) return GColorLimerick;
		else if (strcmp(colorName, "GColorYellow") == 0) return GColorYellow;
		else if (strcmp(colorName, "GColorIcterine") == 0) return GColorIcterine;
		else if (strcmp(colorName, "GColorPastelYellow") == 0) return GColorPastelYellow;
		else if (strcmp(colorName, "GColorRajah") == 0) return GColorRajah;
		else if (strcmp(colorName, "GColorChromeYellow") == 0) return GColorChromeYellow;
		else if (strcmp(colorName, "GColorOrange") == 0) return GColorOrange;
		else if (strcmp(colorName, "GColorMelon") == 0) return GColorMelon;
		else if (strcmp(colorName, "GColorRed") == 0) return GColorRed;
		else if (strcmp(colorName, "GColorWindsorTan") == 0) return GColorWindsorTan;
		else if (strcmp(colorName, "GColorArmyGreen") == 0) return GColorArmyGreen;
		else if (strcmp(colorName, "GColorDarkGreen") == 0) return GColorDarkGreen;
		else if (strcmp(colorName, "GColorMayGreen") == 0) return GColorMayGreen;
		else if (strcmp(colorName, "GColorJaegerGreen") == 0) return GColorJaegerGreen;
		else if (strcmp(colorName, "GColorMediumAquamarine") == 0) return GColorMediumAquamarine;
		else if (strcmp(colorName, "GColorMediumSpringGreen") == 0) return GColorMediumSpringGreen;
		else if (strcmp(colorName, "GColorTiffanyBlue") == 0) return GColorTiffanyBlue;
		else if (strcmp(colorName, "GColorCadetBlue") == 0) return GColorCadetBlue;
		else if (strcmp(colorName, "GColorMidnightGreen") == 0) return GColorMidnightGreen;
		else if (strcmp(colorName, "GColorRoseVale") == 0) return GColorRoseVale;
		else if (strcmp(colorName, "GColorFolly") == 0) return GColorFolly;
		else if (strcmp(colorName, "GColorDarkCandyAppleRed") == 0) return GColorDarkCandyAppleRed;
		else if (strcmp(colorName, "GColorBulgarianRose") == 0) return GColorBulgarianRose;
		else if (strcmp(colorName, "GColorCobaltBlue") == 0) return GColorCobaltBlue;
		else if (strcmp(colorName, "GColorCyan") == 0) return GColorCyan;
		else if (strcmp(colorName, "GColorElectricBlue") == 0) return GColorElectricBlue;
		else if (strcmp(colorName, "GColorCeleste") == 0) return GColorCeleste;
		else if (strcmp(colorName, "GColorVividCerulean") == 0) return GColorVividCerulean;
		else if (strcmp(colorName, "GColorDukeBlue") == 0) return GColorDukeBlue;
		else if (strcmp(colorName, "GColorOxfordBlue") == 0) return GColorOxfordBlue;
		else if (strcmp(colorName, "GColorImperialPurple") == 0) return GColorImperialPurple;
		else if (strcmp(colorName, "GColorJazzberryJam") == 0) return GColorJazzberryJam;
		else if (strcmp(colorName, "GColorFashionMagenta") == 0) return GColorFashionMagenta;
		else if (strcmp(colorName, "GColorBrilliantRose") == 0) return GColorBrilliantRose;
		else if (strcmp(colorName, "GColorMagenta") == 0) return GColorMagenta;
		else if (strcmp(colorName, "GColorPurple") == 0) return GColorPurple;
		else if (strcmp(colorName, "GColorIndigo") == 0) return GColorIndigo;
		else if (strcmp(colorName, "GColorElectricUltramarine") == 0) return GColorElectricUltramarine;
		else if (strcmp(colorName, "GColorBlue") == 0) return GColorBlue;
		else if (strcmp(colorName, "GColorPictonBlue") == 0) return GColorPictonBlue;
		else if (strcmp(colorName, "GColorBlueMoon") == 0) return GColorBlueMoon;
		else if (strcmp(colorName, "GColorVeryLightBlue") == 0) return GColorVeryLightBlue;
		else if (strcmp(colorName, "GColorLiberty") == 0) return GColorLiberty;
		else if (strcmp(colorName, "GColorVividViolet") == 0) return GColorVividViolet;
		else if (strcmp(colorName, "GColorPurpureus") == 0) return GColorPurpureus;
		else if (strcmp(colorName, "GColorShockingPink") == 0) return GColorShockingPink;
		else if (strcmp(colorName, "GColorRichBrilliantLavender") == 0) return GColorRichBrilliantLavender;
		else if (strcmp(colorName, "GColorBabyBlueEyes") == 0) return GColorBabyBlueEyes;
		else if (strcmp(colorName, "GColorLavenderIndigo") == 0) return GColorLavenderIndigo;
		else if (strcmp(colorName, "GColorWhite") == 0) return GColorWhite;
		else if (strcmp(colorName, "GColorLightGray") == 0) return GColorLightGray;
		else if (strcmp(colorName, "GColorDarkGray") == 0) return GColorDarkGray;
		else if (strcmp(colorName, "GColorBlack") == 0) return GColorBlack;
		else return GColorClear;
	#else
		if (strcmp(colorName, "GColorWhite") == 0) return GColorWhite;
		else if (strcmp(colorName, "GColorClear") == 0) return GColorClear;
		else return GColorBlack;
	#endif
}

/**
 * set Color to Text or Background
 */
static void setUserColor(Tuple *t, TextLayer *layer, bool isBackground) {
	char colorNameFromTuple[30];
	GColor userColor;
	
	strcpy(colorNameFromTuple, t->value->cstring);
	userColor = getColor(colorNameFromTuple);
		
	if (isBackground) text_layer_set_background_color(layer, userColor);
	else text_layer_set_text_color(layer, userColor);
}

/**
 * set Window Background Color
 */
static void setUserWindowBGColor(Tuple *t, Window *window) {
	char colorNameFromTuple[30];
	GColor userColor;	
	
	strcpy(colorNameFromTuple, t->value->cstring);
	userColor = getColor(colorNameFromTuple);		
	window_set_background_color(window, userColor);
}

/**
 * customise watchface
 */
static void perform_customisation(Tuple *t) {
	
	switch(t->key) {
		case KEY_SHOW_ALWAYS_TIME: 
			if (strcmp(t->value->cstring, "on") == 0) { isAlwaysShownTimeActive = 1; show_time_layer(); }
			else isAlwaysShownTimeActive = 0;
			break;
		
		case KEY_CUSTOM_NEEDED_TAPS: 
			if (strcmp(t->value->cstring, "") != 0) NEEDED_TAPS = atoi(t->value->cstring);
			break;
		
		case KEY_SHOW_ALWAYS_BEER_OR_HACK:
			if (strcmp(t->value->cstring, "Hack") == 0) isAlwaysShownBeerHackDefault = ALWAYS_HACK;
			else if (strcmp(t->value->cstring, "Beer") == 0) isAlwaysShownBeerHackDefault = ALWAYS_BEER;
			else isAlwaysShownBeerHackDefault = DEFAULT;		
			break;
		
		#ifdef PBL_COLOR
			case KEY_BG_BEER: 
				setUserColor(t, s_beer_layer, true);
				break;

			case KEY_BG_HACK: 
				setUserColor(t, s_hack_layer, true);
				break;

			case KEY_BG_OCLOCK: 
				setUserColor(t, s_oclock_layer, true);
				break;

			case KEY_BG_TIME: 
				setUserColor(t, s_time_layer, true);
				break;

			case KEY_BG_WINBG: 
				setUserWindowBGColor(t, s_window);		
				break;

			case KEY_TC_BEER: 
				setUserColor(t, s_beer_layer, false);
				break;

			case KEY_TC_HACK: 
				setUserColor(t, s_hack_layer, false);
				break;

			case KEY_TC_OCLOCK: 
				setUserColor(t, s_oclock_layer, false);
				break;

			case KEY_TC_TIME: 
				setUserColor(t, s_time_layer, false);
				break;
		#else
			case KEY_BG_PBL_BEER: 
				setUserColor(t, s_beer_layer, true);
				break;

			case KEY_BG_PBL_HACK: 
				setUserColor(t, s_hack_layer, true);
				break;

			case KEY_BG_PBL_OCLOCK: 
				setUserColor(t, s_oclock_layer, true);
				break;

			case KEY_BG_PBL_TIME: 
				setUserColor(t, s_time_layer, true);
				break;

			case KEY_BG_WINBG: 
				setUserWindowBGColor(t, s_window);
				break;

			case KEY_TC_PBL_BEER: 
				setUserColor(t, s_beer_layer, false);
				break;

			case KEY_TC_PBL_HACK: 
				setUserColor(t, s_hack_layer, false);
				break;

			case KEY_TC_PBL_OCLOCK: 
				setUserColor(t, s_oclock_layer, false);
				break;

			case KEY_TC_PBL_TIME: 
				setUserColor(t, s_time_layer, false);
				break;
		#endif
		
		default:
			APP_LOG(APP_LOG_LEVEL_ERROR, "Key %d not recognized!", (int)t->key);
	}
}

/**
 * handle customisation AppMessage
 */
static void in_recv_handler(DictionaryIterator *iterator, void *context) {
	Tuple *t = dict_read_first(iterator);
	
	while(t != NULL) {
		perform_customisation(t);	
		t = dict_read_next(iterator);
	}
}

/**
 * handle tap count & show / hide time layer
 */
static void handleTimeLayerState() {
	tapCounter = tapCounter + 1;
	
	if (isAlwaysShownTimeActive == 1) {
		show_time_layer();
		tapCounter = 0;
	} else {
		if (tapCounter == NEEDED_TAPS) {
			if (isTimeLayerShown == 1) {
				hide_time_layer();
				tapCounter = 0;
				isTimeLayerShown = 0;
			} else {
				show_time_layer();
				tapCounter = 0;
				isTimeLayerShown = 1;
			}				
		}
	}
}

/**
 * handle accelerometer
 */
static void tap_handler(AccelAxisType axis, int32_t direction) {
	switch (axis) {
  case ACCEL_AXIS_X:
    if (direction > 0) {
			handleTimeLayerState();
    } else {
			handleTimeLayerState();
    }
    break;
  case ACCEL_AXIS_Y:
    if (direction > 0) {
			handleTimeLayerState();
    } else {
			handleTimeLayerState();
    }
    break;
  case ACCEL_AXIS_Z:
    if (direction > 0) {
			handleTimeLayerState();
    } else {
			handleTimeLayerState();
    }
    break;
  }
}

/**
 * handle first row text
 *
 * show_time_layer müsste noch probleme bereiten bei ALWAYS_HACK, ALWAYS_BEER
 *
 */
static void handleFirstRowText(bool isWorkingTime) {
	switch(isAlwaysShownBeerHackDefault) {
		case ALWAYS_HACK:
			//layer_set_hidden((Layer *) s_hack_layer, false);
			//layer_set_hidden((Layer *) s_beer_layer, true);
		  switch_text_layers(false, false, true);
		  isBeerOclock = 0;
			break;

		case ALWAYS_BEER:
			//layer_set_hidden((Layer *) s_hack_layer, true);
			//layer_set_hidden((Layer *) s_beer_layer, false);
		  switch_text_layers(false, true, false);
		  isBeerOclock = 1;
			break;

		default:
			if (isWorkingTime) {
				layer_set_hidden((Layer *) s_hack_layer, false);
				layer_set_hidden((Layer *) s_beer_layer, true);
				isBeerOclock = 0;
			} else {
				layer_set_hidden((Layer *) s_hack_layer, true);
				layer_set_hidden((Layer *) s_beer_layer, false);
				isBeerOclock = 1;
			}
	}
}

/**
 * handle time changes
 */
static void handle_minute_tick(struct tm *tick_time, TimeUnits units_changed) {
	static char s_time_text[] = "00:00";

	if(clock_is_24h_style() == true) {
		// use 24 hour format
		strftime(s_time_text, sizeof("00:00"), "%H:%M", tick_time);
		// change textLayer
		int h = tick_time->tm_hour;
		if((h >= 9) == (h < 17)) {
			//layer_set_hidden((Layer *) s_hack_layer, false);
			//layer_set_hidden((Layer *) s_beer_layer, true);
			handleFirstRowText(true);
			isBeerOclock = 0;
		} else {
			//layer_set_hidden((Layer *) s_hack_layer, true);
			//layer_set_hidden((Layer *) s_beer_layer, false);
			handleFirstRowText(false);
			isBeerOclock = 1;
		}
	} else {
		//use 12 hour format
		strftime(s_time_text, sizeof("00:00"), "%I:%M", tick_time);
		//change TextLayer
		int h = tick_time->tm_hour;
    if((h >= 9) == (h < 17)) {
			handleFirstRowText(true);		
		} else {
			handleFirstRowText(false); 
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
	
	app_message_register_inbox_received((AppMessageInboxReceived) in_recv_handler);
	app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
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