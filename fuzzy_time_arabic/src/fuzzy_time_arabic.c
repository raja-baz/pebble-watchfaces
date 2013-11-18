#include <pebble.h>

static const char* const HOURS[] = {
    "tna3sh",
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
    "7da3sh"
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
    TextLayer * hours;
    TextLayer * offsets;
    TextLayer * middles;
    Window * window;
} s_data;

static void update_time(struct tm* t) {
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

    text_layer_set_text(s_data.hours, hour);
    text_layer_set_text(s_data.offsets, offset);
    text_layer_set_text(s_data.middles, middle);
}

static void handle_minute_tick(struct tm *tick_time, TimeUnits units_changed) {
  update_time(tick_time);
}

static void common_text_layer_init(TextLayer * layer, GFont font){
    text_layer_set_background_color(layer, GColorClear);
    text_layer_set_text_color(layer, GColorWhite);
    text_layer_set_font(layer, font);
    text_layer_set_text_alignment(layer, GTextAlignmentCenter);
    layer_add_child(window_get_root_layer(s_data.window), text_layer_get_layer(layer));
}

static void do_init(void) {
    s_data.window = window_create();
    const bool animated = true;
    window_stack_push(s_data.window, animated);

    window_set_background_color(s_data.window, GColorBlack);
    GFont hours_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_GOTHAM_BOLD_36));
    GFont regular_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_GOTHAM_BOOK_28));

    Layer *root_layer = window_get_root_layer(s_data.window);
    GRect frame = layer_get_frame(root_layer);

    int screen_width = frame.size.w;
    s_data.hours = text_layer_create(GRect(0, 5, screen_width, 45));
    common_text_layer_init(s_data.hours, hours_font);

    s_data.middles = text_layer_create(GRect(0, 50, screen_width, 30));
    common_text_layer_init(s_data.middles, regular_font);

    s_data.offsets = text_layer_create(GRect(0, 90, screen_width, 60));
    common_text_layer_init(s_data.offsets, regular_font);

    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    update_time(t);

    tick_timer_service_subscribe(MINUTE_UNIT, &handle_minute_tick);
}

static void do_deinit(void) {
  window_destroy(s_data.window);
  text_layer_destroy(s_data.hours);
  text_layer_destroy(s_data.middles);
  text_layer_destroy(s_data.offsets);
}

int main(void) {
  do_init();
  app_event_loop();
  do_deinit();
}
