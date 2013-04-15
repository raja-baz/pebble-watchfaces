#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"

#define MY_UUID {0x04, 0x07, 0x3C, 0x9D, 0x92, 0x40, 0x4E, 0x2A, 0xBB, 0xFC, 0xB0, 0x2F, 0x2F, 0x7E, 0xB0, 0xF5}
PBL_APP_INFO(MY_UUID, "Simplicity 2", "Pebble Technology + Raja Baz", 1, 0, RESOURCE_ID_IMAGE_MENU_ICON, APP_INFO_WATCH_FACE);

Window window;

TextLayer text_date_layer;
TextLayer text_time_layer;
TextLayer text_am_pm_layer;

Layer line_layer;

#define TEXT_START_Y 48
#define AM_PM_HIGH_OFFSET 52
#define AM_PM_LOW_OFFSET 75

void line_layer_update_callback(Layer *me, GContext* ctx) {
    (void)me;

    graphics_context_set_stroke_color(ctx, GColorWhite);

    graphics_draw_line(ctx, GPoint(8, TEXT_START_Y+29), GPoint(131, TEXT_START_Y+29));
    graphics_draw_line(ctx, GPoint(8, TEXT_START_Y+30), GPoint(131, TEXT_START_Y+30));

}

void display_time(PblTm * tick_time) {
    // Need to be static because they're used by the system later.
    static char time_text[] = "00:00";
    static char date_text[] = "Xxxxxxxxx 00";
    static char am_pm_text[] = "XX";

    static int am_pm_text_is_high = 1; // Insert "high" joke here
    
    char *time_format;


    // TODO: Only update the date when it's changed.
    string_format_time(date_text, sizeof(date_text), "%B %e", tick_time);
    text_layer_set_text(&text_date_layer, date_text);


    if (clock_is_24h_style()) {
        time_format = "%R";
        memset(am_pm_text, 0, sizeof(am_pm_text));
    } else {
        time_format = "%I:%M";
        if (tick_time->tm_hour >= 12){
            am_pm_text[0] = 'P';
        } else {
            am_pm_text[0] = 'A';
        }
        am_pm_text[1] = 'M';
    }

    text_layer_set_text(&text_am_pm_layer, am_pm_text);

    string_format_time(time_text, sizeof(time_text), time_format, tick_time);

    // Kludge to handle lack of non-padded hour format string
    // for twelve hour clock.
    if (!clock_is_24h_style() && (time_text[0] == '0')) {
        memmove(time_text, &time_text[1], sizeof(time_text) - 1);
        if (!am_pm_text_is_high){
            layer_set_frame(&text_am_pm_layer.layer, GRect(110, TEXT_START_Y+AM_PM_HIGH_OFFSET, 144-110, 168-(TEXT_START_Y+AM_PM_HIGH_OFFSET)));
            am_pm_text_is_high = 1;
        }
    } else {
        if (am_pm_text_is_high){
            layer_set_frame(&text_am_pm_layer.layer, GRect(110, TEXT_START_Y+AM_PM_LOW_OFFSET, 144-110, 168-(TEXT_START_Y+AM_PM_LOW_OFFSET)));
            am_pm_text_is_high = 0;
        }
    }

    text_layer_set_text(&text_time_layer, time_text);    
}

void handle_init(AppContextRef ctx) {
    (void)ctx;

    window_init(&window, "Simplicity");
    window_stack_push(&window, true /* Animated */);
    window_set_background_color(&window, GColorBlack);

    resource_init_current_app(&APP_RESOURCES);

    GFont small_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ROBOTO_CONDENSED_21));

    text_layer_init(&text_date_layer, window.layer.frame);
    text_layer_set_text_color(&text_date_layer, GColorWhite);
    text_layer_set_background_color(&text_date_layer, GColorClear);
    layer_set_frame(&text_date_layer.layer, GRect(8, TEXT_START_Y, 144-8, 168-TEXT_START_Y));
    text_layer_set_font(&text_date_layer, small_font);
    layer_add_child(&window.layer, &text_date_layer.layer);


    text_layer_init(&text_time_layer, window.layer.frame);
    text_layer_set_text_color(&text_time_layer, GColorWhite);
    text_layer_set_background_color(&text_time_layer, GColorClear);
    layer_set_frame(&text_time_layer.layer, GRect(7, TEXT_START_Y+24, 144-7, 168-(TEXT_START_Y+24)));
    text_layer_set_font(&text_time_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ROBOTO_BOLD_SUBSET_49)));
    layer_add_child(&window.layer, &text_time_layer.layer);

    text_layer_init(&text_am_pm_layer, window.layer.frame);
    text_layer_set_text_color(&text_am_pm_layer, GColorWhite);
    text_layer_set_background_color(&text_am_pm_layer, GColorClear);
    layer_set_frame(&text_am_pm_layer.layer, GRect(110, TEXT_START_Y+AM_PM_HIGH_OFFSET, 144-110, 168-(TEXT_START_Y+AM_PM_HIGH_OFFSET)));
    text_layer_set_font(&text_am_pm_layer, small_font);
    layer_add_child(&window.layer, &text_am_pm_layer.layer);


    layer_init(&line_layer, window.layer.frame);
    line_layer.update_proc = &line_layer_update_callback;
    layer_add_child(&window.layer, &line_layer);

    // Avoids a blank screen on watch start.
    PblTm tick_time;

    get_time(&tick_time);
    display_time(&tick_time);
}

void handle_minute_tick(AppContextRef ctx, PebbleTickEvent *t) {
    (void)ctx;
    display_time(t->tick_time);
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
