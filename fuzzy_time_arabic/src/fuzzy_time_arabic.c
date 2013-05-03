#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"

#define MY_UUID { 0x91, 0xC1, 0xDA, 0xB7, 0x5B, 0xA7, 0x45, 0xE7, 0xA7, 0x68, 0x78, 0xE8, 0x5C, 0xA6, 0x48, 0x16 }
PBL_APP_INFO(MY_UUID,
             "Arabic Fuzzy Time",
             "Raja Baz",
             1, 0, /* App version */
             DEFAULT_MENU_ICON,
             APP_INFO_WATCH_FACE);

static const char* const HOURS[] = {
    "tna3esh",
    "we7de",
    "tenten",
    "tlete",
    "arb3a",
    "khamse",
    "sette",
    "sab3a",
    "tmene",
    "tes3a",
    "3ashra",
    "7da3esh"
};

static const char * const OFFSETS[] = {
    "",
    "khamse",
    "3ashra",
    "rebe3",
    "telet",
    "noss ella khamse",
    "noss",
    "noss w khamse",
    "telet",
    "rebe3",
    "3ashra",
    "khamse"
};

static const char* const STR_MINUS = "ella";
static const char* const STR_AND = "w";

static struct CommonWordsData {
    TextLayer hours;
    TextLayer offsets;
    TextLayer middles;
    Window window;
} s_data;

static void update_time(PblTm* t) {
    int fuzzy_hours = t->tm_hour;
    int fuzzy_minutes = ((t->tm_min + 2) / 5) * 5;

    int positive_offset = 1;
    if (fuzzy_minutes > 35){
	positive_offset = 0;
	fuzzy_hours += 1;
    }

    if (fuzzy_minutes > 55){
	fuzzy_minutes = 0;
    }

    fuzzy_hours %= 12;
    fuzzy_minutes /= 5;

    const char * hour = HOURS[fuzzy_hours];
    const char * middle = "";
    const char * offset = "";
    if (fuzzy_minutes > 0){
	if (positive_offset){
	    middle = STR_AND;
	} else {
	    middle = STR_MINUS;
	}
	offset = OFFSETS[fuzzy_minutes];
    }

    text_layer_set_text(&s_data.hours, hour);
    text_layer_set_text(&s_data.offsets, offset);
    text_layer_set_text(&s_data.middles, middle);
}

static void handle_minute_tick(AppContextRef app_ctx, PebbleTickEvent* e) {
    update_time(e->tick_time);
}

static void common_text_layer_init(TextLayer * layer, GFont font){
    text_layer_set_background_color(layer, GColorClear);
    text_layer_set_text_color(layer, GColorWhite);
    text_layer_set_font(layer, font);
    text_layer_set_text_alignment(layer, GTextAlignmentCenter);
    layer_add_child(&s_data.window.layer, &layer->layer);
}

static void handle_init(AppContextRef ctx) {
    (void) ctx;

    resource_init_current_app(&FUZZY_TIME_ARABIC_RESOURCES);

    window_init(&s_data.window, "My Fuzzy Time");
    const bool animated = true;
    window_stack_push(&s_data.window, animated);

    window_set_background_color(&s_data.window, GColorBlack);
    GFont hours_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_GOTHAM_BOLD_36));
    GFont regular_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_GOTHAM_LIGHT_28));

    int screen_width = s_data.window.layer.frame.size.w;
    text_layer_init(&s_data.hours, GRect(0, 5, screen_width, 45));
    common_text_layer_init(&s_data.hours, hours_font);

    text_layer_init(&s_data.middles, GRect(0, 50, screen_width, 30));
    common_text_layer_init(&s_data.middles, regular_font);

    text_layer_init(&s_data.offsets, GRect(0, 90, screen_width, 60));
    common_text_layer_init(&s_data.offsets, regular_font);

    PblTm t;
    get_time(&t);
    update_time(&t);
}


void pbl_main(void *params) {
    PebbleAppHandlers handlers = {
        .init_handler = &handle_init,

        .tick_info = {
            .tick_handler = &handle_minute_tick,
            .tick_units = MINUTE_UNIT
        }

    };
    app_event_loop(params, &handlers);
}
