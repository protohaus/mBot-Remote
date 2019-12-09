/**
 * A BLE client example that is rich in capabilities.
 * There is a lot new capabilities implemented.
 * author unknown
 * updated by chegewara
 */

#include "Arduino.h"
#include "BLEDevice.h"
//#include "BLEScan.h"

// The remote service we wish to connect to.
static BLEUUID serviceUUID("ffe1");
// The characteristics of the remote service we are interested in.
static BLEUUID notifyCharUUID("ffe2"); // Notify = a read when data exists 
static BLEUUID writeCharUUID("ffe3"); // Write a command any time

static boolean doConnect = false;
static boolean connected = false;
static boolean doScan = false;
static BLERemoteCharacteristic *pNotifyCharacteristic;
static BLERemoteCharacteristic *pWriteCharacteristic;
static BLEAdvertisedDevice *myDevice;

static void notifyCallback(BLERemoteCharacteristic *pBLERemoteCharacteristic,
                           uint8_t *pData, size_t length, bool isNotify) {
  Serial.print("Notify callback for characteristic ");
  Serial.print(pBLERemoteCharacteristic->getUUID().toString().c_str());
  Serial.print(" of data length ");
  Serial.println(length);
  Serial.print("data: ");
  for (int i = 0; i < length; i++) {
    Serial.printf("%.2X ", pData[i]);
  }
  Serial.println();
}

class MyClientCallback : public BLEClientCallbacks {
  void onConnect(BLEClient *pclient) {}

  void onDisconnect(BLEClient *pclient) {
    connected = false;
    Serial.println("onDisconnect");
  }
};

bool connectToServer() {
  Serial.print("Forming a connection to ");
  Serial.println(myDevice->getAddress().toString().c_str());

  BLEClient *pClient = BLEDevice::createClient();
  Serial.println(" - Created client");

  pClient->setClientCallbacks(new MyClientCallback());

  // Connect to the remove BLE Server.
  pClient->connect(myDevice);  // if you pass BLEAdvertisedDevice instead of
                               // address, it will be recognized type of peer
                               // device address (public or private)
  Serial.println(" - Connected to server");
  std::map<std::string, BLERemoteService *> *services = pClient->getServices();

  for (auto service : (*services)) {
    Serial.print("Found service: ");
    Serial.println(service.second->getUUID().toString().c_str());
    auto characteristics = service.second->getCharacteristics();
    for (auto characteristic : (*characteristics)) {
      Serial.print("  Characteristic: ");
      Serial.print(characteristic.second->getUUID().toString().c_str());
      Serial.printf(" - R: %d, W: %d, N: %d\n",
                    characteristic.second->canRead(),
                    characteristic.second->canWrite(),
                    characteristic.second->canNotify());
    }
  }
  Serial.println();

  // Obtain a reference to the service we are after in the remote BLE server.
  BLERemoteService *pRemoteService = pClient->getService(serviceUUID);
  if (pRemoteService == nullptr) {
    Serial.print("Failed to find our service UUID: ");
    Serial.println(serviceUUID.toString().c_str());
    pClient->disconnect();
    return false;
  }
  Serial.println(" - Found our service");

  // Obtain a reference to the characteristic in the service of the remote BLE
  // server.
  static BLEUUID readCharUUID("ffe2");
  pWriteCharacteristic = pRemoteService->getCharacteristic(writeCharUUID);
  if (pWriteCharacteristic == nullptr) {
    Serial.print("Failed to find our characteristic UUID: ");
    static BLEUUID readCharUUID("ffe2");
    Serial.println(writeCharUUID.toString().c_str());
    pClient->disconnect();
    return false;
  }
  Serial.println(" - Found our characteristic");

  // Read the value of the characteristic.
  if (pWriteCharacteristic->canRead()) {
    std::string value = pWriteCharacteristic->readValue();
    Serial.print("The characteristic value was: ");
    Serial.println(value.c_str());
  }

  pNotifyCharacteristic = pRemoteService->getCharacteristic(notifyCharUUID);
  if (pNotifyCharacteristic->canNotify())
    pNotifyCharacteristic->registerForNotify(notifyCallback);

  connected = true;
  return true;
}
/**
 * Scan for BLE servers and find the first one that advertises the service we
 * are looking for.
 */
class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {
  /**
   * Called for each advertising BLE server.
   */
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    Serial.print("Device found: ");
    Serial.println(advertisedDevice.toString().c_str());

    if (advertisedDevice.getName().compare(0, 9, "Makeblock") == 0) {
      Serial.print("mBot found: ");
      Serial.println(advertisedDevice.toString().c_str());
      BLEDevice::getScan()->stop();
      myDevice = new BLEAdvertisedDevice(advertisedDevice);
      doConnect = true;
      doScan = true;
    }
    // We have found a device, let us now see if it contains the service we are
    // looking for.
    if (advertisedDevice.haveServiceUUID() &&
        advertisedDevice.isAdvertisingService(serviceUUID)) {
      BLEDevice::getScan()->stop();
      myDevice = new BLEAdvertisedDevice(advertisedDevice);
      doConnect = true;
      doScan = true;

    }  // Found our server
  }    // onResult
};     // MyAdvertisedDeviceCallbacks

void setup() {
  Serial.begin(115200);
  Serial.println("Starting Arduino BLE Client application...");
  BLEDevice::init("");

  // Retrieve a Scanner and set the callback we want to use to be informed when
  // we have detected a new device.  Specify that we want active scanning and
  // start the scan to run for 5 seconds.
  BLEScan *pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setInterval(1349);
  pBLEScan->setWindow(449);
  pBLEScan->setActiveScan(true);
  pBLEScan->start(5, false);
}  // End of setup.

// This is the Arduino main loop function.
void loop() {
  // If the flag "doConnect" is true then we have scanned for and found the
  // desired BLE Server with which we wish to connect.  Now we connect to it.
  // Once we are connected we set the connected flag to be true.
  if (doConnect == true) {
    if (connectToServer()) {
      Serial.println("We are now connected to the BLE Server.");
    } else {
      Serial.println(
          "We have failed to connect to the server; there is nothin "
          "more we will do.");
    }
    doConnect = false;
  }

  // If we are connected to a peer BLE Server, update the characteristic each
  // time we are reached with the current time since boot.
  if (connected) {
    // String newValue = "Time since boot: " + String(millis()/1000);
    // Serial.println("Setting new characteristic value to \"" + newValue +
    // "\"");

    // Motor control
    int speed = -120;
    uint8_t speed_low = speed & 0xFF;
    uint8_t speed_high = (speed >> 8) & 0xFF;
    // Serial.printf("Speed: %d, Speed Low: 0x%.2X, Speed High: 0x%.2X", speed,
    //               speed_low, speed_high);
    uint8_t leftMotorGo[] = {0xff, 0x55, 0x06,      0x60,      0x02,
                             0x0a, 0x09, speed_low, speed_high};
    uint8_t rightMotorGo[] = {0xff, 0x55, 0x06,      0x60,      0x02,
                              0x0a, 0x0a, speed_low, speed_high};
    // pWriteCharacteristic->writeValue(leftMotorGo, sizeof(leftMotorGo));
    // pWriteCharacteristic->writeValue(rightMotorGo, sizeof(rightMotorGo));

    // Ultrasonic --> Uses notify callback
    //  ff    55    04    02    01    01    03
    uint8_t getUltrasonic[] = {0xff, 0x55, 0x04, 0x02, 0x01, 0x01, 0x03};
    pWriteCharacteristic->writeValue(getUltrasonic, sizeof(getUltrasonic));

  } else if (doScan) {
    BLEDevice::getScan()->start(0);
             // this is just example to start scan after disconnect, most likely
             // there is better way to do it in arduino
  }

  delay(1000);  // Delay a second between loops.
}  // End of loop
