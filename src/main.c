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
	168, 165, 162, 160, 157, 154, 151, 148, 146, 143, 
	140, 137, 134, 132, 129, 126, 123, 120, 118, 115, 
	112, 109, 106, 104, 101, 98, 95, 92, 90, 87, 
	84, 81, 78, 76, 73, 70, 67, 64, 62, 59, 
	56, 53, 50, 48, 45, 42, 39, 36, 34, 31, 
	28, 25, 22, 20, 17, 14, 11, 8, 6, 3
};

GBitmap *hourImage;
static BitmapLayer *hourLayer;

InverterLayer *minuteLayer;

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

void handle_hour_tick(struct tm *tick_time, TimeUnits units_changed) {
	int hour = tick_time->tm_hour;
	while (hour > 12)
		hour -= 12;
	if (hour == 0)
		hour = 12;
	
	hour -= 1;
		
	set_container_image(&hourImage, hourLayer, HOUR_IMAGE_RESOURCE_IDS[hour]);
}

void handle_minute_tick(struct tm *tick_time, TimeUnits units_changed) {
	int min = tick_time->tm_min;
	int top = MINUTE_HEIGHTS[min];
	
	layer_set_frame(inverter_layer_get_layer(minuteLayer), GRect(0, top, 144, 168));
}

void init(void) {
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
		
	handle_hour_tick(tick_time, HOUR_UNIT);
	handle_minute_tick(tick_time, MINUTE_UNIT);
	
	tick_timer_service_subscribe(HOUR_UNIT, handle_hour_tick);
	tick_timer_service_subscribe(MINUTE_UNIT, handle_minute_tick);
}

void deinit(void) {
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
