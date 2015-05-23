#include <pebble.h>

#define FIRST_ROW_WT_RECT GRect(0, 7, 144, 60)
#define SECOND_ROW_WT_RECT GRect(0, 55, 144, 60)
#define THIRD_ROW_WT_RECT GRect(0, 105, 144, 60)
#define FIRST_ROW_WOT_RECT GRect(0, 25, 144, 60)
#define SECOND_ROW_WOT_RECT GRect(0, 75, 144, 60)
	
#define KEY_CD_COLOR_BG 40
#define KEY_CD_COLOR_TC 41
#define KEY_CD_COLOR_PBL 42
#define KEY_CD_COLOR_FLAGS 43
	
	
static Window* s_window;
static TextLayer *s_hack_layer, *s_beer_layer, *s_oclock_layer, *s_time_layer;
	
enum AppMessageKeys {
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

#ifdef PBL_COLOR
	typedef struct {
		char bgHack[30];
		char bgBeer[30];
		char bgOclock[30];
		char bgTime[30];
		char bgWbg[30];
	} __attribute__((__packed__)) CustomDataColorBG;

	typedef struct {
		char tcHack[30];
		char tcBeer[30];
		char tcOclock[30];
		char tcTime[30];
	} __attribute__((__packed__)) CustomDataColorTC;
#else
	typedef struct {
		char bgPblHack[12];
		char bgPblBeer[12];
		char bgPblOclock[12];
		char bgPblTime[12];
		char bgPblWbg[12];
		char tcPblHack[12];
		char tcPblBeer[12];
		char tcPblOclock[12];
		char tcPblTime[12];
	} __attribute__((__packed__)) CustomDataColorPbl;
#endif
	
typedef struct {
	char showAlwaysTime[3];
	char firstRowTextLayer[7];
	int neededTaps;
	int tapCounter;
	bool isAlwaysShownTimeActive;
	bool isTimeLayerShown;
	bool isBeerOclock;	
} __attribute__((__packed__)) CustomDataFlags;

#ifdef PBL_COLOR
	CustomDataColorBG userCDColorBG = {"", "", "", "", ""};
	CustomDataColorTC userCDColorTC = {"", "", "", ""};
#else
	CustomDataColorPbl userCDColorPbl = {"", "", "", "", "", "", "", "", ""};
#endif

CustomDataFlags userCDFlags = {"off", "Default", 4, 0, 0, 0, 0};


int NEEDED_TAPS = 4;
int tapCounter = 0;
// 0 --> false; 1 --> true
bool isAlwaysShownTimeActive = 0;
bool isTimeLayerShown = 0;
bool isBeerOclock = 0;
enum FirstRowText isAlwaysShownBeerHackDefault = DEFAULT;

static const char *NULLSTRING = "";

/**
 * DEBUG method
 */
static void DEBUG() {
	APP_LOG(APP_LOG_LEVEL_INFO, "----------- BEGIN DEBUG -----------");
	APP_LOG(APP_LOG_LEVEL_INFO, "NEEDED_TAPS: %d", NEEDED_TAPS);
	APP_LOG(APP_LOG_LEVEL_INFO, "tapCounter: %d", tapCounter);
	APP_LOG(APP_LOG_LEVEL_INFO, "isAlwaysShownTimeActive: %d", isAlwaysShownTimeActive);
	APP_LOG(APP_LOG_LEVEL_INFO, "isTimeLayerShown: %d", isTimeLayerShown);
	APP_LOG(APP_LOG_LEVEL_INFO, "isBeerOclock: %d", isBeerOclock);
	APP_LOG(APP_LOG_LEVEL_INFO, "isAlwaysShownBeerHackDefault <D=0; AB=1; AH=2>: %d", isAlwaysShownBeerHackDefault);
	APP_LOG(APP_LOG_LEVEL_INFO, "------------ END DEBUG ------------");
}


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
static void switch_text_layers_first_row(bool showMeTime, bool hideMeHack, bool hideMeBeer) {
	if (showMeTime) {
		change_text_layer_to(s_hack_layer, FIRST_ROW_WT_RECT, hideMeHack);
		change_text_layer_to(s_beer_layer, FIRST_ROW_WT_RECT, hideMeBeer);
	} else {
		change_text_layer_to(s_hack_layer, FIRST_ROW_WOT_RECT, hideMeHack);
		change_text_layer_to(s_beer_layer, FIRST_ROW_WOT_RECT, hideMeBeer);
	}
}

/**
 * show time layer after specific tapCounter
 */
static void show_time_layer() {
	if (isBeerOclock == 1) {
		switch_text_layers_first_row(true, true, false);
	} else {
		switch_text_layers_first_row(true, false, true);
	}
	change_text_layer_to(s_oclock_layer, SECOND_ROW_WT_RECT, false);
	change_text_layer_to(s_time_layer, THIRD_ROW_WT_RECT, false);
}

/**
 * hide time layer after specific tapCounter
 */
static void hide_time_layer() {
	if (isBeerOclock == 1) {
		switch_text_layers_first_row(false, true, false);
	} else {
		switch_text_layers_first_row(false, false, true);
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
			if (strcmp(t->value->cstring, "on") == 0) { 
				isAlwaysShownTimeActive = 1; 
				isTimeLayerShown = 1;
				show_time_layer(); 
			} else {
				isAlwaysShownTimeActive = 0;
			}

			strcpy(userCDFlags.showAlwaysTime, t->value->cstring);
			break;
		
		case KEY_CUSTOM_NEEDED_TAPS: 
			if (strcmp(t->value->cstring, "") != 0) { 
				NEEDED_TAPS = atoi(t->value->cstring);
				
				userCDFlags.neededTaps = NEEDED_TAPS;
			}
			break;
		
		case KEY_SHOW_ALWAYS_BEER_OR_HACK:
			if (strcmp(t->value->cstring, "Hack") == 0) {
				isAlwaysShownBeerHackDefault = ALWAYS_HACK;
				isBeerOclock = 0;
				if (isTimeLayerShown == 1) {
					switch_text_layers_first_row(true, false, true);
				}	else {
					switch_text_layers_first_row(false, false, true);
				}				
			} else if (strcmp(t->value->cstring, "Beer") == 0) {
				isAlwaysShownBeerHackDefault = ALWAYS_BEER;
				isBeerOclock = 1;
				if (isTimeLayerShown == 1) {
					switch_text_layers_first_row(true, true, false);
				} else {
					switch_text_layers_first_row(false, true, false);
				}
			}
			else {
				isAlwaysShownBeerHackDefault = DEFAULT;
			}

			strcpy(userCDFlags.firstRowTextLayer, t->value->cstring);
			break;
		
		#ifdef PBL_COLOR
			case KEY_BG_BEER: 
				setUserColor(t, s_beer_layer, true);
				strcpy(userCDColorBG.bgBeer, t->value->cstring);
				break;

			case KEY_BG_HACK: 
				setUserColor(t, s_hack_layer, true);
				strcpy(userCDColorBG.bgHack, t->value->cstring);
				break;

			case KEY_BG_OCLOCK: 
				setUserColor(t, s_oclock_layer, true);
				strcpy(userCDColorBG.bgOclock, t->value->cstring);
				break;

			case KEY_BG_TIME: 
				setUserColor(t, s_time_layer, true);
				strcpy(userCDColorBG.bgTime, t->value->cstring);
				break;

			case KEY_BG_WINBG: 
				setUserWindowBGColor(t, s_window);		
				strcpy(userCDColorBG.bgWbg, t->value->cstring);
				break;

			case KEY_TC_BEER: 
				setUserColor(t, s_beer_layer, false);
				strcpy(userCDColorTC.tcBeer, t->value->cstring);
				break;

			case KEY_TC_HACK: 
				setUserColor(t, s_hack_layer, false);
				strcpy(userCDColorTC.tcHack, t->value->cstring);
				break;

			case KEY_TC_OCLOCK: 
				setUserColor(t, s_oclock_layer, false);
				strcpy(userCDColorTC.tcOclock, t->value->cstring);
				break;

			case KEY_TC_TIME: 
				setUserColor(t, s_time_layer, false);
				strcpy(userCDColorTC.tcTime, t->value->cstring);
				break;
		#else
			case KEY_BG_PBL_BEER: 
				setUserColor(t, s_beer_layer, true);
				strcpy(userCDColorPbl.bgPblBeer, t->value->cstring);
				break;

			case KEY_BG_PBL_HACK: 
				setUserColor(t, s_hack_layer, true);
				strcpy(userCDColorPbl.bgPblHack, t->value->cstring);
				break;

			case KEY_BG_PBL_OCLOCK: 
				setUserColor(t, s_oclock_layer, true);
				strcpy(userCDColorPbl.bgPblOclock, t->value->cstring);
				break;

			case KEY_BG_PBL_TIME: 
				setUserColor(t, s_time_layer, true);
				strcpy(userCDColorPbl.bgPblTime, t->value->cstring);
				break;

			case KEY_BG_WINBG: 
				setUserWindowBGColor(t, s_window);
				strcpy(userCDColorPbl.bgPblWbg, t->value->cstring);
				break;

			case KEY_TC_PBL_BEER: 
				setUserColor(t, s_beer_layer, false);
				strcpy(userCDColorPbl.tcPblBeer, t->value->cstring);
				break;

			case KEY_TC_PBL_HACK: 
				setUserColor(t, s_hack_layer, false);
				strcpy(userCDColorPbl.tcPblHack, t->value->cstring);
				break;

			case KEY_TC_PBL_OCLOCK: 
				setUserColor(t, s_oclock_layer, false);
				strcpy(userCDColorPbl.tcPblOclock, t->value->cstring);
				break;

			case KEY_TC_PBL_TIME: 
				setUserColor(t, s_time_layer, false);
				strcpy(userCDColorPbl.tcPblTime, t->value->cstring);
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
		isTimeLayerShown = 1;
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
 */
static void handleFirstRowText(bool isWorkingTime) {
	switch(isAlwaysShownBeerHackDefault) {
		case ALWAYS_HACK:
		  if (isTimeLayerShown == 1) switch_text_layers_first_row(true, false, true);
		  else switch_text_layers_first_row(false, false, true);
			break;

		case ALWAYS_BEER:
		  if (isTimeLayerShown == 1) switch_text_layers_first_row(true, true, false);
			else switch_text_layers_first_row(false, true, false);
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
			handleFirstRowText(true);
		} else {
			handleFirstRowText(false);
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

/**
 * get user custom data
 */
static void get_user_data_from_persist() {	
	APP_LOG(APP_LOG_LEVEL_ERROR, "BEGIN get_user_data_from_persist");
	
	#ifdef PBL_COLOR
		if (persist_exists(KEY_CD_COLOR_BG)) persist_read_data(KEY_CD_COLOR_BG, &userCDColorBG, sizeof(userCDColorBG));
		else APP_LOG(APP_LOG_LEVEL_ERROR, "KEY_CD_COLOR_BG not defined!");
		if (persist_exists(KEY_CD_COLOR_TC)) persist_read_data(KEY_CD_COLOR_TC, &userCDColorTC, sizeof(userCDColorTC));
		else APP_LOG(APP_LOG_LEVEL_ERROR, "KEY_CD_COLOR_TC not defined!");
	#else
		if (persist_exists(KEY_CD_COLOR_PBL)) persist_read_data(KEY_CD_COLOR_PBL, &userCDColorPbl, sizeof(userCDColorPbl));
		else APP_LOG(APP_LOG_LEVEL_ERROR, "KEY_CD_COLOR_FLAGS not defined!");
	#endif
		
	if (persist_exists(KEY_CD_COLOR_FLAGS)) persist_read_data(KEY_CD_COLOR_FLAGS, &userCDFlags, sizeof(userCDFlags));
	else APP_LOG(APP_LOG_LEVEL_ERROR, "KEY_CD_COLOR_FLAGS not defined!");
	
	APP_LOG(APP_LOG_LEVEL_ERROR, "END get_user_data_from_persist");
}

/**
 * save user custom data
 */
static void set_user_data_to_persist() {
	APP_LOG(APP_LOG_LEVEL_ERROR, "BEGIN set_user_data_to_persist");
	
	userCDFlags.tapCounter = tapCounter;
	userCDFlags.isAlwaysShownTimeActive = isAlwaysShownTimeActive;
	userCDFlags.isTimeLayerShown = isTimeLayerShown;
	userCDFlags.isBeerOclock = isBeerOclock;
	
	#ifdef PBL_COLOR
		persist_write_data(KEY_CD_COLOR_BG, &userCDColorBG, sizeof(userCDColorBG));
		persist_write_data(KEY_CD_COLOR_TC, &userCDColorTC, sizeof(userCDColorTC));
	#else
		persist_write_data(KEY_CD_COLOR_PBL, &userCDColorPbl, sizeof(userCDColorPbl));
	#endif
	persist_write_data(KEY_CD_COLOR_FLAGS, &userCDFlags, sizeof(userCDFlags));
	
	APP_LOG(APP_LOG_LEVEL_ERROR, "END set_user_data_to_persist");
}


static void window_load(Window *window) {
		
	get_user_data_from_persist();
	
	if (strcmp(userCDColorBG.bgHack, NULLSTRING) == 0) {
		APP_LOG(APP_LOG_LEVEL_ERROR, "userCDColorBG.bgHack is empty");
	} else {
		APP_LOG(APP_LOG_LEVEL_ERROR, "userCDColorBG.bgHack is not empty");
	}
		
	#ifdef PBL_COLOR
		s_hack_layer = init_text_layer(FIRST_ROW_WOT_RECT, "HACK", GColorJaegerGreen, GColorBlack, FONT_KEY_BITHAM_42_BOLD, GTextAlignmentCenter, false);
		s_beer_layer = init_text_layer(FIRST_ROW_WOT_RECT, "BEER", GColorRed, GColorBlack, FONT_KEY_BITHAM_42_BOLD, GTextAlignmentCenter, true);
		s_oclock_layer = init_text_layer(SECOND_ROW_WOT_RECT, "o'clock", GColorWhite, GColorBlack, FONT_KEY_BITHAM_42_LIGHT, GTextAlignmentCenter, false);
		s_time_layer = init_text_layer(THIRD_ROW_WT_RECT, "00:00", GColorWhite, GColorBlack, FONT_KEY_BITHAM_42_LIGHT, GTextAlignmentCenter, true);
		window_set_background_color(s_window, GColorBlack);
	#else
		s_hack_layer = init_text_layer(FIRST_ROW_WOT_RECT, "HACK", GColorWhite, GColorClear, FONT_KEY_BITHAM_42_BOLD, GTextAlignmentCenter, false);
		s_beer_layer = init_text_layer(FIRST_ROW_WOT_RECT, "BEER", GColorWhite, GColorClear, FONT_KEY_BITHAM_42_BOLD, GTextAlignmentCenter, true);
		s_oclock_layer = init_text_layer(SECOND_ROW_WOT_RECT, "o'clock", GColorWhite, GColorClear, FONT_KEY_BITHAM_42_LIGHT, GTextAlignmentCenter, false);
		s_time_layer = init_text_layer(THIRD_ROW_WT_RECT, "00:00", GColorWhite, GColorClear, FONT_KEY_BITHAM_42_LIGHT, GTextAlignmentCenter, true);
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
	set_user_data_to_persist();
	
  window_destroy(s_window);
	accel_tap_service_unsubscribe();
}
 
int main(void) {
  init();
  app_event_loop();
  deinit();
}


/**
Setting top to                           : /tmp/tmpjuDWPf 
Setting out to                           : /tmp/tmpjuDWPf/build 
Found Pebble SDK for basalt in:          : /app/sdk3/Pebble/basalt 
Checking for program gcc,cc              : arm-none-eabi-gcc 
Checking for program ar                  : arm-none-eabi-ar 
Found Pebble SDK for aplite in:          : /app/sdk3/Pebble/aplite 
Checking for program gcc,cc              : arm-none-eabi-gcc 
Checking for program ar                  : arm-none-eabi-ar 
'configure' finished successfully (0.101s)
Waf: Entering directory `/tmp/tmpjuDWPf/build'
[ 1/35] pebble-js-app.js: src/config.js -> build/pebble-js-app.js
[ 2/35] Start build for basalt: 
[ 3/35] subst: ../../app/sdk3/Pebble/common/pebble_app.ld.template -> build/basalt/pebble_app.ld.auto
[ 4/35] timeline_resources.json: appinfo.json -> build/basalt/timeline_resources.json
[ 5/35] appinfo.auto.c: appinfo.json -> build/basalt/appinfo.auto.c
[ 6/35] icon.png: resources/images/icon.png -> build/resources/basalt/images/icon.png
[ 7/35] resource_ids.auto.h: ../../app/sdk3/Pebble/common/tools/generate_resource_code.py -> build/basalt/src/resource_ids.auto.h
[ 8/35] app_resources.pbpack.data: build/resources/basalt/images/icon.png ../../app/sdk3/Pebble/common/tools/pbpack_meta_data.py -> build/basalt/app_resources.pbpack.data
[ 9/35] app_resources.pbpack.manifest: build/resources/basalt/images/icon.png ../../app/sdk3/Pebble/common/tools/pbpack_meta_data.py -> build/basalt/app_resources.pbpack.manifest
[10/35] app_resources.pbpack.table: build/resources/basalt/images/icon.png ../../app/sdk3/Pebble/common/tools/pbpack_meta_data.py -> build/basalt/app_resources.pbpack.table
[11/35] c: build/basalt/appinfo.auto.c -> build/basalt/appinfo.auto.c.22.o
[12/35] c: src/hackoclock.c -> build/src/hackoclock.c.22.o
[13/35] app_resources.pbpack: build/basalt/app_resources.pbpack.manifest build/basalt/app_resources.pbpack.table build/basalt/app_resources.pbpack.data -> build/basalt/app_resources.pbpack
../src/hackoclock.c:110:13: warning: 'DEBUG' defined but not used [-Wunused-function]
[14/35] cprogram: build/src/hackoclock.c.22.o build/basalt/appinfo.auto.c.22.o -> build/basalt/pebble-app.elf
[15/35] Start build for aplite: 
[16/35] subst: ../../app/sdk3/Pebble/common/pebble_app.ld.template -> build/aplite/pebble_app.ld.auto
[17/35] timeline_resources.json: appinfo.json -> build/aplite/timeline_resources.json
[18/35] appinfo.auto.c: appinfo.json -> build/aplite/appinfo.auto.c
[19/35] icon.pbi: resources/images/icon.png ../../app/sdk3/Pebble/common/tools/bitmapgen.py -> build/resources/aplite/images/icon.pbi
[20/35] resource_ids.auto.h: ../../app/sdk3/Pebble/common/tools/generate_resource_code.py -> build/aplite/src/resource_ids.auto.h
[22/35] c: build/aplite/appinfo.auto.c -> build/aplite/appinfo.auto.c.23.o
[22/35] c: src/hackoclock.c -> build/src/hackoclock.c.23.o
[23/35] app_resources.pbpack.table: build/resources/aplite/images/icon.pbi ../../app/sdk3/Pebble/common/tools/pbpack_meta_data.py -> build/aplite/app_resources.pbpack.table
[24/35] app_resources.pbpack.manifest: build/resources/aplite/images/icon.pbi ../../app/sdk3/Pebble/common/tools/pbpack_meta_data.py -> build/aplite/app_resources.pbpack.manifest
../src/hackoclock.c: In function 'window_load':
../src/hackoclock.c:607:13: error: 'userCDColorBG' undeclared (first use in this function)
../src/hackoclock.c:607:13: note: each undeclared identifier is reported only once for each function it appears in
../src/hackoclock.c: At top level:
../src/hackoclock.c:110:13: warning: 'DEBUG' defined but not used [-Wunused-function]
Waf: Leaving directory `/tmp/tmpjuDWPf/build'
Build failed
 -> task in 'aplite/pebble-app.elf' failed (exit status 1): 
	{task 140485209009616: c hackoclock.c -> hackoclock.c.23.o}
['arm-none-eabi-gcc', '-std=c99', '-mcpu=cortex-m3', '-mthumb', '-ffunction-sections', '-fdata-sections', '-g', '-Os', '-Wall', '-Wextra', '-Werror', '-Wno-unused-parameter', '-Wno-error=unused-function', '-Wno-error=unused-variable', '-fPIE', '-I/tmp/app/sdk3/Pebble/aplite/include', '-I/app/sdk3/Pebble/aplite/include', '-I/tmp/tmpjuDWPf/build', '-I/tmp/tmpjuDWPf', '-I/tmp/tmpjuDWPf/build/src', '-I/tmp/tmpjuDWPf/src', '-I/tmp/tmpjuDWPf/build/aplite', '-I/tmp/tmpjuDWPf/aplite', '-DRELEASE', '-DPBL_PLATFORM_APLITE', '-DPBL_BW', '-DPBL_SDK_2', '-D__FILE_NAME__="hackoclock.c"', '../src/hackoclock.c', '-c', '-o', 'src/hackoclock.c.23.o']
[ERROR   ] A compilation error occurred
*/