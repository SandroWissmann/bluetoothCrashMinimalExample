#include <QBluetoothLocalDevice>
#include <QCoreApplication>

#include "DeviceScanner.hpp"
#include "ScannedDevicesFilter.hpp"

int main(int argc, char *argv[]) {
  QCoreApplication a(argc, argv);

  QBluetoothLocalDevice bluetoothLocalDevice;
  bluetoothLocalDevice.powerOn();

  ScannedDevicesFilter scannedDevicesFilter;

  DeviceScanner deviceScanner;

  QObject::connect(&deviceScanner, &DeviceScanner::deviceDiscovered,
                   &scannedDevicesFilter, &ScannedDevicesFilter::onAddDevice);

  QObject::connect(&deviceScanner, &DeviceScanner::scanCompleted,
                   &scannedDevicesFilter,
                   &ScannedDevicesFilter::onStartFiltering);

  deviceScanner.startScanning();

  return a.exec();
}
