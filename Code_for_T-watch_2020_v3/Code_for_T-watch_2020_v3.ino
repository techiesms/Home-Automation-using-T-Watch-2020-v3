
#include "config.h"


#include <WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"


/************************* WiFi Access Point *********************************/

#define WLAN_SSID       "SSID"
#define WLAN_PASS       "PASS"

/************************* Adafruit.io Setup *********************************/

#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883                   // use 8883 for SSL
#define AIO_USERNAME    "USERNAME"
#define AIO_KEY         "AIOKEY"


// Create an ESP32 WiFiClient class to connect to the MQTT server.
WiFiClient client;

// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);




char* Temp_Data;
char* Hum_Data;

static void slider_event_cb(lv_obj_t * slider, lv_event_t event);
static lv_obj_t * slider_label;
static lv_obj_t * temp_label;
static lv_obj_t * hum_label;

int temp_data;
int hum_data;
int Temperature_Value;
int Humidity_Value;

bool Relay1_Status = 0;
bool Relay2_Status = 0;
bool Relay3_Status = 0;
bool Relay4_Status = 0;

Adafruit_MQTT_Subscribe temperature = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/temperature");
Adafruit_MQTT_Subscribe humidity = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/humidity");

// Setup a feed called 'onoff' for subscribing to changes.
Adafruit_MQTT_Publish relay1 = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/relay1");
Adafruit_MQTT_Publish relay2 = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/relay2");
Adafruit_MQTT_Publish relay3 = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/relay3");
Adafruit_MQTT_Publish relay4 = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/relay4");
Adafruit_MQTT_Publish fan = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/fan");



TTGOClass *ttgo;
lv_obj_t *gContainer = NULL;

void lv_ex_bar_1(void)
{
  lv_obj_t * bar1 = lv_bar_create(lv_scr_act(), NULL);
  lv_obj_set_size(bar1, 100, 10);
  lv_obj_align(bar1, NULL, LV_ALIGN_CENTER, -50, 80);
  String RAW = Temp_Data;
  int temp_value = RAW.toInt();
  Serial.println(temp_value);
  lv_bar_set_value(bar1, temp_value, LV_ANIM_OFF);

  /* Create an informative label */
  lv_obj_t * temp_info = lv_label_create(lv_scr_act(), NULL);
  lv_label_set_text(temp_info, "Temperature");
  lv_obj_align(temp_info, NULL, LV_ALIGN_IN_TOP_LEFT, 13, 180);

  /* Create a label below the slider */
  temp_label = lv_label_create(lv_scr_act(), NULL);
  lv_label_set_text(temp_label, "       ");
  lv_obj_set_auto_realign(temp_label, true);
  lv_obj_align(temp_label, bar1, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);

  /* Create a label below the slider */
  temp_label = lv_label_create(lv_scr_act(), NULL);
  lv_label_set_text(temp_label, Temp_Data);
  lv_obj_set_auto_realign(temp_label, true);
  lv_obj_align(temp_label, bar1, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);

}

void lv_ex_bar_2(void)
{
  lv_obj_t * bar2 = lv_bar_create(lv_scr_act(), NULL);
  lv_obj_set_size(bar2, 100, 10);
  lv_obj_align(bar2, NULL, LV_ALIGN_CENTER, 50, 80);

  String RAW = Hum_Data;
  int hum_value = RAW.toInt();
  Serial.println(hum_value);
  lv_bar_set_value(bar2, hum_value, LV_ANIM_OFF);

  /* Create an informative label */
  lv_obj_t * hum_info = lv_label_create(lv_scr_act(), NULL);
  lv_label_set_text(hum_info, "Humidity");
  lv_obj_align(hum_info, NULL, LV_ALIGN_IN_TOP_LEFT, 140, 180);



  /* Create a label below the slider */
  hum_label = lv_label_create(lv_scr_act(), NULL);
  lv_label_set_text(hum_label, Hum_Data);
  lv_obj_set_auto_realign(hum_label, true);
  lv_obj_align(hum_label, bar2, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);


}



static void event_handler1(lv_obj_t *obj, lv_event_t event)
{
  if (event == LV_EVENT_VALUE_CHANGED)
  {
    Serial.printf("Relay1 Toggled");
    Relay1_Status = !Relay1_Status;
    relay1.publish(Relay1_Status);

  }
}

static void event_handler2(lv_obj_t *obj, lv_event_t event)
{
  if (event == LV_EVENT_VALUE_CHANGED)
  {
    Serial.printf("Relay2 Toggled");
    Relay2_Status = !Relay2_Status;
    relay2.publish(Relay2_Status);
  }
}

static void event_handler3(lv_obj_t *obj, lv_event_t event)
{
  if (event == LV_EVENT_VALUE_CHANGED)
  {
    Serial.printf("Relay3 Toggled");
    Relay3_Status = !Relay3_Status;
    relay3.publish(Relay3_Status);
  }
}

static void event_handler4(lv_obj_t *obj, lv_event_t event)
{
  if (event == LV_EVENT_VALUE_CHANGED)
  {
    Serial.printf("Relay4 Toggled");
    Relay4_Status = !Relay4_Status;
    relay4.publish(Relay4_Status);
  }
}

static void slider_event_cb(lv_obj_t * slider, lv_event_t event)
{
  if (event == LV_EVENT_VALUE_CHANGED) {
    static char buf[4]; /* max 3 bytes for number plus 1 null terminating byte */
    snprintf(buf, 4, "%u", lv_slider_get_value(slider));
    Serial.print("Fan Speed:- "); Serial.println(buf);
    fan.publish(buf);
    lv_label_set_text(slider_label, buf);
  }
}
void lv_ex_slider_2(void)
{
  /* Create a slider in the center of the display */
  lv_obj_t * slider = lv_slider_create(lv_scr_act(), NULL);
  lv_obj_set_width(slider, LV_DPI * 1.5);
  lv_obj_align(slider, NULL, LV_ALIGN_CENTER, 0, 20);
  lv_obj_set_event_cb(slider, slider_event_cb);
  lv_slider_set_range(slider, 0, 4);

  /* Create a label below the slider */
  slider_label = lv_label_create(lv_scr_act(), NULL);
  lv_label_set_text(slider_label, "0");
  lv_obj_set_auto_realign(slider_label, true);
  lv_obj_align(slider_label, slider, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);

  /* Create an informative label */
  lv_obj_t * info = lv_label_create(lv_scr_act(), NULL);
  lv_label_set_text(info, "Fan Speed");
  lv_obj_align(info, NULL, LV_ALIGN_IN_TOP_LEFT, 80, 110);
}

void setup()
{
  Serial.begin(115200);


  ttgo = TTGOClass::getWatch();
  ttgo->begin();



  ttgo->openBL();


  Serial.println("Adafruit MQTT demo"); // It will be printed on Serial Monitor


  /*************************** WiFi Connection Code ************************************/
  // Connect to WiFi access point.
  Serial.println(); Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);

  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.println("WiFi connected");
  Serial.println("IP address: "); Serial.println(WiFi.localIP());





  //  Blynk.begin(auth, ssid, pass);
  ttgo->lvgl_begin();

  lv_obj_t *label;

  /* Create a Title  */
  lv_obj_t * title = lv_label_create(lv_scr_act(), NULL);
  lv_label_set_text(title, "MQTT Home Automation");
  lv_obj_align(title, NULL, LV_ALIGN_IN_TOP_LEFT, 30, 0);


  lv_obj_t *btn1 = lv_btn_create(lv_scr_act(), NULL);
  lv_obj_set_event_cb(btn1, event_handler1);
  lv_obj_align(btn1, NULL, LV_ALIGN_CENTER, -80, -80);
  lv_btn_set_checkable(btn1, true);
  lv_btn_toggle(btn1);
  lv_btn_set_fit2(btn1,  LV_FIT_NONE, LV_FIT_TIGHT);

  label = lv_label_create(btn1, NULL);
  lv_label_set_text(label, "Relay1");



  lv_obj_t *btn2 = lv_btn_create(lv_scr_act(), NULL);
  lv_obj_set_event_cb(btn2, event_handler2);
  lv_obj_align(btn2, NULL, LV_ALIGN_CENTER, 80 , -80);
  lv_btn_set_checkable(btn2, true);
  lv_btn_toggle(btn2);
  lv_btn_set_fit2(btn2, LV_FIT_NONE, LV_FIT_TIGHT);

  label = lv_label_create(btn2, NULL);
  lv_label_set_text(label, "Relay2");


  lv_obj_t *btn3 = lv_btn_create(lv_scr_act(), NULL);
  lv_obj_set_event_cb(btn3, event_handler3);
  lv_obj_align(btn3, NULL, LV_ALIGN_CENTER, -80, -30);
  lv_btn_set_checkable(btn3, true);
  lv_btn_toggle(btn3);
  lv_btn_set_fit2(btn3, LV_FIT_NONE, LV_FIT_TIGHT);

  label = lv_label_create(btn3, NULL);
  lv_label_set_text(label, "Relay3");



  lv_obj_t *btn4 = lv_btn_create(lv_scr_act(), NULL);
  lv_obj_set_event_cb(btn4, event_handler4);
  lv_obj_align(btn4, NULL, LV_ALIGN_CENTER, 80, -30);
  lv_btn_set_checkable(btn4, true);
  lv_btn_toggle(btn4);
  lv_btn_set_fit2(btn4, LV_FIT_NONE, LV_FIT_TIGHT);

  label = lv_label_create(btn4, NULL);
  lv_label_set_text(label, "Relay4");

  lv_ex_slider_2();
  lv_ex_bar_1();
  lv_ex_bar_2();

  // Setup MQTT subscription for onoff feed.
  mqtt.subscribe(&temperature);
  mqtt.subscribe(&humidity);
}

void loop()
{
  MQTT_connect();
  lv_task_handler();
  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(1)))
  {
    if (subscription == &temperature)
    {
      // Storing the data
      Temp_Data = (char*) temperature.lastread;
      Serial.print("Got: ");
      Serial.println(Temp_Data);

    }

    if (subscription == &humidity)
    {
      // Storing the data
      Hum_Data = (char*) humidity.lastread;
      Serial.print("Got: ");
      Serial.println(Hum_Data);

    }
    lv_label_set_text(hum_label, "      ");
    lv_label_set_text(temp_label, "      ");
    lv_ex_bar_1();
    lv_ex_bar_2();
  }
}
void MQTT_connect()
{
  int ret;

  // Stop if already connected.
  if (mqtt.connected())
  {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  int retries = 3;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
    Serial.println(mqtt.connectErrorString(ret));
    Serial.println("Retrying MQTT connection in 5 seconds...");
    mqtt.disconnect();
    delay(5000);  // wait 5 seconds
    retries--;
    if (retries == 0) {
      // basically die and wait for WDT to reset me
      while (1);
    }
  }
  Serial.println("MQTT Connected!");
}
