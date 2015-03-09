#include <pebble.h>
#include <timer.h>
  
/*
    Resolution: 144(w) x 168(h)
    
    Bitmap area: 134(w) x 180(h)
*/  
  
static Window *window;

//Layers
static TextLayer *setcounter_layer;
static TextLayer *time_layer;
static TextLayer *countdown_layer;
static TextLayer *animation_layer;
static AppTimer *countdown_timer;

//Animation
static BitmapLayer *image_layer;
static AppTimer *animation_timer;
static GBitmap *jumpingjacks1_bitmap;
static GBitmap *jumpingjacks2_bitmap;
static int animation_frame;
static bool animation_isrunning;

//Countdown timer
static int countdown_min;
static int countdown_sec;
char* str_countdown_time;

//Set tracker
static int setCount;
static int setTotal;
char* str_setCounter;


void countdown_callback(void *data)
{
  //updating countdown
  if (countdown_sec == 0)
  {
    countdown_min -= 1;
    countdown_sec = 59;
  }  
  else
  {
    countdown_sec -= 1;  
  }
  
  //If the timer is done, cancel it
  if (countdown_min == 0 && countdown_sec == 0)
  {  
    app_timer_cancel(countdown_timer);
    snprintf(str_countdown_time, 8, "%d:%02d", countdown_min, countdown_sec);
    text_layer_set_text(countdown_layer, str_countdown_time);
    return;
  }
  
  snprintf(str_countdown_time, 8, "%d:%02d", countdown_min, countdown_sec);
  text_layer_set_text(countdown_layer, str_countdown_time);
  countdown_timer = app_timer_register(1000, (AppTimerCallback)countdown_callback, NULL);
}

void animation_callback(void* data)
{
  //Update animation
  if (animation_frame==1)
  {
    bitmap_layer_set_bitmap(image_layer, jumpingjacks2_bitmap);
    animation_frame = 2;
  }
  else
  {
    bitmap_layer_set_bitmap(image_layer, jumpingjacks1_bitmap);
    animation_frame = 1;
  }
  animation_timer = app_timer_register(500, (AppTimerCallback)animation_callback, NULL);
}

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  if (animation_isrunning)
  {
    app_timer_cancel(animation_timer);
    animation_isrunning = false;
  }
  else
  {
    animation_timer = app_timer_register(500, (AppTimerCallback)animation_callback, NULL);
    animation_isrunning = true;
  }
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  //Increment set counter and update window
  setCount += 1;
  snprintf(str_setCounter, sizeof(str_setCounter), "%d/%d", setCount, setTotal);
  text_layer_set_text(setcounter_layer, str_setCounter);
  
  //Cancel old counter
  app_timer_cancel(countdown_timer);
  
  //Start countdown
  countdown_min = 1;
  countdown_sec = 0;
  snprintf(str_countdown_time, 8, "%d:%02d", countdown_min, countdown_sec);
  countdown_timer = app_timer_register(1000, (AppTimerCallback)countdown_callback, NULL);
  
  if (setCount == setTotal)
  {
      //update exercise
      
      //get next setTotal
      
      //reset setCount
      setCount = 0;
  }
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  text_layer_set_text(setcounter_layer, "Down");
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
}

/* tick time handler */
void tick_handler(struct tm *tick_time, TimeUnits units_changed)
{
    static char buffer[]  = "00:00";
    if(clock_is_24h_style()) {
      // Use 24 hour format
      strftime(buffer, sizeof("00:00"), "%H:%M", tick_time);
    } else {
      // Use 12 hour format
      strftime(buffer, sizeof("00:00"), "%I:%M", tick_time);
    }

    text_layer_set_text(time_layer, buffer);
}

static void window_load(Window *window) {
  
  //Get window details
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  
  //initiate set counter section
  setCount = 0;
  setTotal = 5;
  str_setCounter = (char*)malloc(8*sizeof(char));
  snprintf(str_setCounter, sizeof(str_setCounter), "%d/%d", setCount, setTotal);
  
  // Create time section
  // Details: top left, white text, black background
  // Dimensions: 72(w) x 40(h)
  time_layer = text_layer_create((GRect) { .origin = {0, 0}, .size = {bounds.size.w/2-2, 30}});
  text_layer_set_font(time_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28));
  text_layer_set_background_color(time_layer, GColorBlack);
  text_layer_set_text_color(time_layer, GColorWhite);
  text_layer_set_text_alignment(time_layer, GTextAlignmentCenter);
  
  // Creating Set counter section
  // Details: top right, white text, black background
  // Dimensions: 72(w) x 40(h)
  setcounter_layer = text_layer_create((GRect) { .origin = { bounds.size.w/2+2, 0 }, .size = { bounds.size.w/2-2, 30 } });
  text_layer_set_text(setcounter_layer, str_setCounter);
  text_layer_set_font(setcounter_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28));
  text_layer_set_background_color(setcounter_layer, GColorBlack);
  text_layer_set_text_color(setcounter_layer, GColorWhite);
  text_layer_set_text_alignment(setcounter_layer, GTextAlignmentRight);
  
  // Creating Bitmap Layer -----filler for now------
  // Details: center, white background, shows exercise animation
  animation_frame = 1;
  jumpingjacks1_bitmap = gbitmap_create_with_resource(RESOURCE_ID_JUMPINGJACKS_1);
  jumpingjacks2_bitmap = gbitmap_create_with_resource(RESOURCE_ID_JUMPINGJACKS_2);
  image_layer = bitmap_layer_create((GRect) {.origin = {0, 30}, .size = {134, 108} });
  bitmap_layer_set_bitmap(image_layer, jumpingjacks1_bitmap);
  animation_isrunning = true;
  animation_timer = app_timer_register(500, (AppTimerCallback)animation_callback, NULL);
  
  // Create Timer section
  // Details: bottom bar, white text, black background
  // Dimensions: 144(w) x 30(h)
  countdown_min = 1;
  countdown_sec = 0;
  str_countdown_time = (char*)malloc(8*sizeof(char));
  snprintf(str_countdown_time, 8, "%d:%02d", countdown_min, countdown_sec);
  countdown_layer = text_layer_create((GRect) { .origin = {0, bounds.size.h-30}, .size = {bounds.size.w, 30} });
  text_layer_set_text(countdown_layer, str_countdown_time);
  text_layer_set_font(countdown_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28));
  text_layer_set_background_color(countdown_layer, GColorBlack);
  text_layer_set_text_color(countdown_layer, GColorWhite);
  text_layer_set_text_alignment(countdown_layer, GTextAlignmentCenter);
  
  // Add layers left->right
  layer_add_child(window_layer, text_layer_get_layer(setcounter_layer));
  layer_add_child(window_layer, text_layer_get_layer(time_layer));
  layer_add_child(window_layer, bitmap_layer_get_layer(image_layer));
  layer_add_child(window_layer, text_layer_get_layer(countdown_layer));
  
}

static void window_unload(Window *window) {
  text_layer_destroy(setcounter_layer);
  text_layer_destroy(time_layer);
  text_layer_destroy(countdown_layer);
  text_layer_destroy(animation_layer);
  gbitmap_destroy(jumpingjacks1_bitmap);
  gbitmap_destroy(jumpingjacks2_bitmap);
  bitmap_layer_destroy(image_layer);
}

static void init(void) {
  window = window_create();
  window_set_click_config_provider(window, click_config_provider);
  window_set_window_handlers(window, (WindowHandlers) {
	.load = window_load,
    .unload = window_unload,
  });
  
  tick_timer_service_subscribe(MINUTE_UNIT, (TickHandler)tick_handler);
  
  const bool animated = true;
  window_stack_push(window, animated);
}

static void deinit(void) {
  window_destroy(window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
