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
            static_cast<void (QBluetoothDeviceDiscoveryAgent::*)(
                QBluetoothDeviceDiscoveryAgent::Error)>(
                &QBluetoothDeviceDiscoveryAgent::error),
            this, &DeviceScanner::onScanError);

    connect(mDeviceDiscoveryAgentPtr, &QBluetoothDeviceDiscoveryAgent::finished,
            this, &DeviceScanner::onScanCompleted);
    connect(mDeviceDiscoveryAgentPtr, &QBluetoothDeviceDiscoveryAgent::canceled,
            this, &DeviceScanner::onScanCompleted);
  }

signals:
  void deviceDiscovered(const QBluetoothDeviceInfo &deviceInfo);

  void scanCompleted();

public:
  void startScanning() {
    qDebug() << Q_FUNC_INFO;

    mDevicesInfosPtr.clear();
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

    auto deviceInfoPtr = std::make_shared<QBluetoothDeviceInfo>(deviceInfo);
    mDevicesInfosPtr.emplace_back(deviceInfoPtr);
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
  std::vector<std::shared_ptr<QBluetoothDeviceInfo>> mDevicesInfosPtr;
};

#endif
