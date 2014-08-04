#include <pebble.h>
#define COUNT_DEFAULT 1
#define COUNT_KEY 1

static Window *window;
static TextLayer *text_layer;
static int count = COUNT_DEFAULT;

void display() {
  static char body_text[50];
  snprintf(body_text, sizeof(body_text), "High Fives: %u", count);
  text_layer_set_text(text_layer, body_text); 
  vibes_cancel();
  vibes_short_pulse();
}

void accel_tap_handler(AccelAxisType axis, int32_t direction) {
  // Process tap on ACCEL_AXIS_X, ACCEL_AXIS_Y or ACCEL_AXIS_Z
  // Direction is 1 or -1

  count = persist_exists(COUNT_KEY) ? persist_read_int(COUNT_KEY) : COUNT_DEFAULT;
  count++;

  display();
  persist_write_int(COUNT_KEY, count);

  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);
  Tuplet value = TupletInteger(1, count);
  dict_write_tuplet(iter, &value);
  app_message_outbox_send();
}

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  persist_write_int(COUNT_KEY, 0);
  text_layer_set_text(text_layer, "Awaiting Hi Five");
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  display();
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  display();
}

void out_sent_handler(DictionaryIterator *sent, void *context) {
   // outgoing message was delivered
 }


 void out_failed_handler(DictionaryIterator *failed, AppMessageResult reason, void *context) {
   // outgoing message failed
 }

 void in_received_handler(DictionaryIterator *received, void *context) {
   // incoming message received
 }


 void in_dropped_handler(AppMessageResult reason, void *context) {
   // incoming message dropped
 }

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  text_layer = text_layer_create((GRect) { .origin = { 0, 35 }, .size = { bounds.size.w, 60 } });
  text_layer_set_font(text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
  text_layer_set_text(text_layer, "Awaiting High Five");
  text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(text_layer));
}

static void window_unload(Window *window) {
  text_layer_destroy(text_layer);
}

static void init(void) {

  app_message_register_inbox_received(in_received_handler);
  app_message_register_inbox_dropped(in_dropped_handler);
  app_message_register_outbox_sent(out_sent_handler);
  app_message_register_outbox_failed(out_failed_handler);

  const uint32_t inbound_size = 64;
  const uint32_t outbound_size = 64;
  app_message_open(inbound_size, outbound_size);


  window = window_create();
  window_set_click_config_provider(window, click_config_provider);
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  accel_tap_service_subscribe(accel_tap_handler);
  const bool animated = true;
  window_stack_push(window, animated);
}

static void deinit(void) {
  accel_tap_service_unsubscribe();
  window_destroy(window);
}

int main(void) {
  init();

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window: %p", window);

  app_event_loop();
  deinit();
}
