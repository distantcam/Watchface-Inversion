#include <pebble.h>

static Window *window;

#define TOTAL_HOUR_IMAGES 12

const uint32_t HOUR_IMAGE_RESOURCE_IDS[] = {
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
    RESOURCE_ID_IMAGE_12
};

const int MINUTE_HEIGHTS[] = { 
    167, 165, 162, 160, 157, 154, 151, 148, 146, 143, 
    140, 137, 134, 132, 129, 126, 123, 120, 118, 115, 
    112, 109, 106, 104, 101, 98, 95, 92, 90, 87, 
    84, 81, 78, 76, 73, 70, 67, 64, 62, 59, 
    56, 53, 50, 48, 45, 42, 39, 36, 34, 31, 
    28, 25, 22, 20, 17, 14, 11, 8, 6, 3
};

static GBitmap *hourImage;
static BitmapLayer *hourLayer;

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

static void on_animation_stopped(Animation *anim, bool finished, void *context) {
    //Free the memoery used by the Animation
    property_animation_destroy((PropertyAnimation*) anim);
}
 
static void animate_layer(Layer *layer, GRect *start, GRect *finish, int duration, int delay) {
    //Declare animation
    PropertyAnimation *anim = property_animation_create_layer_frame(layer, start, finish);
    
    //Set characteristics
    animation_set_duration((Animation*) anim, duration);
    animation_set_delay((Animation*) anim, delay);
     
    //Set stopped handler to free memory
    AnimationHandlers handlers = {
        //The reference to the stopped handler is the only one in the array
        .stopped = (AnimationStoppedHandler) on_animation_stopped
    };
    animation_set_handlers((Animation*) anim, handlers, NULL);
    
    //Start animation!
    animation_schedule((Animation*) anim);
}

static void handle_tick(struct tm *tick_time, TimeUnits units_changed) {
    int hour = tick_time->tm_hour;
    int min = tick_time->tm_min;
    int sec = tick_time->tm_sec;

    GRect start, finish;

    if ((units_changed & HOUR_UNIT) != 0) {
        while (hour > 12)
            hour -= 12;
        if (hour == 0)
            hour = 12;
        
        hour -= 1;

        set_container_image(&hourImage, hourLayer, HOUR_IMAGE_RESOURCE_IDS[hour]);
    }
    if ((units_changed & MINUTE_UNIT) != 0) {
        int previous = min - 1;
        if (previous < 0) 
            previous += 60;
        
        start = GRect(0, MINUTE_HEIGHTS[previous], 144, 168);
        finish = GRect(0, MINUTE_HEIGHTS[min], 144, 168);
        animate_layer(inverter_layer_get_layer(minuteLayer), &start, &finish, 300, 0);
    }

    if (min == 59 && sec == 59) {
        start = GRect(0, 0, 144, 168);
        finish = GRect(0, 168, 144, 168);
        animate_layer(bitmap_layer_get_layer(hourLayer), &start, &finish, 300, 700);
    }
    if (min == 0 && sec == 0) {
        start = GRect(0, -168, 144, 168);
        finish = GRect(0, 0, 144, 168);
        animate_layer(bitmap_layer_get_layer(hourLayer), &start, &finish, 300, 0);
    }
}

static void init(void) {
    window = window_create();
    window_set_background_color(window, GColorBlack);
    window_stack_push(window, true); // Animated
    
    Layer *windowLayer = window_get_root_layer(window);
    GRect windowFrame = layer_get_frame(windowLayer);
    
    hourLayer = bitmap_layer_create(windowFrame);
    layer_add_child(windowLayer, bitmap_layer_get_layer(hourLayer));
    
    minuteLayer = inverter_layer_create(windowFrame);
    layer_add_child(windowLayer, inverter_layer_get_layer(minuteLayer));
    
    time_t now = time(NULL);
    struct tm *tick_time = localtime(&now);
    
    handle_tick(tick_time, HOUR_UNIT);
    handle_tick(tick_time, MINUTE_UNIT);
    
    tick_timer_service_subscribe(SECOND_UNIT, handle_tick);
}

static void deinit(void) {
    tick_timer_service_unsubscribe();
    
    layer_remove_child_layers(window_get_root_layer(window));
    
    inverter_layer_destroy(minuteLayer);
    
    gbitmap_destroy(hourImage);
    bitmap_layer_destroy(hourLayer);
    
    window_destroy(window);
}

int main(void) {
    init();
    app_event_loop();
    deinit();
}