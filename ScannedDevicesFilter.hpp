#ifndef TEST_SCANNED_DEVICES_FILTER
#define TEST_SCANNED_DEVICES_FILTER

#include "ServiceScanner.hpp"

#include <QObject>

class ScannedDevicesFilter : public QObject {
  Q_OBJECT
public:
  explicit ScannedDevicesFilter(QObject *parent = nullptr)
      : QObject{parent}, mServiceScannerPtr{new ServiceScanner(this)} {
    connect(mServiceScannerPtr, &ServiceScanner::serviceScanCompleted, this,
            &ScannedDevicesFilter::onServiceScanComplete);

    connect(mServiceScannerPtr, &ServiceScanner::errorOccured, this,
            &ScannedDevicesFilter::onServiceScanError);

    connect(mServiceScannerPtr, &ServiceScanner::disconnected, this,
            &ScannedDevicesFilter::onServiceScannerDisconnected);
  }

signals:
  void eq3DeviceFound(const QBluetoothDeviceInfo &deviceInfo);

  void filteringCompleted();

public slots:
  void onStartFiltering() {
    qDebug() << "#########" << Q_FUNC_INFO;

    analyzeNextElement();
  }

  void onAddDevice(const QBluetoothDeviceInfo &deviceInfo) {
    qDebug() << "#########" << Q_FUNC_INFO;

    mScannedDevices.emplace_back(deviceInfo);
  }

private slots:
  void onServiceScanComplete() {
    qDebug() << "#########" << Q_FUNC_INFO;

    auto serviceUuids = mServiceScannerPtr->servicesUuids();

    qDebug() << "Found device with serviceUuids: " << serviceUuids;

    mServiceScannerPtr->disconnectFromDevice();
  }

  void onServiceScanError() {
    qDebug() << "#########" << Q_FUNC_INFO;

    mServiceScannerPtr->disconnectFromDevice();
  }

  void onServiceScannerDisconnected() {
    qDebug() << "#########" << Q_FUNC_INFO;

    analyzeNextElement();
  }

private:
  void analyzeNextElement() {
    qDebug() << "#########" << Q_FUNC_INFO;

    if (mScannedDevices.empty()) {
      qDebug() << "#########" << Q_FUNC_INFO << "filteringCompleted";
      emit filteringCompleted();
    } else {
      mCurrentScannedDevice = mScannedDevices.back();
      mScannedDevices.pop_back();

      qDebug() << "#########" << Q_FUNC_INFO
               << "Check last device in vector. Mac address: "
               << mCurrentScannedDevice.address();

      mServiceScannerPtr->connectToDeviceAndStartScan(mCurrentScannedDevice);
    }
  }

  bool isEq3Thermostat(const std::vector<QBluetoothUuid> &uuids) {
    if (uuids.size() != 5) {
      return false;
    }

    std::vector<QBluetoothUuid> expectedUuids{
        QBluetoothUuid{QString{"{00001800-0000-1000-8000-00805f9b34fb}"}},
        QBluetoothUuid{QString{"{00001801-0000-1000-8000-00805f9b34fb}"}},
        QBluetoothUuid{QString{"{0000180a-0000-1000-8000-00805f9b34fb}"}},
        QBluetoothUuid{QString{"{3e135142-654f-9090-134a-a6ff5bb77046}"}},
        QBluetoothUuid{QString{"{9e5d1e47-5c13-43a0-8635-82ad38a1386f}"}}};

    if (uuids != expectedUuids) {
      return false;
    }
    return true;
  }

  std::vector<QBluetoothDeviceInfo> mScannedDevices;
  ServiceScanner *mServiceScannerPtr;
  QBluetoothDeviceInfo mCurrentScannedDevice;
};

#endif
