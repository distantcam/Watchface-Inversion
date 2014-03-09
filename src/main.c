#include <pebble.h>

static Window *window;

#define TOTAL_HOUR_IMAGES 24

const uint32_t HOUR_IMAGE_RESOURCE_IDS[] = {
    RESOURCE_ID_IMAGE_0,
    RESOURCE_ID_IMAGE_1,
    RESOURCE_ID_IMAGE_2,
    RESOURCE_ID_IMAGE_3,
    RESOURCE_ID_IMAGE_4,
    RESOURCE_ID_IMAGE_5,
    RESOURCE_ID_IMAGE_6,
    RESOURCE_ID_IMAGE_7,
    RESOURCE_ID_IMAGE_8,
    RESOURCE_ID_IMAGE_9,
    RESOURCE_ID_IMAGE_10,
    RESOURCE_ID_IMAGE_11,
    RESOURCE_ID_IMAGE_12,
    RESOURCE_ID_IMAGE_13,
    RESOURCE_ID_IMAGE_14,
    RESOURCE_ID_IMAGE_15,
    RESOURCE_ID_IMAGE_16,
    RESOURCE_ID_IMAGE_17,
    RESOURCE_ID_IMAGE_18,
    RESOURCE_ID_IMAGE_19,
    RESOURCE_ID_IMAGE_20,
    RESOURCE_ID_IMAGE_21,
    RESOURCE_ID_IMAGE_22,
    RESOURCE_ID_IMAGE_23
};

const int MINUTE_HEIGHTS[] = { 
    167, 165, 162, 160, 157, 154, 151, 148, 146, 143, 
    140, 137, 134, 132, 129, 126, 123, 120, 118, 115, 
    112, 109, 106, 104, 101, 98, 95, 92, 90, 87, 
    84, 81, 78, 76, 73, 70, 67, 64, 62, 59, 
    56, 53, 50, 48, 45, 42, 39, 36, 34, 31, 
    28, 25, 22, 20, 17, 14, 11, 8, 6, 3
};

static GBitmap *hourImage[2];
static BitmapLayer *hourLayer[2];

static int currentIndex = 0;

static InverterLayer *minuteLayer;

static void set_container_image(GBitmap **bmp_image, BitmapLayer *bmp_layer, const int resource_id) {
    GBitmap *old_image = *bmp_image;

    *bmp_image = gbitmap_create_with_resource(resource_id);
    GRect frame = (GRect) {
        .size = (*bmp_image)->bounds.size
    };
    bitmap_layer_set_bitmap(bmp_layer, *bmp_image);
    layer_set_frame(bitmap_layer_get_layer(bmp_layer), frame);

    if (old_image != NULL) {
        gbitmap_destroy(old_image);
    }
}

static void handle_tap_timer(void *data) {
    layer_set_hidden(inverter_layer_get_layer(minuteLayer), false);
}

static void handle_accel_tapped(AccelAxisType axis, int32_t direction) {
    layer_set_hidden(inverter_layer_get_layer(minuteLayer), true);

    app_timer_register(3500, handle_tap_timer, NULL);
}

static void handle_animation_stopped(Animation *anim, bool finished, void *context) {
    //Free the memoery used by the Animation
    property_animation_destroy((PropertyAnimation*) anim);
}
 
static void animate_layer(Layer *layer, GRect *start, GRect *finish, int duration, int delay) {
    PropertyAnimation *anim = property_animation_create_layer_frame(layer, start, finish);
    animation_set_duration((Animation*) anim, duration);
    animation_set_delay((Animation*) anim, delay);
     
    AnimationHandlers handlers = {
        .stopped = (AnimationStoppedHandler) handle_animation_stopped
    };
    animation_set_handlers((Animation*) anim, handlers, NULL);
    
    animation_schedule((Animation*) anim);
}

static unsigned short get_display_hour(unsigned short hour) {
  if (clock_is_24h_style()) {
    return hour;
  }
  unsigned short display_hour = hour % 12;
  // Converts "0" to "12"
  return display_hour ? display_hour : 12;
}

static void handle_tick(struct tm *tick_time, TimeUnits units_changed) {
    unsigned short hour = get_display_hour(tick_time->tm_hour);
    unsigned short min = tick_time->tm_min;
    //unsigned short sec = tick_time->tm_sec;

    GRect start, finish;

    if ((units_changed & HOUR_UNIT) != 0) {
        int oldIndex = currentIndex;

        currentIndex ^= 1;

        set_container_image(&hourImage[currentIndex], hourLayer[currentIndex], HOUR_IMAGE_RESOURCE_IDS[hour]);

        start = GRect(0, 0, 144, 168);
        finish = GRect(0, 168, 144, 168);
        animate_layer(bitmap_layer_get_layer(hourLayer[oldIndex]), &start, &finish, 300, 0);

        start = GRect(0, -168, 144, 168);
        finish = GRect(0, 0, 144, 168);
        animate_layer(bitmap_layer_get_layer(hourLayer[currentIndex]), &start, &finish, 300, 0);
    }

    if ((units_changed & MINUTE_UNIT) != 0) {
        int previous = min - 1;
        if (previous < 0) 
            previous += 60;
        
        start = GRect(0, MINUTE_HEIGHTS[previous], 144, 168);
        finish = GRect(0, MINUTE_HEIGHTS[min], 144, 168);
        animate_layer(inverter_layer_get_layer(minuteLayer), &start, &finish, 300, 0);
    }
}

static void init(void) {
    window = window_create();
    window_set_background_color(window, GColorBlack);
    window_stack_push(window, true); // Animated
    
    Layer *windowLayer = window_get_root_layer(window);
    GRect windowFrame = layer_get_frame(windowLayer);
    
    for (int i = 0; i < 2; ++i) {
        hourLayer[i] = bitmap_layer_create(windowFrame);
        layer_add_child(windowLayer, bitmap_layer_get_layer(hourLayer[i]));
    }
    
    minuteLayer = inverter_layer_create(windowFrame);
    layer_add_child(windowLayer, inverter_layer_get_layer(minuteLayer));
    
    time_t now = time(NULL);
    struct tm *tick_time = localtime(&now);
    
    handle_tick(tick_time, HOUR_UNIT);
    handle_tick(tick_time, MINUTE_UNIT);
    
    tick_timer_service_subscribe(MINUTE_UNIT, handle_tick);
    //accel_tap_service_subscribe(handle_accel_tapped);
}

static void deinit(void) {
    tick_timer_service_unsubscribe();
    accel_tap_service_unsubscribe();
    
    layer_remove_child_layers(window_get_root_layer(window));
    
    inverter_layer_destroy(minuteLayer);
    
    for (int i = 0; i < 2; ++i) {
        if (hourImage[i] != NULL) {
            gbitmap_destroy(hourImage[i]);
        }

        bitmap_layer_destroy(hourLayer[i]);
    }

    window_destroy(window);
}

int main(void) {
    init();
    app_event_loop();
    deinit();
}