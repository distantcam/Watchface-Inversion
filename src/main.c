#include <pebble.h>

Window *window;

GBitmap *image1;
GBitmap *image2;
GBitmap *image3;
GBitmap *image4;
GBitmap *image5;
GBitmap *image6;
GBitmap *image7;
GBitmap *image8;
GBitmap *image9;
GBitmap *image10;
GBitmap *image11;
GBitmap *image12;
BitmapLayer *hourLayer;

InverterLayer *minuteLayer;

void handle_hour_tick(struct tm *tick_time, TimeUnits units_changed) {
	int hour = tick_time->tm_hour;
	if (hour > 12)
		hour -= 12;
	if (hour == 0)
		hour = 12;
	
	if (hour == 1)
		bitmap_layer_set_bitmap(hourLayer, image1);
	else if (hour == 2)
		bitmap_layer_set_bitmap(hourLayer, image2);
	else if (hour == 3)
		bitmap_layer_set_bitmap(hourLayer, image3);
	else if (hour == 4)
		bitmap_layer_set_bitmap(hourLayer, image4);
	else if (hour == 5)
		bitmap_layer_set_bitmap(hourLayer, image5);
	else if (hour == 6)
		bitmap_layer_set_bitmap(hourLayer, image6);
	else if (hour == 7)
		bitmap_layer_set_bitmap(hourLayer, image7);
	else if (hour == 8)
		bitmap_layer_set_bitmap(hourLayer, image8);
	else if (hour == 9)
		bitmap_layer_set_bitmap(hourLayer, image9);
	else if (hour == 10)
		bitmap_layer_set_bitmap(hourLayer, image10);
	else if (hour == 11)
		bitmap_layer_set_bitmap(hourLayer, image11);
	else if (hour == 12)
		bitmap_layer_set_bitmap(hourLayer, image12);
}

void handle_minute_tick(struct tm *tick_time, TimeUnits units_changed) {
	int min = tick_time->tm_min;
	int top = 0;
	
	if (min == 0) top = 168;
	if (min == 1) top = 165;
	if (min == 2) top = 162;
	if (min == 3) top = 160;
	if (min == 4) top = 157;
	if (min == 5) top = 154;
	if (min == 6) top = 151;
	if (min == 7) top = 148;
	if (min == 8) top = 146;
	if (min == 9) top = 143;
	if (min == 10) top = 140;
	if (min == 11) top = 137;
	if (min == 12) top = 134;
	if (min == 13) top = 132;
	if (min == 14) top = 129;
	if (min == 15) top = 126;
	if (min == 16) top = 123;
	if (min == 17) top = 120;
	if (min == 18) top = 118;
	if (min == 19) top = 115;
	if (min == 20) top = 112;
	if (min == 21) top = 109;
	if (min == 22) top = 106;
	if (min == 23) top = 104;
	if (min == 24) top = 101;
	if (min == 25) top = 98;
	if (min == 26) top = 95;
	if (min == 27) top = 92;
	if (min == 28) top = 90;
	if (min == 29) top = 87;
	if (min == 30) top = 84;
	if (min == 31) top = 81;
	if (min == 32) top = 78;
	if (min == 33) top = 76;
	if (min == 34) top = 73;
	if (min == 35) top = 70;
	if (min == 36) top = 67;
	if (min == 37) top = 64;
	if (min == 38) top = 62;
	if (min == 39) top = 59;
	if (min == 40) top = 56;
	if (min == 41) top = 53;
	if (min == 42) top = 50;
	if (min == 43) top = 48;
	if (min == 44) top = 45;
	if (min == 45) top = 42;
	if (min == 46) top = 39;
	if (min == 47) top = 36;
	if (min == 48) top = 34;
	if (min == 49) top = 31;
	if (min == 50) top = 28;
	if (min == 51) top = 25;
	if (min == 52) top = 22;
	if (min == 53) top = 20;
	if (min == 54) top = 17;
	if (min == 55) top = 14;
	if (min == 56) top = 11;
	if (min == 57) top = 8;
	if (min == 58) top = 6;
	if (min == 59) top = 3;

	layer_set_frame(inverter_layer_get_layer(minuteLayer), GRect(0, top, 144, 168));
}

void init(void) {
	window = window_create();
	window_set_background_color(window, GColorBlack);
  	window_stack_push(window, true); // Animated
	
	Layer *windowLayer = window_get_root_layer(window);
	GRect windowFrame = layer_get_frame(windowLayer);

	image1 = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_1);
	image2 = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_2);
	image3 = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_3);
	image4 = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_4);
	image5 = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_5);
	image6 = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_6);
	image7 = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_7);
	image8 = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_8);
	image9 = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_9);
	image10 = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_10);
	image11 = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_11);
	image12 = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_12);
	
	hourLayer = bitmap_layer_create(windowFrame);
	layer_add_child(windowLayer, bitmap_layer_get_layer(hourLayer));
	
	minuteLayer = inverter_layer_create(windowFrame);
	layer_add_child(bitmap_layer_get_layer(hourLayer), inverter_layer_get_layer(minuteLayer));
	
	tick_timer_service_subscribe(HOUR_UNIT, handle_hour_tick);
	tick_timer_service_subscribe(MINUTE_UNIT, handle_minute_tick);
	
	time_t now = time(NULL);
  	struct tm *tick_time = localtime(&now);
		
	handle_hour_tick(tick_time, HOUR_UNIT);
	handle_minute_tick(tick_time, MINUTE_UNIT);
}

void deinit(void) {
	layer_remove_from_parent(inverter_layer_get_layer(minuteLayer));
	inverter_layer_destroy(minuteLayer);
	
	layer_remove_from_parent(bitmap_layer_get_layer(hourLayer));
	bitmap_layer_destroy(hourLayer);
	
	gbitmap_destroy(image1);
	gbitmap_destroy(image2);
	gbitmap_destroy(image3);
	gbitmap_destroy(image4);
	gbitmap_destroy(image5);
	gbitmap_destroy(image6);
	gbitmap_destroy(image7);
	gbitmap_destroy(image8);
	gbitmap_destroy(image9);
	gbitmap_destroy(image10);
	gbitmap_destroy(image11);
	gbitmap_destroy(image12);
	
	window_destroy(window);
}

int main(void) {
	init();
	app_event_loop();
	deinit();
}
