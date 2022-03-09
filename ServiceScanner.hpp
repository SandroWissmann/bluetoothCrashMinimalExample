#ifndef TEST_SERVICE_SCANNER
#define TEST_SERVICE_SCANNER

#include <QDebug>
#include <QLowEnergyController>
#include <QMetaEnum>
#include <QObject>

#include <vector>

class ServiceScanner : public QObject {
  Q_OBJECT
public:
  explicit ServiceScanner(QObject *parent = nullptr) : QObject(parent) {}

  void connectToDeviceAndStartScan(const QBluetoothDeviceInfo &deviceInfo) {
    qDebug() << Q_FUNC_INFO;

    mLowEnergyControllerPtr =
        QLowEnergyController::createCentral(deviceInfo, this);

    makeConnections();

    mLowEnergyControllerPtr->connectToDevice();
  }

  void disconnectFromDevice() {
    qDebug() << Q_FUNC_INFO;

    if (!mLowEnergyControllerPtr) {
      return;
    }
    qDebug() << Q_FUNC_INFO << "state" << mLowEnergyControllerPtr->state();

    mLowEnergyControllerPtr->disconnectFromDevice();
  }

  std::vector<QBluetoothUuid> servicesUuids() const {
    return mDiscoveredServicesUuids;
  }

signals:
  void serviceScanCompleted();
  void disconnected();
  void errorOccured();

private slots:
  void onError(QLowEnergyController::Error error) {
    QMetaEnum metaEnum = QMetaEnum::fromType<QLowEnergyService::ServiceError>();
    qDebug() << Q_FUNC_INFO << metaEnum.valueToKey(error);

    emit errorOccured();
  }

  void onConnected() {
    qDebug() << Q_FUNC_INFO << "Controller connected. Search services...";
    mLowEnergyControllerPtr->discoverServices();
  }

  void onDisconnected() {
    qDebug() << Q_FUNC_INFO << "LowEnergy controller disconnected";
    qDebug() << Q_FUNC_INFO << "state" << mLowEnergyControllerPtr->state();

    delete mLowEnergyControllerPtr;
    mLowEnergyControllerPtr = nullptr;

    emit disconnected();
  }

  void onServiceDiscovered(const QBluetoothUuid &bluetoothUuid) {
    qDebug() << Q_FUNC_INFO << bluetoothUuid.toString();
    mDiscoveredServicesUuids.emplace_back(bluetoothUuid);
  }

  void onServiceScanDone() {
    qDebug() << Q_FUNC_INFO;
    emit serviceScanCompleted();
  }

private:
  void makeConnections() {
    Q_ASSERT(mLowEnergyControllerPtr);

    connect(mLowEnergyControllerPtr, &QLowEnergyController::serviceDiscovered,
            this, &ServiceScanner::onServiceDiscovered);

    connect(mLowEnergyControllerPtr, &QLowEnergyController::discoveryFinished,
            this, &ServiceScanner::onServiceScanDone);

    connect(mLowEnergyControllerPtr, &QLowEnergyController::connected, this,
            &ServiceScanner::onConnected);

    connect(mLowEnergyControllerPtr, &QLowEnergyController::disconnected, this,
            &ServiceScanner::onDisconnected);
  }

  QLowEnergyController *mLowEnergyControllerPtr{nullptr};

  std::vector<QBluetoothUuid> mDiscoveredServicesUuids;
  // std::vector<QMetaObject::Connection> mConnections;
};

#endif
