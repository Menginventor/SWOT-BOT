#include <Wire.h>
#include <Adafruit_INA219.h>

// ===== BLE =====
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

// =================================================
// BLE UUIDs
#define BLE_SERVICE_UUID  "12345678-1234-1234-1234-1234567890ab"
#define BLE_RX_UUID       "12345678-1234-1234-1234-1234567890ac"
#define BLE_TX_UUID       "12345678-1234-1234-1234-1234567890ad"

// =================================================
// INA219
Adafruit_INA219 ina219;

// ---------------- Motor Pins ----------------
const int AIN1 = 25;
const int AIN2 = 33;
const int BIN1 = 27;
const int BIN2 = 14;
const int STBY = 26;

// ---------------- Motor Direction ----------------
const int MOTOR_A_DIR = -1;
const int MOTOR_B_DIR = -1;

// ---------------- Safety ----------------
const float MIN_SUPPLY_VOLTAGE = 6.0;

// ---------------- Auto Stop ----------------
const bool AUTO_STOP_ENABLED = true;
const unsigned long AUTO_STOP_TIMEOUT_MS = 3000;

// ---------------- Status Push ----------------
const unsigned long STATUS_PERIOD_MS = 500;
unsigned long lastStatusTime = 0;

// ---------------- State ----------------
bool driverEnabled = false;
bool bleConnected  = false;

unsigned long lastCommandTime = 0;
int cmd_left  = 0;
int cmd_right = 0;

// ---------------- BLE ----------------
BLECharacteristic *bleTx;
String rxBLE = "";

// =================================================
// Forward declarations
void parseCommand(String cmd);

void pin_config();
void ina219_init();
void enableDriver();
void disableDriver();

void motor_drive(int pin_1, int pin_2, int speed);
void motor_A_drive(int speed);
void motor_B_drive(int speed);
void stop_all();

void pushStatus();

// =================================================
// BLE callbacks
class ServerCB : public BLEServerCallbacks {
  void onConnect(BLEServer*) override {
    bleConnected = true;
    Serial.println("[BLE] Client connected");
  }

  void onDisconnect(BLEServer*) override {
    bleConnected = false;
    Serial.println("[BLE] Client disconnected");
    BLEDevice::getAdvertising()->start();
    Serial.println("[BLE] Advertising restarted");
  }
};

class RxCB : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *c) override {
    String val = c->getValue();

    for (size_t i = 0; i < val.length(); i++) {
      char ch = val[i];

      if (ch == '\n' || ch == '\r') {
        if (rxBLE.length()) {
          Serial.print("[BLE RX] ");
          Serial.println(rxBLE);
          parseCommand(rxBLE);
          rxBLE = "";
        }
      } else {
        rxBLE += ch;
      }
    }
  }
};

// =================================================
void setup() {
  Serial.begin(115200);
  delay(500);

  Serial.println("Booting ESP32 BLE Robot...");

  pin_config();
  ina219_init();

  // ---- BLE Init ----
  BLEDevice::init("ESP32_ROBOT-ble");

  BLEServer *server = BLEDevice::createServer();
  server->setCallbacks(new ServerCB());

  BLEService *service = server->createService(BLE_SERVICE_UUID);

  BLECharacteristic *rx = service->createCharacteristic(
    BLE_RX_UUID,
    BLECharacteristic::PROPERTY_WRITE
  );

  bleTx = service->createCharacteristic(
    BLE_TX_UUID,
    BLECharacteristic::PROPERTY_NOTIFY
  );

  bleTx->addDescriptor(new BLE2902());
  rx->setCallbacks(new RxCB());

  service->start();

  BLEAdvertising *adv = BLEDevice::getAdvertising();
  adv->addServiceUUID(BLE_SERVICE_UUID);
  adv->setScanResponse(true);
  adv->start();

  Serial.println("[BLE] Advertising started");
  Serial.println("Ready.");

  lastCommandTime = millis();
  lastStatusTime  = millis();
}

// =================================================
void loop() {
  // ---- Auto-stop watchdog ----
  if (AUTO_STOP_ENABLED && millis() - lastCommandTime > AUTO_STOP_TIMEOUT_MS) {
    if (cmd_left != 0 || cmd_right != 0) {
      Serial.println("[AUTO] Timeout → stop");
    }
    cmd_left = 0;
    cmd_right = 0;
  }

  // ---- Power safety ----
  float busV   = ina219.getBusVoltage_V();
  float shuntV = ina219.getShuntVoltage_mV() / 1000.0;
  float supplyV = busV + shuntV;

  if (supplyV < MIN_SUPPLY_VOLTAGE) {
    disableDriver();
  } else {
    enableDriver();
    motor_A_drive(cmd_left);
    motor_B_drive(cmd_right);
  }

  // ---- Periodic BLE status push ----
  if (bleConnected && millis() - lastStatusTime >= STATUS_PERIOD_MS) {
    lastStatusTime = millis();
    pushStatus();
  }

  delay(10);
}

// =================================================
// Command parser (motor only)
void parseCommand(String cmd) {
  cmd.trim();
  lastCommandTime = millis();

  if (!cmd.startsWith("motor")) {
    Serial.println("[CMD] Unknown");
    return;
  }

  int s = cmd.indexOf(' ');
  if (s < 0) return;

  String a = cmd.substring(s + 1);
  int c = a.indexOf(',');
  if (c < 0) return;

  cmd_left  = constrain(a.substring(0, c).toInt(),  -255, 255);
  cmd_right = constrain(a.substring(c + 1).toInt(), -255, 255);

  Serial.print("[MOTOR] L=");
  Serial.print(cmd_left);
  Serial.print(" R=");
  Serial.println(cmd_right);
}

// =================================================
// BLE telemetry push
void pushStatus() {
  float busV   = ina219.getBusVoltage_V();
  float shuntV = ina219.getShuntVoltage_mV() / 1000.0;
  float supplyV = busV + shuntV;
  float current_mA = ina219.getCurrent_mA();

  char buf[64];
  snprintf(buf, sizeof(buf),
           "status V=%.2f I=%.1f\n",
           supplyV, current_mA);

  bleTx->setValue((uint8_t*)buf, strlen(buf));
  bleTx->notify();

  Serial.print("[BLE TX] ");
  Serial.print(buf);
}

// =================================================
// Hardware
void pin_config() {
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);
  pinMode(STBY, OUTPUT);
  digitalWrite(STBY, LOW);
}

void motor_A_drive(int speed) {
  motor_drive(AIN1, AIN2, speed * MOTOR_A_DIR);
}

void motor_B_drive(int speed) {
  motor_drive(BIN2, BIN1, speed * MOTOR_B_DIR);
}

void motor_drive(int p1, int p2, int s) {
  s = constrain(s, -255, 255);

  if (s > 0) {
    analogWrite(p1, s);
    analogWrite(p2, 0);
  } else if (s < 0) {
    analogWrite(p1, 0);
    analogWrite(p2, -s);
  } else {
    // BRAKE
    analogWrite(p1, 255);
    analogWrite(p2, 255);
  }
}

void stop_all() {
  motor_A_drive(0);
  motor_B_drive(0);
}

// =================================================
// Driver control
void enableDriver() {
  if (!driverEnabled) {
    digitalWrite(STBY, HIGH);
    driverEnabled = true;
    Serial.println("[DRV] Enabled");
  }
}

void disableDriver() {
  if (driverEnabled) {
    stop_all();
    digitalWrite(STBY, LOW);
    driverEnabled = false;
    Serial.println("[DRV] Disabled");
  }
}

// =================================================
// INA219 init
void ina219_init() {
  Wire.begin(21, 22);

  if (!ina219.begin()) {
    Serial.println("INA219 not found");
    while (1);
  }

  ina219.setCalibration_32V_2A();
}
