#include <pebble.h>

Window * window;
TextLayer * text_date_layer;
TextLayer * text_time_layer;
TextLayer * text_am_pm_layer;
Layer * line_layer;

#define TEXT_START_Y 48
#define AM_PM_HIGH_OFFSET 52
#define AM_PM_LOW_OFFSET 75

void line_layer_update_callback(Layer *me, GContext* ctx) {
    graphics_context_set_stroke_color(ctx, GColorWhite);

    graphics_draw_line(ctx, GPoint(8, TEXT_START_Y+29), GPoint(131, TEXT_START_Y+29));
    graphics_draw_line(ctx, GPoint(8, TEXT_START_Y+30), GPoint(131, TEXT_START_Y+30));

}

void display_time(struct tm * tick_time) {
    // Need to be static because they're used by the system later.
    static char time_text[] = "00:00";
    static char date_text[] = "Xxxxxxxxx 00";
    static char am_pm_text[] = "XX";

    static int am_pm_text_is_high = 1; // Insert "high" joke here
    
    char *time_format;


    // TODO: Only update the date when it's changed.
    strftime(date_text, sizeof(date_text), "%B %e", tick_time);
    text_layer_set_text(text_date_layer, date_text);

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

    text_layer_set_text(text_am_pm_layer, am_pm_text);

    strftime(time_text, sizeof(time_text), time_format, tick_time);

    // Kludge to handle lack of non-padded hour format string
    // for twelve hour clock.
    if (!clock_is_24h_style() && (time_text[0] == '0')) {
        memmove(time_text, &time_text[1], sizeof(time_text) - 1);
        if (!am_pm_text_is_high){
            layer_set_frame(text_layer_get_layer(text_am_pm_layer), GRect(110, TEXT_START_Y+AM_PM_HIGH_OFFSET, 144-110, 168-(TEXT_START_Y+AM_PM_HIGH_OFFSET)));
            am_pm_text_is_high = 1;
        }
    } else {
        if (am_pm_text_is_high){
            layer_set_frame(text_layer_get_layer(text_am_pm_layer), GRect(110, TEXT_START_Y+AM_PM_LOW_OFFSET, 144-110, 168-(TEXT_START_Y+AM_PM_LOW_OFFSET)));
            am_pm_text_is_high = 0;
        }
    }

    text_layer_set_text(text_time_layer, time_text);    
}

static void handle_minute_tick(struct tm *tick_time, TimeUnits units_changed) {
    display_time(tick_time);
}

void handle_deinit(void){
    tick_timer_service_unsubscribe();
    window_destroy(window);
    text_layer_destroy(text_date_layer);
    text_layer_destroy(text_time_layer);
    text_layer_destroy(text_am_pm_layer);
    layer_destroy(line_layer);
}

void handle_init(void) {
    window = window_create();
    window_stack_push(window, true /* Animated */);
    window_set_background_color(window, GColorBlack);

    GFont small_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ROBOTO_CONDENSED_21));

    Layer * root_layer = window_get_root_layer(window);
    GRect frame = layer_get_frame(root_layer);
    text_date_layer = text_layer_create(frame);
    text_layer_set_text_color(text_date_layer, GColorWhite);
    text_layer_set_background_color(text_date_layer, GColorClear);
    layer_set_frame(text_layer_get_layer(text_date_layer), GRect(8, TEXT_START_Y, 144-8, 168-TEXT_START_Y));
    text_layer_set_font(text_date_layer, small_font);
    layer_add_child(root_layer, text_layer_get_layer(text_date_layer));

    text_time_layer = text_layer_create(frame);
    text_layer_set_text_color(text_time_layer, GColorWhite);
    text_layer_set_background_color(text_time_layer, GColorClear);
    layer_set_frame(text_layer_get_layer(text_time_layer), GRect(7, TEXT_START_Y+24, 144-7, 168-(TEXT_START_Y+24)));
    text_layer_set_font(text_time_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ROBOTO_BOLD_SUBSET_49)));
    layer_add_child(root_layer, text_layer_get_layer(text_time_layer));

    text_am_pm_layer = text_layer_create(frame);
    text_layer_set_text_color(text_am_pm_layer, GColorWhite);
    text_layer_set_background_color(text_am_pm_layer, GColorClear);
    layer_set_frame(text_layer_get_layer(text_am_pm_layer), GRect(110, TEXT_START_Y+AM_PM_HIGH_OFFSET, 144-110, 168-(TEXT_START_Y+AM_PM_HIGH_OFFSET)));
    text_layer_set_font(text_am_pm_layer, small_font);
    layer_add_child(root_layer, text_layer_get_layer(text_am_pm_layer));

    line_layer = layer_create(frame);
    layer_set_update_proc(line_layer, line_layer_update_callback);
    layer_add_child(root_layer, line_layer);

    // Avoids a blank screen on watch start.
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    display_time(t);
    tick_timer_service_subscribe(MINUTE_UNIT, handle_minute_tick);
}

int main(void) {
    handle_init();
    app_event_loop();  
    handle_deinit();
}
