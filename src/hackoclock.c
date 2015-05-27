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
#define KEY_CUSTOM_DATA 44
	
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
	KEY_SHOW_ALWAYS_BEER_OR_HACK = 20,
	KEY_CUSTOM_BEGIN_TIME = 21,
	KEY_CUSTOM_END_TIME = 22,
	KEY_CUSTOM_HACK_TEXT = 23,
	KEY_CUSTOM_BEER_TEXT = 24
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
	int neededTaps;
	int tapCounter;
	bool isAlwaysShownTimeActive;
	bool isTimeLayerShown;
	bool isBeerOclock;
	enum FirstRowText isAlwaysShownBeerHackDefault;
} __attribute__((__packed__)) CustomDataFlags;

typedef struct {
	int customBeginHour;
	int customEndHour;
	char customHackText[6];
	char customBeerText[6];
} __attribute__((__packed__)) CustomData;

#ifdef PBL_COLOR
	CustomDataColorBG userCDColorBG = {"", "", "", "", ""};
	CustomDataColorTC userCDColorTC = {"", "", "", ""};
#else
	CustomDataColorPbl userCDColorPbl = {"", "", "", "", "", "", "", "", ""};
#endif

CustomDataFlags userCDFlags = {"off", 4, 0, 0, 0, 0, DEFAULT};
CustomData userCustomData = {9, 17, "HACK", "BEER"};

/**
 * Workaround for needed Taps always 0
 * after perform customisation
 */
bool isNeededTapsDirty = 0;
int NEEDED_TAPS = 4;
int TMP_NEEDED_TAPS = 0;

bool isBeginHourDirty = 0;
int BEGIN_HOUR = 9;
int TMP_BEGIN_HOUR = 0;

bool isEndHourDirty = 0;
int END_HOUR = 17;
int TMP_END_HOUR = 0;

/*bool isHackTextDirty = 0;
char TMP_HACK_TEXT[6];
bool isBeerTextDirty = 0;
char TMP_BEER_TEXT[6];*/


//########################################################################################//
//																																												//
// 																	DEBUG METHODS																					//
//																																												//
//########################################################################################//
#ifdef PBL_COLOR
static void DEBUG_STRUCT_COLOR_BG() {
	APP_LOG(APP_LOG_LEVEL_INFO, "----------- BEGIN DEBUG_STRUCT_COLOR_BG -----------");
	APP_LOG(APP_LOG_LEVEL_INFO, "userCDColorBG.bgHack: %s", userCDColorBG.bgHack);
	APP_LOG(APP_LOG_LEVEL_INFO, "userCDColorBG.bgBeer: %s", userCDColorBG.bgBeer);
	APP_LOG(APP_LOG_LEVEL_INFO, "userCDColorBG.bgOclock: %s", userCDColorBG.bgOclock);
	APP_LOG(APP_LOG_LEVEL_INFO, "userCDColorBG.bgTime: %s", userCDColorBG.bgTime);
	APP_LOG(APP_LOG_LEVEL_INFO, "userCDColorBG.bgWbg: %s", userCDColorBG.bgWbg);
	APP_LOG(APP_LOG_LEVEL_INFO, "------------ END DEBUG_STRUCT_COLOR_BG ------------");	
}
#endif

static void DEBUG_STRUCT_FLAGS() {
	APP_LOG(APP_LOG_LEVEL_INFO, "----------- BEGIN DEBUG_STRUCT_FLAGS -----------");
	APP_LOG(APP_LOG_LEVEL_INFO, "showAlwaysTime: %s", userCDFlags.showAlwaysTime);
	APP_LOG(APP_LOG_LEVEL_INFO, "neededTaps / tapCounter: %d / %d", userCDFlags.neededTaps, userCDFlags.tapCounter);
	APP_LOG(APP_LOG_LEVEL_INFO, "isAlwaysShownTimeActive / isTimeLayerShown: %d / %d", userCDFlags.isAlwaysShownTimeActive, userCDFlags.isTimeLayerShown);
	APP_LOG(APP_LOG_LEVEL_INFO, "isBeerOclock: %d", userCDFlags.isBeerOclock);
	APP_LOG(APP_LOG_LEVEL_INFO, "isAlwaysShownBeerHackDefault <D=0; AB=1; AH=2>: %d", userCDFlags.isAlwaysShownBeerHackDefault);
	APP_LOG(APP_LOG_LEVEL_INFO, "working Time: %d To %d", userCustomData.customBeginHour, userCustomData.customEndHour);
	//APP_LOG(APP_LOG_LEVEL_INFO, "first row Text: %s and %s", userCustomData.customHackText, userCustomData.customBeerText);
	APP_LOG(APP_LOG_LEVEL_INFO, "------------ END DEBUG_STRUCT_FLAGS ------------");	
}	

//########################################################################################//
//																																												//
// 																CHANGE UI METHODS																				//
//																																												//
//########################################################################################//
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
 * set text to layer
 */
static void set_text_to_layer(TextLayer *layer, const char *text) {
	text_layer_set_text(layer, text);
}
/**
 * change TextLayer
 */
static void change_text_layer_to(TextLayer *layer, GRect location, bool isHidden) {
  layer_set_frame((Layer *) layer, location);
	layer_set_hidden((Layer *) layer, isHidden);
}
/**
 * handle time layer 
 */
static void handle_time_layer(bool showTimeLayer) {
	if (showTimeLayer) {
		change_text_layer_to(s_oclock_layer, SECOND_ROW_WT_RECT, false);
		change_text_layer_to(s_time_layer, THIRD_ROW_WT_RECT, false);
	} else {
		change_text_layer_to(s_oclock_layer, SECOND_ROW_WOT_RECT, false);
		change_text_layer_to(s_time_layer, THIRD_ROW_WT_RECT, true);
	}
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
	if (userCDFlags.isBeerOclock == 1) {
		switch_text_layers_first_row(true, true, false);
	} else {
		switch_text_layers_first_row(true, false, true);
	}
	handle_time_layer(true);
}
/**
 * hide time layer after specific tapCounter
 */
static void hide_time_layer() {
	if (userCDFlags.isBeerOclock == 1) {
		switch_text_layers_first_row(false, true, false);
	} else {
		switch_text_layers_first_row(false, false, true);
	}
	handle_time_layer(false);
}
/**
 * show / hide time layer
 */
static void time_layer_state(bool showTime) {
	if (showTime) {
		show_time_layer();
		userCDFlags.tapCounter = 0;
		userCDFlags.isTimeLayerShown = 1;
	} else {
		hide_time_layer();
		userCDFlags.tapCounter = 0;
		userCDFlags.isTimeLayerShown = 0;
	}
}
/**
 * change first row text layer
 */
static void handle_first_row_text_layer(enum FirstRowText e, bool setBeerOclock, bool hideHack, bool hideBeer) {
	userCDFlags.isAlwaysShownBeerHackDefault = e;
	userCDFlags.isBeerOclock = setBeerOclock;
	if (userCDFlags.isTimeLayerShown == 1) switch_text_layers_first_row(true, hideHack, hideBeer);
	else switch_text_layers_first_row(false, hideHack, hideBeer);
}

//########################################################################################//
//																																												//
// 																CHANGE COLOR METHODS																		//
//																																												//
//########################################################################################//
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
	strcpy(colorNameFromTuple, t->value->cstring);
		
	if (isBackground) text_layer_set_background_color(layer, getColor(colorNameFromTuple));
	else text_layer_set_text_color(layer, getColor(colorNameFromTuple));
}
/**
 * set Window Background Color
 */
static void setUserWindowBGColor(Tuple *t, Window *window) {
	char colorNameFromTuple[30];
	strcpy(colorNameFromTuple, t->value->cstring);
	window_set_background_color(window, getColor(colorNameFromTuple));
}
/**
 * get color from user customisation
 */
static GColor get_user_color(GColor defaultColor, char *colorName) {
	return (strcmp(colorName, "") == 0) ? defaultColor : getColor(colorName);
}

//########################################################################################//
//																																												//
// 															CUSTOMISATION METHODS																			//
//																																												//
//########################################################################################//
/**
 * customise watchface
 */
static void perform_customisation(Tuple *t) {
	switch(t->key) {
		case KEY_SHOW_ALWAYS_TIME: 
			if (strcmp(t->value->cstring, "on") == 0) { 
				userCDFlags.isAlwaysShownTimeActive = 1; 
				userCDFlags.isTimeLayerShown = 1;
				show_time_layer(); 
			} else {
				userCDFlags.isAlwaysShownTimeActive = 0;
				userCDFlags.isTimeLayerShown = 0;
				hide_time_layer(); 
			}
			strcpy(userCDFlags.showAlwaysTime, t->value->cstring);
			break;
		
		case KEY_CUSTOM_NEEDED_TAPS: 
			if (strcmp(t->value->cstring, "") == 0) { 
				isNeededTapsDirty = 0;
			} else {
				isNeededTapsDirty = 1;
				TMP_NEEDED_TAPS = atoi(t->value->cstring);
			}
			break;
		
		case KEY_SHOW_ALWAYS_BEER_OR_HACK:
			if (strcmp(t->value->cstring, "Hack") == 0) {
				handle_first_row_text_layer(ALWAYS_HACK, 0, false, true);		
			} else if (strcmp(t->value->cstring, "Beer") == 0) {
				handle_first_row_text_layer(ALWAYS_BEER, 1, true, false);		
			}	else {
				userCDFlags.isAlwaysShownBeerHackDefault = DEFAULT;
			}
			break;
		
		case KEY_CUSTOM_BEGIN_TIME:
			if (strcmp(t->value->cstring, "") == 0) { 
				isBeginHourDirty = 0;
			} else {
				isBeginHourDirty = 1;
				TMP_BEGIN_HOUR = atoi(t->value->cstring);
			}
			break;
		
		case KEY_CUSTOM_END_TIME:
			if (strcmp(t->value->cstring, "") == 0) { 
				isEndHourDirty = 0;
			} else {
				isEndHourDirty = 1;
				TMP_END_HOUR = atoi(t->value->cstring);
			}
			break;	
		
		/*case KEY_CUSTOM_HACK_TEXT:
			if (strcmp(t->value->cstring, "") != 0) { 
				APP_LOG(APP_LOG_LEVEL_INFO, "KEY_CUSTOM_HACK_TEXT: t->value->cstring: %s", t->value->cstring);
				APP_LOG(APP_LOG_LEVEL_INFO, "KEY_CUSTOM_HACK_TEXT: userCustomData.customHackText: %s", userCustomData.customHackText);
				strcpy(userCustomData.customHackText, t->value->cstring);
				APP_LOG(APP_LOG_LEVEL_INFO, "KEY_CUSTOM_HACK_TEXT: userCustomData.customHackText: %s", userCustomData.customHackText);
			}
			break;

		case KEY_CUSTOM_BEER_TEXT:
			if (strcmp(t->value->cstring, "") != 0) { 
				APP_LOG(APP_LOG_LEVEL_INFO, "KEY_CUSTOM_BEER_TEXT: t->value->cstring: %s", t->value->cstring);
				APP_LOG(APP_LOG_LEVEL_INFO, "KEY_CUSTOM_BEER_TEXT: userCustomData.customBeerText: %s", userCustomData.customBeerText);
				strcpy(userCustomData.customBeerText, t->value->cstring);
				APP_LOG(APP_LOG_LEVEL_INFO, "KEY_CUSTOM_BEER_TEXT: userCustomData.customBeerText: %s", userCustomData.customBeerText);
			}
			break;
		
		case KEY_CUSTOM_HACK_TEXT:
			if (strcmp(t->value->cstring, "") == 0) { 
				isHackTextDirty = 0;
			} else {
				isHackTextDirty = 1;
				strcpy(TMP_HACK_TEXT, t->value->cstring);
			}
			break;

		case KEY_CUSTOM_BEER_TEXT:
			if (strcmp(t->value->cstring, "") == 0) { 
				isBeerTextDirty = 0;
			} else {
				isBeerTextDirty = 1;
				strcpy(TMP_BEER_TEXT, t->value->cstring);
			}
			break;*/
		
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

			case KEY_BG_PBL_WINBG: 
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
	
	if (isNeededTapsDirty == 1) userCDFlags.neededTaps = TMP_NEEDED_TAPS;
	else userCDFlags.neededTaps = NEEDED_TAPS;
	
	if (isBeginHourDirty == 1) userCustomData.customBeginHour = TMP_BEGIN_HOUR;
	else userCustomData.customBeginHour = BEGIN_HOUR;
	
	if (isEndHourDirty == 1) userCustomData.customEndHour = TMP_END_HOUR;
	else userCustomData.customEndHour = END_HOUR;
		
	/*if (isHackTextDirty == 1) strcpy(userCustomData.customHackText, TMP_HACK_TEXT);
	else strcpy(userCustomData.customHackText, "HACK");
	
	if (isBeerTextDirty == 1) strcpy(userCustomData.customBeerText, TMP_BEER_TEXT);
	else strcpy(userCustomData.customBeerText, "BEER");
	
	set_text_to_layer(s_hack_layer, userCustomData.customHackText);
	set_text_to_layer(s_beer_layer, userCustomData.customBeerText);*/
}

//########################################################################################//
//																																												//
// 																		HANDLER METHODS																			//
//																																												//
//########################################################################################//
/**
 * handle tap count & show / hide time layer
 */
static void handle_time_layer_state() {
	userCDFlags.tapCounter++;
	
	if (userCDFlags.isAlwaysShownTimeActive == 1) {
		time_layer_state(true);
	} else {
		if (userCDFlags.tapCounter == userCDFlags.neededTaps) {
			if (userCDFlags.isTimeLayerShown == 1) {
				time_layer_state(false);
			} else {
				time_layer_state(true);
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
			handle_time_layer_state();
    } else {
			handle_time_layer_state();
    }
    break;
  case ACCEL_AXIS_Y:
    if (direction > 0) {
			handle_time_layer_state();
    } else {
			handle_time_layer_state();
    }
    break;
  case ACCEL_AXIS_Z:
    if (direction > 0) {
			handle_time_layer_state();
    } else {
			handle_time_layer_state();
    }
    break;
  }
}
/**
 * handle first row text
 */
static void handle_first_row_text(bool isWorkingTime) {
	switch(userCDFlags.isAlwaysShownBeerHackDefault) {
		case ALWAYS_HACK:
		  if (userCDFlags.isTimeLayerShown == 1) switch_text_layers_first_row(true, false, true);
		  else switch_text_layers_first_row(false, false, true);
			break;

		case ALWAYS_BEER:
		  if (userCDFlags.isTimeLayerShown == 1) switch_text_layers_first_row(true, true, false);
			else switch_text_layers_first_row(false, true, false);
			break;

		default:
			if (isWorkingTime) {
				if (userCDFlags.isTimeLayerShown == 1) switch_text_layers_first_row(true, false, true);
				else switch_text_layers_first_row(false, false, true);
				userCDFlags.isBeerOclock = 0;
			} else {
				if (userCDFlags.isTimeLayerShown == 1) switch_text_layers_first_row(true, true, false);
				else switch_text_layers_first_row(false, true, false);
				userCDFlags.isBeerOclock = 1;
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
		if((h >= userCustomData.customBeginHour) == (h < userCustomData.customEndHour)) {
			handle_first_row_text(true);
		} else {
			handle_first_row_text(false);
		}
	} else {
		//use 12 hour format
		strftime(s_time_text, sizeof("00:00"), "%I:%M", tick_time);
		//change TextLayer
		int h = tick_time->tm_hour;
    if((h >= userCustomData.customBeginHour) == (h < userCustomData.customEndHour)) {
			handle_first_row_text(true);
		} else {
			handle_first_row_text(false);
		}
	}	
	text_layer_set_text(s_time_layer, s_time_text);
}

//########################################################################################//
//																																												//
// 																		PERSIST METHODS																			//
//																																												//
//########################################################################################//
/**
 * get user custom data
 */
static void get_user_data_from_persist() {	
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
	
	if (persist_exists(KEY_CUSTOM_DATA)) persist_read_data(KEY_CUSTOM_DATA, &userCustomData, sizeof(userCustomData));
	else APP_LOG(APP_LOG_LEVEL_ERROR, "KEY_CD_COLOR_FLAGS not defined!");
}
/**
 * save user custom data
 */
static void set_user_data_to_persist() { 	
	//DEBUG_STRUCT_FLAGS();
	
	#ifdef PBL_COLOR
		persist_write_data(KEY_CD_COLOR_BG, &userCDColorBG, sizeof(userCDColorBG));
		persist_write_data(KEY_CD_COLOR_TC, &userCDColorTC, sizeof(userCDColorTC));
	#else
		persist_write_data(KEY_CD_COLOR_PBL, &userCDColorPbl, sizeof(userCDColorPbl));
	#endif
	persist_write_data(KEY_CD_COLOR_FLAGS, &userCDFlags, sizeof(userCDFlags));
	persist_write_data(KEY_CUSTOM_DATA, &userCustomData, sizeof(userCustomData));
}

//########################################################################################//
//																																												//
// 																		RESTORE WATCHFACE																		//
//																																												//
//########################################################################################//
/**
 * get text for first row from custom data
 */
static char* get_user_text_from_key(bool isHack) {
	//if (isHack) { APP_LOG(APP_LOG_LEVEL_INFO, "userCustomData.customHackText: %s", userCustomData.customHackText); return (strcmp(userCustomData.customHackText, "") == 0) ? "HACK" : userCustomData.customHackText; }
	//else { APP_LOG(APP_LOG_LEVEL_INFO, "userCustomData.customBeerText: %s", userCustomData.customBeerText); return (strcmp(userCustomData.customBeerText, "") == 0) ? "BEER" : userCustomData.customBeerText; }
	if (isHack) return "HACK";
	else return "BEER";
}
/**
 * set text for first row from custom data
 */
static void set_user_text_from_key() {
	set_text_to_layer(s_hack_layer, get_user_text_from_key(true));
	set_text_to_layer(s_beer_layer, get_user_text_from_key(false));
}
/**
 * set user flags
 */
static void set_user_flags() {
	set_user_text_from_key();
	
	switch (userCDFlags.isAlwaysShownBeerHackDefault) {
		case ALWAYS_HACK:
			if (userCDFlags.isAlwaysShownTimeActive == 1 || userCDFlags.isTimeLayerShown == 1) { switch_text_layers_first_row(true, false, true); handle_time_layer(true); } 
			else { switch_text_layers_first_row(false, false, true); handle_time_layer(false); }
			break;
		case ALWAYS_BEER:
			if (userCDFlags.isAlwaysShownTimeActive == 1 || userCDFlags.isTimeLayerShown == 1) { switch_text_layers_first_row(true, true, false); handle_time_layer(true); }
			else { switch_text_layers_first_row(false, true, false); handle_time_layer(false); }
			break;
		case DEFAULT:
			if (userCDFlags.isAlwaysShownTimeActive == 1 || userCDFlags.isTimeLayerShown == 1) show_time_layer();
			else hide_time_layer();
			break;
	}
}

//########################################################################################//
//																																												//
// 																	LIFE CYCLE METHODS																		//
//																																												//
//########################################################################################//

/**
 * load window
 */
static void window_load(Window *window) {
	get_user_data_from_persist();
		
	//DEBUG_STRUCT_FLAGS();
	
	#ifdef PBL_COLOR
		s_hack_layer = init_text_layer(FIRST_ROW_WOT_RECT, get_user_text_from_key(true), get_user_color(GColorJaegerGreen, userCDColorTC.tcHack), get_user_color(GColorBlack, userCDColorBG.bgHack), FONT_KEY_BITHAM_42_BOLD, GTextAlignmentCenter, false);
		s_beer_layer = init_text_layer(FIRST_ROW_WOT_RECT, get_user_text_from_key(false), get_user_color(GColorRed, userCDColorTC.tcBeer), get_user_color(GColorBlack, userCDColorBG.bgBeer), FONT_KEY_BITHAM_42_BOLD, GTextAlignmentCenter, true);
		s_oclock_layer = init_text_layer(SECOND_ROW_WOT_RECT, "o'clock", get_user_color(GColorWhite, userCDColorTC.tcOclock), get_user_color(GColorBlack, userCDColorBG.bgOclock), FONT_KEY_BITHAM_42_LIGHT, GTextAlignmentCenter, false);
		s_time_layer = init_text_layer(THIRD_ROW_WT_RECT, "00:00", get_user_color(GColorWhite, userCDColorTC.tcTime), get_user_color(GColorBlack, userCDColorBG.bgTime), FONT_KEY_BITHAM_42_LIGHT, GTextAlignmentCenter, true);
		window_set_background_color(s_window, get_user_color(GColorBlack, userCDColorBG.bgWbg));
	#else
		s_hack_layer = init_text_layer(FIRST_ROW_WOT_RECT, get_user_text_from_key(true), get_user_color(GColorWhite, userCDColorPbl.tcPblHack), get_user_color(GColorClear, userCDColorPbl.bgPblHack), FONT_KEY_BITHAM_42_BOLD, GTextAlignmentCenter, false);
		s_beer_layer = init_text_layer(FIRST_ROW_WOT_RECT, get_user_text_from_key(false), get_user_color(GColorWhite, userCDColorPbl.tcPblBeer), get_user_color(GColorClear, userCDColorPbl.bgPblBeer), FONT_KEY_BITHAM_42_BOLD, GTextAlignmentCenter, true);
		s_oclock_layer = init_text_layer(SECOND_ROW_WOT_RECT, "o'clock", get_user_color(GColorWhite, userCDColorPbl.tcPblOclock), get_user_color(GColorClear, userCDColorPbl.bgPblOclock), FONT_KEY_BITHAM_42_LIGHT, GTextAlignmentCenter, false);
		s_time_layer = init_text_layer(THIRD_ROW_WT_RECT, "00:00", get_user_color(GColorWhite, userCDColorPbl.tcPblTime), get_user_color(GColorClear, userCDColorPbl.bgPblTime), FONT_KEY_BITHAM_42_LIGHT, GTextAlignmentCenter, true);
		window_set_background_color(s_window, get_user_color(GColorBlack, userCDColorPbl.bgPblWbg));
	#endif	
					
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_hack_layer));
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_beer_layer));
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_oclock_layer));
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_layer));
	
	set_user_flags();
	
	tick_timer_service_subscribe(MINUTE_UNIT, &handle_minute_tick);
	accel_tap_service_subscribe(tap_handler);
}
/**
 * destroy window elements
 */
static void window_unload(Window *window) {
	text_layer_destroy(s_hack_layer);
	text_layer_destroy(s_beer_layer);
	text_layer_destroy(s_oclock_layer);
	text_layer_destroy(s_time_layer);
}
/**
 * init watchface
 */
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
/**
 * save & destroy window & unsubscribe service
 */
static void deinit() {
	set_user_data_to_persist();
  window_destroy(s_window);
	accel_tap_service_unsubscribe();
}
/**
 * program start
 */
int main(void) {
  init();
  app_event_loop();
  deinit();
}