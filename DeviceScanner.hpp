#ifndef TEST_DEVICE_SCANNER
#define TEST_DEVICE_SCANNER

#include <QBluetoothDeviceDiscoveryAgent>
#include <QBluetoothLocalDevice>
#include <QDebug>
#include <QMetaEnum>

#include <QObject>

class DeviceScanner : public QObject {
  Q_OBJECT
public:
  explicit DeviceScanner(QObject *parent = nullptr) : QObject{parent} {
    mDeviceDiscoveryAgentPtr = new QBluetoothDeviceDiscoveryAgent(this);
    mDeviceDiscoveryAgentPtr->setLowEnergyDiscoveryTimeout(5000);

    connect(mDeviceDiscoveryAgentPtr,
            &QBluetoothDeviceDiscoveryAgent::deviceDiscovered, this,
            &DeviceScanner::onAddDeviceInfo);

    connect(mDeviceDiscoveryAgentPtr,
            &QBluetoothDeviceDiscoveryAgent::errorOccurred, this,
            &DeviceScanner::onScanError);

    connect(mDeviceDiscoveryAgentPtr, &QBluetoothDeviceDiscoveryAgent::finished,
            this, &DeviceScanner::onScanCompleted);
    connect(mDeviceDiscoveryAgentPtr, &QBluetoothDeviceDiscoveryAgent::canceled,
            this, &DeviceScanner::onScanCompleted);
  }

signals:
  // emit when new device is discovered
  void deviceDiscovered(const QBluetoothDeviceInfo &deviceInfo);

  // emit when
  void scanCompleted();

public:
  void startScanning() {
    qDebug() << Q_FUNC_INFO;

    mDeviceDiscoveryAgentPtr->start(
        QBluetoothDeviceDiscoveryAgent::LowEnergyMethod);

    qDebug() << Q_FUNC_INFO << "Scanning for devices...";
  }

private slots:
  void onAddDeviceInfo(const QBluetoothDeviceInfo &deviceInfo) {
    qDebug() << Q_FUNC_INFO;

    if (!(deviceInfo.coreConfigurations() &
          QBluetoothDeviceInfo::LowEnergyCoreConfiguration)) {
      return;
    }

    qDebug() << Q_FUNC_INFO
             << "Found device with address:" << deviceInfo.address();

    emit deviceDiscovered(deviceInfo);
  }

  void onScanError(QBluetoothDeviceDiscoveryAgent::Error error) {
    QMetaEnum metaEnum =
        QMetaEnum::fromType<QBluetoothDeviceDiscoveryAgent::Error>();
    qDebug() << Q_FUNC_INFO << metaEnum.valueToKey(error);
  }

  void onScanCompleted() {
    qDebug() << Q_FUNC_INFO;
    emit scanCompleted();
  }

private:
  QBluetoothDeviceDiscoveryAgent *mDeviceDiscoveryAgentPtr;
};

#endif
