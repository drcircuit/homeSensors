// Display
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)

// LEDs
#define RED D7
#define GREEN D8

// Sensor
#define DHTPIN D5
#define DHTTYPE DHT11

// Lock
#define KEY D6
#define LOCK D3

// Wifi
const char *ssid = "SunnyTownRoad_A2G";
const char *pass = "SandeLarsens";
const int maxWifi = 50; // max retries
