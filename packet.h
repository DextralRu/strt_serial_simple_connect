/****************************************************************************************
*
*				ОА НПО 'РусБиТех'
*
            (с) Copyright 2020, Rusbitech, Moscow
*
*  Все права защищены. Данная программа не предназначена для публикации и копирования.
*  Программа состоит из конфиденциальных, содержащих коммерческую тайну ОА НПО 'РусБиТех'
*  материалов. Любые попытки или участие в изменении кода программы строго ЗАПРЕЩЕНЫ.
*
****************************************************************************************/
/**
 @file	   packet.h
 @author   Ухваркин Сергей <s.ukhvarkin@rusbitech.ru>
 @date     23.04.2021
 @version  1.0
 @brief    Реализация и описание класса Packet
**/

#ifndef PACKET_CLIENT_H
#define PACKET_CLIENT_H

#include <QByteArray>
#include <QBitArray>
#include <QDataStream>
#include <math.h>
#include <algorithm>
#include <QList>
#include <QDebug>
#include "enums-client.h"


namespace BLE {

class Packet {
public:
    enum class TypeDevice
    {
        ALL             = 0xFF,
        ALL_FROM_PROCESSOR_NAME = 0xCC,
        GUN             = 0x01,
        SENSOR_FIGHTER  = 0x02,
        SENSOR_GRENADE  = 0x03,
        SENSOR_MACHINE  = 0x04,
        BPD             = 0x05,
        SENSOR_LANDING  = 0x06,
        PIB             = 0x08,
        KSUI            = 0x09,
        GRENADE_RGD     = 0x17,
        GRENADE_F1      = 0x18,
    };

    enum {
        BYTE_SIZE = 3,
        BYTE_TYPE = 4
    };

    virtual QString name() { return QString(); }
    virtual void printInfo() {}

    void setSource(const QByteArray &raw) { mRaw = raw; }
    QByteArray source() const  { return mRaw; }

    virtual ~Packet() = default;

private:
    QByteArray mRaw;

};

class PacketIncoming : public Packet
{
public:
    enum
    {
        BYTE_ID_KIT_LOW = 5,
        BYTE_ID_KIT_HIGH,
        BYTE_KIT_STATUS
    };
    virtual ~PacketIncoming() = default;
    virtual bool parseData(const QByteArray &data) = 0;
};

class PacketOutgoing : public Packet
{
public:
    virtual ~PacketOutgoing() = default;
    virtual QByteArray data() = 0;
};


/* Опрос устройств */
class PacketPoll : public PacketOutgoing
{
public:
    virtual QString name() override { return QString("Опрос всех устройств"); }
    virtual QByteArray data() override;
};

class PacketPollGun : public PacketPoll
{
public:
    virtual QString name() override { return QString("Опрос оружия"); }
    virtual QByteArray data() override;
};

class PacketPollSensor : public PacketPoll
{
public:
    PacketPollSensor(quint8 typeSensor = 0) { mType = typeSensor; }
    virtual QString name() override { return QString("Опрос датчиков"); }
    virtual QByteArray data() override;

    void setType(quint8 type) { mType = type; }

private:
    quint8 mType = 0;
};

class PacketPollSensorByID : public PacketPollSensor
{
public:
    PacketPollSensorByID(quint8 typeSensor = 0, quint8 id = 0) { mID = id; mType = typeSensor;}
    virtual QString name() override { return QString("Опрос датчика по его идентификатору"); }
    virtual QByteArray data() override;

    void setID(quint8 id) { mID = id; }
    void setType(quint8 type) { mType = type; }

private:
    quint8 mID = 0;
    quint8 mType = 0;
};

class PacketPollSensorByProcessor : public PacketPollSensor
{
public:
    PacketPollSensorByProcessor(quint8 typeSensor = 0, quint64 id = 0) { mID = id; mType = typeSensor;}
    virtual QString name() override { return QString("Опрос датчика по его номеру процессора"); }
    virtual QByteArray data() override;

    void setID(quint64 id) { mID = id; }
    void setType(quint8 type) { mType = type; }

private:
    quint64 mID = 0;
    quint8 mType = 0;
};

class PacketSearchSensors : public PacketPoll
{
public:
    virtual QString name() override { return QString("Опрос всех датчиков по номеру процессора"); }
    virtual QByteArray data() override;
};

class PacketPollUSB : public PacketPoll
{
public:
    virtual QString name() override { return QString("Опрос USB устройство"); }
    virtual QByteArray data() override;
};

class PacketPollBPD : public PacketPoll
{
public:
    virtual QString name() override { return QString("Опрос БПД"); }
    virtual QByteArray data() override;
};



/* Состояние датчика */

class PacketSensorStateByProcessor : public PacketIncoming
{
    enum
    {
        BYTE_SENSOR_ID = 8,
        BYTE_SENSOR_PROCESSOR_NUMBER_LOW = 9,
        BYTE_SENSOR_PROCESSOR_NUMBER_HIGH = 16,
        BYTE_SENSOR_STATE = 17,
        BYTE_SENSOR_BATTERY,
        BYTE_RESERVED
    };

public:
    virtual bool parseData(const QByteArray &data) override;
    virtual QString name() override { return QString("Текущее состояние датчика(по номеру процессора)"); }
    virtual void printInfo() override;

    quint16 kitNumber() const { return mKitNumber; }
    quint8 kitStatus() const { return mKitStatus; }
    quint64 processorNumber() const { return mProcessor; }
    quint8 sensor() const { return mSensor; }
    quint8 sensorState() const { return mSensorState; }
    quint8 battery() const { return mBattery; }
    quint8 type() const { return mType; }


private:
    quint16 mKitNumber = 0;
    quint8 mKitStatus = 0;
    quint64 mProcessor = 0;
    quint8 mSensor = 0;
    quint8 mSensorState = 0;
    quint8 mBattery = 0;
    quint8 mType = 0;

};

class PacketSensorState : public PacketIncoming
{
    enum
    {
        BYTE_SENSOR_ID = 8,
        BYTE_SENSOR_STATE,
        BYTE_SENSOR_BATTERY,
        BYTE_RESERVED
    };

public:
    virtual bool parseData(const QByteArray &data) override;
    virtual QString name() override { return QString("Текущее состояние датчика"); }
    virtual void printInfo() override;

    quint16 kitNumber() const { return mKitNumber; }
    quint8 kitStatus() const { return mKitStatus; }
    quint8 sensor() const { return mSensor; }
    quint8 sensorState() const { return mSensorState; }
    quint8 battery() const { return mBattery; }
    quint8 type() const { return mType; }


private:
    quint16 mKitNumber = 0;
    quint8 mKitStatus = 0;
    quint8 mSensor = 0;
    quint8 mSensorState = 0;
    quint8 mBattery = 0;
    quint8 mType = 0;

};



/* Сообщения от датчиков */

class PacketSensorFlashPart0 : public PacketSensorState
{
    enum
    {
        BYTE_SENSOR_ID = 8,
        BYTE_SENSOR_STATE,
        BYTE_SENSOR_BATTERY,
        BYTE_RESERVED,
        BYTE_FIRST_DATA,
        BYTE_SECOND_DATA,
        BYTE_THIRD_DATA
    };

public:
    virtual bool parseData(const QByteArray &data) override;
    virtual QString name() override { return QString("Вертикальная засветка - часть 0"); }

    quint16 kitNumber() const { return mKitNumber; }
    quint8 kitStatus() const { return mKitStatus; }
    quint8 sensor() const { return mSensor; }
    quint8 sensorState() const { return mSensorState; }
    quint8 battery() const { return mBattery; }
    quint8 ammunition() const  { return mAmmunition; }
    quint8 laserNumber() const  { return mLaserNumber; }
    quint16 fighterID() const { return mFighterID; }
    quint8 type() const { return mType; }


private:
    quint16 mKitNumber = 0;
    quint8 mKitStatus = 0;
    quint8 mSensor = 0;
    quint8 mSensorState = 0;
    quint8 mBattery = 0;
    quint8 mAmmunition = 0;
    quint16 mFighterID = 0;
    quint8 mLaserNumber = 0;
    quint8 mType = 0;

};

class PacketSensorFlashPart1 : public PacketSensorState
{
    enum
    {
        BYTE_SENSOR_ID = 8,
        BYTE_SENSOR_STATE,
        BYTE_SENSOR_BATTERY,
        BYTE_RESERVED,
        BYTE_FIRST_DATA,
        BYTE_SECOND_DATA,
        BYTE_THIRD_DATA
    };

public:
    virtual bool parseData(const QByteArray &data) override;
    virtual QString name() override { return QString("Вертикальная засветка - часть 1"); }

    quint16 kitNumber() const { return mKitNumber; }
    quint8 kitStatus() const { return mKitStatus; }
    quint8 sensor() const { return mSensor; }
    quint8 sensorState() const { return mSensorState; }
    quint8 battery() const { return mBattery; }

    quint16 shotNumber() const  { return mShotNumber; }
    quint8 shotNumberLow() const  { return quint8(mShotNumber & 0x001F); }
    quint8 laserNumber() const  { return mLaserNumber; }
    quint8 xLow() const  { return mX_Low; }

    quint8 type() const { return mType; }


private:
    quint16 mKitNumber = 0;
    quint8 mKitStatus = 0;
    quint8 mSensor = 0;
    quint8 mSensorState = 0;
    quint8 mBattery = 0;
    quint16 mShotNumber = 0;
    quint8 mLaserNumber = 0;
    quint8 mX_Low = 0;
    quint8 mType = 0;

};

class PacketSensorFlashPart2 : public PacketSensorState
{
    enum
    {
        BYTE_SENSOR_ID = 8,
        BYTE_SENSOR_STATE,
        BYTE_SENSOR_BATTERY,
        BYTE_RESERVED,
        BYTE_FIRST_DATA,
        BYTE_SECOND_DATA,
        BYTE_THIRD_DATA
    };

public:
    virtual bool parseData(const QByteArray &data) override;
    virtual QString name() override { return QString("Вертикальная засветка - часть 2"); }

    quint16 kitNumber() const { return mKitNumber; }
    quint8 kitStatus() const { return mKitStatus; }
    quint8 sensor() const { return mSensor; }
    quint8 sensorState() const { return mSensorState; }
    quint8 battery() const { return mBattery; }

    quint8 laserNumber() const  { return mLaserNumber; }
    quint8 x() const  { return mX; }
    quint8 yLow() const  { return mY_Low; }
    quint8 type() const { return mType; }


private:
    quint16 mKitNumber = 0;
    quint8 mKitStatus = 0;
    quint8 mSensor = 0;
    quint8 mSensorState = 0;
    quint8 mBattery = 0;
    quint8 mLaserNumber = 0;
    quint8 mY_Low = 0;
    quint16 mX = 0;
    quint8 mType = 0;

};

class PacketSensorFlashPart3 : public PacketSensorState
{
    enum
    {
        BYTE_SENSOR_ID = 8,
        BYTE_SENSOR_STATE,
        BYTE_SENSOR_BATTERY,
        BYTE_RESERVED,
        BYTE_FIRST_DATA,
        BYTE_SECOND_DATA,
        BYTE_THIRD_DATA
    };

public:
    virtual bool parseData(const QByteArray &data) override;
    virtual QString name() override { return QString("Вертикальная засветка - часть 3"); }

    quint16 kitNumber() const { return mKitNumber; }
    quint8 kitStatus() const { return mKitStatus; }
    quint8 sensor() const { return mSensor; }
    quint8 sensorState() const { return mSensorState; }
    quint8 battery() const { return mBattery; }

    quint8 laserNumber() const  { return mLaserNumber; }
    quint8 y() const  { return mY; }
    quint8 weapon() const  { return mWeapon; }

    quint8 type() const { return mType; }


private:
    quint16 mKitNumber = 0;
    quint8 mKitStatus = 0;
    quint8 mSensor = 0;
    quint8 mSensorState = 0;
    quint8 mBattery = 0;
    quint8 mWeapon = 0;
    quint8 mLaserNumber = 0;
    quint16 mY = 0;
    quint8 mType = 0;

};

class PacketSensorFlashPart4 : public PacketSensorState
{
    enum
    {
        BYTE_SENSOR_ID = 8,
        BYTE_SENSOR_STATE,
        BYTE_SENSOR_BATTERY,
        BYTE_RESERVED,
        BYTE_FIRST_DATA,
        BYTE_SECOND_DATA,
        BYTE_THIRD_DATA
    };

public:
    virtual bool parseData(const QByteArray &data) override;
    virtual QString name() override { return QString("Горизонтальная засветка"); }

    quint16 kitNumber() const { return mKitNumber; }
    quint8 kitStatus() const { return mKitStatus; }
    quint8 sensor() const { return mSensor; }
    quint8 sensorState() const { return mSensorState; }
    quint8 battery() const { return mBattery; }

    quint8 shotNumberLow() const { return mShotNumber; }
    quint16 fighterID() const { return mFighterID; }
    quint8 laserNumber() const { return mLaserNumber; }
    quint8 type() const { return mType; }


private:
    quint16 mKitNumber = 0;
    quint8 mKitStatus = 0;
    quint8 mSensor = 0;
    quint8 mSensorState = 0;
    quint8 mBattery = 0;
    quint8 mShotNumber = 0;
    quint16 mFighterID = 0;
    quint8 mLaserNumber = 0;
    quint8 mType = 0;

};

class PacketVerticalSensor : public PacketIncoming
{
    enum
    {
        BYTE_ID_SENSOR = 5
    };

public:
    virtual bool parseData(const QByteArray &data) override;
    virtual QString name() override { return QString("Вертикальная засветка"); }
    virtual void printInfo() override;

    quint8 sensorID() const  { return mSensorID; }
    quint8 ammunition() const  { return mAmmunition; }
    quint16 shotNumber() const  { return mShotNumber; }

    quint8 shotNumberLow() const  { return quint8(mShotNumber & 0x001F); }

    quint8 laserNumber() const  { return mLaserNumber; }
    quint16 x() const  { return mX; }
    quint16 y() const  { return mY; }
    quint16 fighterID() const { return mFighterID; }
    quint8 weapon() const  { return mWeapon; }

private:
    quint8 mSensorID = 0;
    quint8 mAmmunition = 0;
    quint16 mShotNumber = 0;
    quint8 mLaserNumber = 0;
    quint16 mX = 0;
    quint16 mY = 0;
    quint16 mFighterID = 0;
    quint8 mWeapon = 0;

};

class PacketGorizontalSensor : public PacketIncoming
{
    enum
    {
        BYTE_ID_SENSOR = 8
    };

public:
    virtual bool parseData(const QByteArray &data) override;
    virtual QString name() override { return QString("Горизонтальная засветка"); }
    virtual void printInfo() override;

    quint16 shotNumberLow() const { return mShotNumber; }

    quint16 fighterID() const { return mFighterID; }
    quint8 laserNumber() const { return mLaserNumber; }
    quint8 sensorID() const { return mSensorID; }
    quint16 kitNumber() const { return mKitNumber; }
    quint8 kitStatus() const { return mKitStatus; }

private:
    quint8 mSensorID = 0;
    quint8 mShotNumber = 0;
    quint16 mFighterID = 0;
    quint8 mLaserNumber = 0;
    quint16 mKitNumber = 0;
    quint8 mKitStatus = 0;
};



/* Сообщения от оружия */
class PacketGunState : public PacketIncoming
{
    enum
    {
        BYTE_GUN_STATE = 8,
        BYTE_AMMO_TYPE,
        BYTE_GUN_TYPE,
        BYTE_SHOT_NUMBER_LOW,
        BYTE_SHOT_NUMBER_HIGH,
        BYTE_X_LOW,
        BYTE_X_HIGH,
        BYTE_Y_LOW,
        BYTE_Y_HIGH,
        BYTE_BATTERY,
        BYTE_RESERVED
    };


public:
    virtual bool parseData(const QByteArray &data) override;
    virtual QString name() override { return QString("Состояние оружия"); }
    virtual void printInfo() override;

    quint8 kitStatus() const { return mKitStatus; }
    quint16 kitNumber() const { return mKitNumber; }
    quint8 gun() const { return mGun; }
    quint8 gunState() const { return mGun; }
    quint8 battery() const { return mBattery; }
    quint8 ammunition() const { return mAmmunition; }
    quint16 shotNumber() const { return mShotNumber; }
    quint16 geoX() const { return mGeoX; }
    quint16 geoY() const { return mGeoY; }

private:
    quint16 mKitNumber = 0;
    quint8 mKitStatus = 0;
    quint8 mGun = 0;
    quint8 mGunState = 0;
    quint8 mAmmunition = 0;
    quint16 mShotNumber = 0;
    quint16 mGeoX = 0;
    quint16 mGeoY = 0;
    quint8 mBattery = 0;
};



/* Сообщения от датчика гранаты */
class PacketGrenadeSensorF1 : public PacketIncoming
{
    enum
    {
        BYTE_SENSOR_ID = 8,
        BYTE_SENSOR_STATE,
        BYTE_SENSOR_BATTERY,
        BYTE_RESERVED,
        BYTE_TYPE_GUN,
        BYTE_TYPE_CHANEL,
        BYTE_ID_GRENADE_LOW,
        BYTE_ID_GRENADE_HIGH
    };

public:
    virtual bool parseData(const QByteArray &data) override;
    virtual QString name() override { return QString("Засветка гранатой F1"); }
    virtual void printInfo() override;

    quint16 kitNumber() const { return mKitNumber; }
    quint8 kitStatus() const { return mKitStatus; }
    quint8 sensor() const { return mSensor; }
    quint8 sensorState() const { return mSensorState; }
    quint8 battery() const { return mBattery; }

    quint8 grenade() const { return mGrenadeID; }
    quint8 gun() const { return mGun; }
    quint8 chanel() const { return mChanel; }
    quint8 chanelPower() const { return mChanelPower; }

private:
    quint8 mSensor = 0;
    quint8 mSensorState = 0;
    quint16 mKitStatus = 0;
    quint16 mGrenadeID = 0;
    quint8 mBattery = 0;
    quint8 mGun = 0;
    quint8 mChanel = 0;
    quint8 mChanelPower = 0;
    quint16 mKitNumber = 0;

};

class PacketGrenadeSensorRGD : public PacketIncoming
{
    enum
    {
        BYTE_SENSOR_ID = 8,
        BYTE_SENSOR_STATE,
        BYTE_SENSOR_BATTERY,
        BYTE_RESERVED,
        BYTE_TYPE_GUN,
        BYTE_TYPE_CHANEL,
        BYTE_ID_GRENADE_LOW,
        BYTE_ID_GRENADE_HIGH
    };

public:
    virtual bool parseData(const QByteArray &data) override;
    virtual QString name() override { return QString("Датчик гранаты RGD"); }
    virtual void printInfo() override;

    quint16 kitNumber() const { return mKitNumber; }
    quint8 kitStatus() const { return mKitStatus; }
    quint8 sensor() const { return mSensor; }
    quint8 sensorState() const { return mSensorState; }
    quint8 battery() const { return mBattery; }

    quint8 grenade() const { return mGrenadeID; }
    quint8 gun() const { return mGun; }
    quint8 chanel() const { return mChanel; }
    quint8 chanelPower() const { return mChanelPower; }

private:
    quint8 mSensor = 0;
    quint8 mSensorState = 0;
    quint16 mKitStatus = 0;
    quint16 mGrenadeID = 0;
    quint8 mBattery = 0;
    quint8 mGun = 0;
    quint8 mChanel = 0;
    quint8 mChanelPower = 0;
    quint16 mKitNumber = 0;

};

class PacketGrenadeSensorDD : public PacketIncoming
{
    enum
    {
        BYTE_SENSOR_ID = 8,
        BYTE_SENSOR_STATE,
        BYTE_SENSOR_BATTERY,
        BYTE_RESERVED,
        BYTE_TYPE_GUN,
        BYTE_FIGHTER_STATUS
    };

public:
    virtual bool parseData(const QByteArray &data) override;
    virtual QString name() override { return QString("Датчик гранаты DD"); }
    virtual void printInfo() override;

    quint16 kitNumber() const { return mKitNumber; }
    quint8 kitStatus() const { return mKitStatus; }
    quint8 sensor() const { return mSensor; }
    quint8 sensorState() const { return mSensorState; }
    quint8 battery() const { return mBattery; }

    quint8 gun() const { return mGun; }
    quint8 fighterStatus() const { return mFighterStatus; }


private:
    quint8 mSensor = 0;
    quint8 mSensorState = 0;
    quint16 mKitStatus = 0;
    quint16 mKitNumber = 0;
    quint8 mBattery = 0;
    quint8 mGun = 0;
    quint8 mFighterStatus = 0;

};


/* Сообщения от оружия */
class PacketBpdState : public PacketIncoming
{
public:
    virtual bool parseData(const QByteArray &data) override;
    virtual QString name() override { return QString("Текущее состояние БПД"); }
    virtual void printInfo() override;

    quint16 kitNumber() const { return mKitNumber; }
    quint8 status() const { return mStatus; }
private:
    quint16 mKitNumber = 0;
    quint8 mStatus = 0;
};

class PacketBpdControlKitStatus : public PacketOutgoing
{
public:
    PacketBpdControlKitStatus(quint16 kitNumber, quint8 kitStatus) { mKitNumber = kitNumber; mKitStatus = kitStatus; }
    virtual QString name() override { return QString("Изменить статус устройства"); }
    virtual QByteArray data() override;

    void setKitNumber(quint16 kitNumber) { mKitNumber = kitNumber; }
    void setKitStatus(quint16 kitStatus) { mKitStatus = kitStatus; }

private:
    quint16 mKitNumber = 0;
    quint8 mKitStatus = 0;
};

class PacketKsuiStateReq : public PacketIncoming
{
public:
    virtual bool parseData(const QByteArray &data) override;
    virtual QString name() override { return QString("Запрос от ПИБ статуса вычислителя"); }
    virtual void printInfo() override;

    quint16 kitNumber() const { return mKitNumber; }
private:
    quint16 mKitNumber = 0;
};

class PacketKsuiState : public PacketIncoming
{
public:
    virtual bool parseData(const QByteArray &data) override;
    virtual QString name() override { return QString("Запрос от ПИБ статуса вычислителя"); }
    virtual void printInfo() override;

    quint16 kitNumber() const { return mKitNumber; }
    quint8 status() const { return mStatus; }
private:
    quint16 mKitNumber = 0;
    quint8 mStatus = 0;
};

/* Управление ПИБ */
class PacketSearchPIB : public PacketOutgoing
{
public:
    PacketSearchPIB(quint16 kitNumber = 0) { mKitNumber = kitNumber; }
    virtual QString name() override { return QString("Поиск устройств ПИБ"); }
    virtual QByteArray data() override;

    void setKitNumber(quint16 kitNumber = 0) { mKitNumber = kitNumber; }

private:
    quint16 mKitNumber = 0;
};

class PacketPibState : public PacketIncoming
{
public:
    virtual bool parseData(const QByteArray &data) override;
    virtual QString name() override { return QString("Запрос от ПИБ статуса вычислителя"); }
    virtual void printInfo() override;

    quint16 kitNumber() const { return mKitNumber; }
    quint8 status() const { return mStatus; }
private:
    quint16 mKitNumber = 0;
    quint8 mStatus = 0;
};

class PacketPibControlKitStatus : public PacketOutgoing
{
public:
    PacketPibControlKitStatus(quint16 kitNumber, quint8 kitStatus) { mKitNumber = kitNumber; mKitStatus = kitStatus; }
    virtual QString name() override { return QString("Изменить статус устройства"); }
    virtual QByteArray data() override;

    void setKitNumber(quint16 kitNumber) { mKitNumber = kitNumber; }
    void setKitStatus(quint16 kitStatus) { mKitStatus = kitStatus; }

private:
    quint16 mKitNumber = 0;
    quint8 mKitStatus = 0;
};

class PacketPibReboot : public PacketOutgoing
{
public:
    PacketPibReboot(quint16 kitNumber) { mKitNumber = kitNumber; }

    virtual QString name() override { return QString("Перазагрузить ПИБ"); }
    virtual QByteArray data() override;

    void setKitNumber(quint16 kitNumber = 0) { mKitNumber = kitNumber; }

private:
    quint16 mKitNumber = 0;
};

class PacketPibImitGrenade : public PacketOutgoing
{
public:
    PacketPibImitGrenade(quint16 kitNumber) { mKitNumber = kitNumber; }

    virtual QString name() override { return QString("Имитация гранаты"); }
    virtual QByteArray data() override;

    void setKitNumber(quint16 kitNumber = 0) { mKitNumber = kitNumber; }

private:
    quint16 mKitNumber = 0;
};

class PacketPibImitDesant : public PacketOutgoing
{
public:
    PacketPibImitDesant(quint16 kitNumber) { mKitNumber = kitNumber; }

    virtual QString name() override { return QString("Имитация датчика десанта"); }
    virtual QByteArray data() override;

    void setKitNumber(quint16 kitNumber = 0) { mKitNumber = kitNumber; }

private:
    quint16 mKitNumber = 0;
};

class PacketPibLoadBKCommand : public PacketOutgoing
{
public:
    PacketPibLoadBKCommand(quint16 kitNumber) { mKitNumber = kitNumber; }

    virtual QString name() override { return QString("Загрузка БК"); }
    virtual QByteArray data() override;

    void setKitNumber(quint16 kitNumber = 0) { mKitNumber = kitNumber; }

private:
    quint16 mKitNumber = 0;
};

class PacketPibShotCommand : public PacketOutgoing
{
public:
    PacketPibShotCommand(quint16 kitNumber) { mKitNumber = kitNumber; }

    virtual QString name() override { return QString("Выстрел"); }
    virtual QByteArray data() override;

    void setKitNumber(quint16 kitNumber = 0) { mKitNumber = kitNumber; }

private:
    quint16 mKitNumber = 0;
};

class PacketPibKillCommand : public PacketOutgoing
{
public:
    PacketPibKillCommand(quint16 kitNumber) { mKitNumber = kitNumber; }

    virtual QString name() override { return QString("Поразить"); }
    virtual QByteArray data() override;

    void setKitNumber(quint16 kitNumber = 0) { mKitNumber = kitNumber; }

private:
    quint16 mKitNumber = 0;
};

class PacketPibLiveCommand : public PacketOutgoing
{
public:
    PacketPibLiveCommand(quint16 kitNumber) { mKitNumber = kitNumber; }

    virtual QString name() override { return QString("Оживить"); }
    virtual QByteArray data() override;

    void setKitNumber(quint16 kitNumber = 0) { mKitNumber = kitNumber; }

private:
    quint16 mKitNumber = 0;
};

class PacketPibShotCommandRecv : public PacketIncoming
{
public:
    virtual bool parseData(const QByteArray &data) override;
    virtual QString name() override { return QString("Выстрел от ПИБ"); }
    virtual void printInfo() override;

    quint16 kitNumber() const { return mKitNumber; }
private:
    quint16 mKitNumber = 0;
};

class PacketPibLoadBKCommandRecv : public PacketIncoming
{
public:
    virtual bool parseData(const QByteArray &data) override;
    virtual QString name() override { return QString("Загрузка БК от ПИБ"); }
    virtual void printInfo() override;

    quint16 kitNumber() const { return mKitNumber; }
private:
    quint16 mKitNumber = 0;
};

class PacketPibLiveCommandRecv : public PacketIncoming
{
public:
    virtual bool parseData(const QByteArray &data) override;
    virtual QString name() override { return QString("Оживить от ПИБ"); }
    virtual void printInfo() override;

    quint16 kitNumber() const { return mKitNumber; }
private:
    quint16 mKitNumber = 0;
};

class PacketPibKillCommandRecv : public PacketIncoming
{
public:
    virtual bool parseData(const QByteArray &data) override;
    virtual QString name() override { return QString("Убить от ПИБ"); }
    virtual void printInfo() override;

    quint16 kitNumber() const { return mKitNumber; }
private:
    quint16 mKitNumber = 0;
};

class PacketPibImitLIS : public PacketOutgoing
{
public:
    PacketPibImitLIS(quint16 kitNumber) { mKitNumber = kitNumber; }

    virtual QString name() override { return QString("Имитация ЛИС"); }
    virtual QByteArray data() override;

    void setKitNumber(quint16 kitNumber = 0) { mKitNumber = kitNumber; }

private:
    quint16 mKitNumber = 0;
};

class PacketPibControlBPD : public PacketOutgoing
{
public:
    PacketPibControlBPD(quint16 kitNumber) { mKitNumber = kitNumber; }

    virtual QString name() override { return QString("Запрос состояния БПД"); }
    virtual QByteArray data() override;

    void setKitNumber(quint16 kitNumber = 0) { mKitNumber = kitNumber; }

private:
    quint16 mKitNumber = 0;
};

class PacketPibFindedDesant : public PacketOutgoing
{
public:
    PacketPibFindedDesant(quint16 kitNumber, quint16 desantNumber) { mKitNumber = kitNumber; mDesantNumber = desantNumber; }

    virtual QString name() override { return QString("Найден десант"); }
    virtual QByteArray data() override;

    void setKitNumber(quint16 kitNumber = 0) { mKitNumber = kitNumber; }
    void setDesantNumber(quint16 desantNumber = 0) { mDesantNumber = desantNumber; }

private:
    quint16 mKitNumber = 0;
    quint16 mDesantNumber = 0;
};

class PacketPibRequestDesantSensor : public PacketIncoming
{
public:
    virtual bool parseData(const QByteArray &data) override;
    virtual QString name() override { return QString("Запрос от ДД"); }
    virtual void printInfo() override;

    quint16 idDesant() const { return mIdDesant; }
    quint8 idDevice() const { return mIdDevice; }
private:
    quint16 mIdDesant = 0;
    quint8 mIdDevice = 0;
};

class PacketIrFlashGrenade : public PacketIncoming
{
public:
    virtual bool parseData(const QByteArray &data) override;
    virtual QString name() override { return QString("Засветка от гранаты по ИК"); }
    virtual void printInfo() override;

    quint16 grenadeNumber() const { return m_GrenadeNumber; }
    quint16 type() const { return m_type; }
    quint16 channel() const { return m_channel; }
    quint16 power() const { return m_power; }
private:
    quint16 m_GrenadeNumber = 0;
    quint8 m_type = 0;
    quint8 m_channel = 0;
    quint8 m_power = 0;
};

class PacketPibDevicesRequest : public PacketIncoming
{
public:
    virtual bool parseData(const QByteArray &data) override;
    virtual QString name() override { return QString("Опрос устройств от БПД"); }
    virtual void printInfo() override;
};

class PacketRfFlashGrenadeRgd : public PacketIncoming
{
public:
    virtual bool parseData(const QByteArray &data) override;
    virtual QString name() override { return QString("Засветка от РГД по радио"); }
    virtual void printInfo() override;

    quint16 grenadeNumber() const { return m_GrenadeNumber; }
    quint16 type() const { return m_type; }
    quint16 channel() const { return m_channel; }
    quint16 power() const { return m_power; }
private:
    quint16 m_GrenadeNumber = 0;
    quint8 m_type = 0;
    quint8 m_channel = 0;
    quint8 m_power = 0;
};

class PacketRfFlashGrenadeF1 : public PacketIncoming
{
public:
    virtual bool parseData(const QByteArray &data) override;
    virtual QString name() override { return QString("Засветка от Ф1 по радио"); }
    virtual void printInfo() override;

    quint16 grenadeNumber() const { return m_GrenadeNumber; }
    quint16 type() const { return m_type; }
    quint16 channel() const { return m_channel; }
    quint16 power() const { return m_power; }
private:
    quint16 m_GrenadeNumber = 0;
    quint8 m_type = 0;
    quint8 m_channel = 0;
    quint8 m_power = 0;
};

class PacketPibReceiveWeaponKitState : public PacketIncoming
{
public:
    virtual bool parseData(const QByteArray &data) override;
    virtual QString name() override { return QString("Пакет состояния комплекта"); }
    virtual void printInfo() override;

    quint8 status() const { return mStatus; }
    quint16 kitNumber() { return mKitNumber; }
private:
    quint8 mStatus = 0;
    quint16 mKitNumber = 0;
};

class PacketPibReceiveWeaponDeviceState : public PacketIncoming
{
public:
    virtual bool parseData(const QByteArray &data) override;
    virtual QString name() override { return QString("Пакет состояния устройства"); }
    virtual void printInfo() override;

    quint8 status() const { return mStatus; }
    quint16 kitNumber() { return mKitNumber; }
private:
    quint8 mStatus = 0;
    quint16 mKitNumber = 0;
};

class PacketKSUIControl : public PacketOutgoing
{
public:
    PacketKSUIControl(quint16 kitNumber, quint8 state) { mKit = kitNumber; mState = state; }
    virtual QString name() override { return QString("Статус вычислителя"); }
    virtual QByteArray data() override;

    void setKitNumber(quint16 kitNumber) { mKit = kitNumber; }
    void setState(quint8 state) { mState = state; }

private:
    quint16 mKit = 0;
    quint8 mState = 0;
};

class PacketPibControl : public PacketOutgoing
{
public:
    PacketPibControl(quint16 lastKitNumber = 0, quint16 newKitNumber = 0) { mKitNumber = lastKitNumber; mNewKitNumber = newKitNumber; }
    virtual QString name() override { return QString("Установить новый идентификатор ПИБ"); }
    virtual QByteArray data() override;

    void setLastKitNumber(quint16 kitNumber = 0) { mKitNumber = kitNumber; }
    void setNewKitNumber(quint16 kitNumber = 0) { mNewKitNumber = kitNumber; }

private:
    quint16 mKitNumber = 0;
    quint16 mNewKitNumber = 0;
};

/* Управление оружием */
class PacketGunControlKitNumber : public PacketOutgoing
{
public:
    PacketGunControlKitNumber(quint16 kitNumber, quint16 nextKitNumber) { oldKit = kitNumber; nextKit = nextKitNumber; }
    virtual QString name() override { return QString("Изменить номер комплекта оружия"); }
    virtual QByteArray data() override;

    void setKitNumber(quint16 kitNumber) { oldKit = kitNumber; }
    void setNextKitNumber(quint16 kitNumber) { nextKit = kitNumber; }

private:
    quint16 oldKit = 0;
    quint16 nextKit = 0;
};

class PacketGunControlKitStatus : public PacketOutgoing
{
public:
    PacketGunControlKitStatus(quint16 kitNumber, quint8 kitStatus) { mKitNumber = kitNumber; mKitStatus = kitStatus; }
    virtual QString name() override { return QString("Изменить статус комплекта оружия"); }
    virtual QByteArray data() override;

    void setKitNumber(quint16 kitNumber) { mKitNumber = kitNumber; }
    void setKitStatus(quint16 kitStatus) { mKitStatus = kitStatus; }

private:
    quint16 mKitNumber = 0;
    quint8 mKitStatus = 0;
};

class PacketGunControlStatus : public PacketOutgoing
{
public:
    PacketGunControlStatus(quint16 kitNumber, quint8 status) { mKitNumber = kitNumber; mStatus = status; }
    virtual QString name() override { return QString("Изменить статус оружия"); }
    virtual QByteArray data() override;

    void setKitNumber(quint16 kitNumber) { mKitNumber = kitNumber; }
    void setStatus(quint16 status) { mStatus = status; }

private:
    quint16 mKitNumber = 0;
    quint8 mStatus = 0;
};

class PacketGunControlShotNumber : public PacketOutgoing
{
public:
    PacketGunControlShotNumber(quint16 kitNumber, quint16 shotNumber) { mKitNumber = kitNumber; mShotNumber = shotNumber; }
    virtual QString name() override { return QString("Изменить номер выстрела оружия"); }
    virtual QByteArray data() override;

    void setKitNumber(quint16 kitNumber) { mKitNumber = kitNumber; }
    void setShotNumber(quint16 shotNumber) { mShotNumber = shotNumber; }

private:
    quint16 mKitNumber = 0;
    quint16 mShotNumber = 0;
};

class PacketGunControlCoord : public PacketOutgoing
{
public:
    PacketGunControlCoord(quint16 kitNumber, quint16 x, quint16 y) { mKitNumber = kitNumber; mX = x; mY = y;}
    virtual QString name() override { return QString("Изменить координаты оружия"); }
    virtual QByteArray data() override;

    void setKitNumber(quint16 kitNumber) { mKitNumber = kitNumber; }
    void setX(quint16 geoX) { mX = geoX; }
    void setY(quint16 geoY) { mY = geoY; }

private:
    quint16 mKitNumber = 0;
    quint16 mX = 0;
    quint16 mY = 0;
};

/* Управление датчиками */
class PacketSensorControlKitStatus : public PacketOutgoing
{
public:
    PacketSensorControlKitStatus(quint16 kitNumber = 0, quint8 typeSensor=0, quint8 sensor = 0, quint8 kitStatus = 0)
    {
        mKitNumber = kitNumber;
        mSensor = sensor;
        mKitStatus = kitStatus;
        mType = typeSensor;
    }
    virtual QString name() override { return QString("Установить статус комплекта для датчика"); }
    virtual QByteArray data() override;

    void setKitNumber(quint16 kitNumber = 0) { mKitNumber = kitNumber; }
    void setSensor(quint8 sensor = 0) { mSensor = sensor; }
    void setKitStatus(quint8 kitStatus = 0) { mKitStatus = kitStatus; }
    void setType(quint8 type = 0) { mType = type; }


private:
    quint16 mKitNumber = 0;
    quint8 mSensor = 0;
    quint8 mKitStatus = 0;
    quint8 mType = 0;
};

class PacketSensorControlStatus : public PacketOutgoing
{
public:
    PacketSensorControlStatus(quint16 kitNumber = 0, quint8 typeSensor=0, quint8 sensor = 0, quint8 deviceStatus = 0)
    {
        mKitNumber = kitNumber;
        mSensor = sensor;
        mStatus = deviceStatus;
        mType = typeSensor;
    }
    virtual QString name() override { return QString("Установить статус устройства для датчика"); }
    virtual QByteArray data() override;

    void setKitNumber(quint16 kitNumber = 0) { mKitNumber = kitNumber; }
    void setSensor(quint8 sensor = 0) { mSensor = sensor; }
    void setStatus(quint8 status = 0) { mStatus = status; }
    void setType(quint8 type = 0) { mType = type; }


private:
    quint16 mKitNumber = 0;
    quint8 mSensor = 0;
    quint8 mStatus = 0;
    quint8 mType = 0;
};

class PacketSensorControlKitNumber : public PacketOutgoing
{
public:
    PacketSensorControlKitNumber(quint16 kitNumber = 0, quint8 typeSensor=0, quint8 sensor = 0, quint16 newKitNumber = 0)
    {
        mKitNumber = kitNumber;
        mSensor = sensor;
        mNewKitNumber = newKitNumber;
        mType = typeSensor;
    }
    virtual QString name() override { return QString("Установить идентификатор комплекта для датчика"); }
    virtual QByteArray data() override;

    void setKitNumber(quint16 kitNumber = 0) { mKitNumber = kitNumber; }
    void setNewKitNumber(quint16 kitNumber = 0) { mNewKitNumber = kitNumber; }
    void setSensor(quint8 sensor = 0) { mSensor = sensor; }
    void setType(quint8 type = 0) { mType = type; }


private:
    quint16 mKitNumber = 0;
    quint16 mNewKitNumber = 0;
    quint8 mSensor = 0;
    quint8 mType = 0;
};

class PacketSensorControlId : public PacketOutgoing
{
public:
    PacketSensorControlId(quint16 kitNumber = 0, quint8 typeSensor=0, quint8 sensorId = 0, quint8 newSensorId = 0)
    {
        mKitNumber = kitNumber;
        mSensor = sensorId;
        mNewSensorId = newSensorId;
        mType = typeSensor;
    }
    virtual QString name() override { return QString("Установить идентификатор датчика"); }
    virtual QByteArray data() override;

    void setKitNumber(quint16 kitNumber = 0) { mKitNumber = kitNumber; }
    void setNewSensorId(quint8 sensor = 0) { mNewSensorId = sensor; }
    void setSensor(quint8 sensor = 0) { mSensor = sensor; }
    void setType(quint8 type = 0) { mType = type; }



private:
    quint16 mKitNumber = 0;
    quint8 mNewSensorId = 0;
    quint8 mSensor = 0;
    quint8 mType = 0;
};

class PacketSensorControlIdByProcessor : public PacketOutgoing
{
public:
    PacketSensorControlIdByProcessor(quint16 kitNumber = 0, quint8 typeSensor=0, quint64 processorNumber = 0, quint8 newSensorId = 0)
    {
        mKitNumber = kitNumber;
        mSensor = processorNumber;
        mNewSensorId = newSensorId;
        mType = typeSensor;
    }
    virtual QString name() override { return QString("Установить идентификатор датчика по номеру процессора"); }
    virtual QByteArray data() override;

    void setKitNumber(quint16 kitNumber = 0) { mKitNumber = kitNumber; }
    void setNewSensorId(quint8 sensor = 0) { mNewSensorId = sensor; }
    void setProcessorNumber(quint64 number = 0) { mSensor = number; }
    void setType(quint8 type = 0) { mType = type; }


private:
    quint16 mKitNumber = 0;
    quint8 mNewSensorId = 0;
    quint64 mSensor = 0;
    quint8 mType = 0;
};


/* Управление БПД */
class PacketSearchBPD : public PacketOutgoing
{
public:
    PacketSearchBPD(quint16 kitNumber = 0) { mKitNumber = kitNumber; }
    virtual QString name() override { return QString("Поиск устройств БПД"); }
    virtual QByteArray data() override;

    void setKitNumber(quint16 kitNumber = 0) { mKitNumber = kitNumber; }

private:
    quint16 mKitNumber = 0;
};

class PacketBpdControl : public PacketOutgoing
{
public:
    PacketBpdControl(quint16 lastKitNumber = 0, quint16 newKitNumber = 0) { mKitNumber = lastKitNumber; mNewKitNumber = newKitNumber; }
    virtual QString name() override { return QString("Установить новый идентификатор БПД"); }
    virtual QByteArray data() override;

    void setLastKitNumber(quint16 kitNumber = 0) { mKitNumber = kitNumber; }
    void setNewKitNumber(quint16 kitNumber = 0) { mNewKitNumber = kitNumber; }

private:
    quint16 mKitNumber = 0;
    quint16 mNewKitNumber = 0;
};

class PacketBpdReboot : public PacketOutgoing
{
public:
    PacketBpdReboot(quint16 kitNumber) { mKitNumber = kitNumber; }

    virtual QString name() override { return QString("Перазагрузить БПД"); }
    virtual QByteArray data() override;

    void setKitNumber(quint16 kitNumber = 0) { mKitNumber = kitNumber; }

private:
    quint16 mKitNumber = 0;
};



}

namespace CAN
{

const int MAX_LENGHT_FRAME = 64;

struct CanFrame
{
    uint32_t id = 0;
    uint8_t len = 0;
    uint8_t flags = 0;
    uint8_t data[MAX_LENGHT_FRAME];

    void fromByteArray(const QByteArray array)
    {
        len = array.size();

        for(int i = 0; i < len; i++)
        {
            data[i] = uint8_t(array[i]);
        }
    }

    const QByteArray toArray() const
    {
        QByteArray packet;
        packet.append(id & 0xFF);
        packet.append((id >> 8) & 0xFF);
        packet.append(len);
        packet.append((char*)data, len);

        return packet;
    }
};

class Packet {
public:

    virtual QString name() { return QString(); }
    virtual void printInfo() {}



    virtual ~Packet() = default;

};

class PacketIncoming : public Packet
{
public:
    virtual ~PacketIncoming() = default;
    virtual bool parseFrame(const CanFrame &frame) = 0;
};

class PacketOutgoing : public Packet
{
public:
    virtual CanFrame frame() const = 0;
    virtual ~PacketOutgoing() = default;

};

class PacketPollDevice : public PacketOutgoing
{
    enum
    {
        TYPE = 0xFF07
    };
public:
    virtual QString name() override { return QString("Опрос устройств на CAN шине"); }
    virtual CanFrame frame() const override
    {
        CanFrame frame;
        frame.id = TYPE;
        return frame;
    }
};

class PacketSimulationMode : public PacketOutgoing
{
    enum
    {
        TYPE = 0x0301
    };
public:
    PacketSimulationMode(const bool mode = 0) { mMode = mode; }

    virtual QString name() override { return QString("Режим имитации"); }
    virtual CanFrame frame() const override
    {
        CanFrame frame;
        QByteArray payload;

        frame.id = TYPE;
        payload.append(char(mMode));
        frame.fromByteArray(payload);

        return frame;
    }

    void setMode(const bool mode) { mMode = mode; }


private:
    bool mMode;
};

class PacketWeaponStatus : public PacketOutgoing
{
    enum
    {
        TYPE = 0x0300
    };
public:
    PacketWeaponStatus(const bool status = 0) { mStatus = status;  }
    virtual QString name() override { return QString("Статус вооружения"); }
    virtual CanFrame frame() const override
    {
        CanFrame frame;
        QByteArray payload;

        frame.id = TYPE;
        payload.append(char(mStatus));
        frame.fromByteArray(payload);

        return frame;
    }

    void setStatus(const bool status) { mStatus = status; }


private:
    bool mStatus;
};

class PacketAmmunitionLoading : public PacketOutgoing
{
    enum
    {
        TYPE = 0x0468
    };
public:
    PacketAmmunitionLoading(const bool isLoading = 0) { mLoading = isLoading; }
    virtual QString name() override { return QString("Процесс зарядки боеприпасов"); }
    virtual CanFrame frame() const override
    {
        CanFrame frame;
        QByteArray payload;

        frame.id = TYPE;
        payload.append(char(mLoading));
        frame.fromByteArray(payload);

        return frame;
    }

    void setLoading(const bool loading) { mLoading = loading; }


private:
    bool mLoading;
};

class PacketAmmunitionBlocking : public PacketOutgoing
{
    enum
    {
        TYPE = 0x0467
    };
public:
    PacketAmmunitionBlocking(const quint8 ammoID = 0, const bool status = 0)
    {
        mStatus = status;
        mID = ammoID;
    }
    virtual QString name() override { return QString("Блокировка боеприпаса"); }
    virtual CanFrame frame() const override
    {
        CanFrame frame;
        QByteArray payload;

        frame.id = TYPE;
        payload.append(mID).append(char(mStatus));
        frame.fromByteArray(payload);

        return frame;
    }

    void setStatus(const quint8 ammoID, const bool status) { mID = ammoID; mStatus = status; }

private:
    bool mStatus;
    quint8 mID;
};


/**
  * @brief PacketTypeAmmunition
  * \note пакет с типом боеприпаса и порядковым номером
  * @param seqNumber порядковый номер боеприпаса на экране
  * @param ID уникальный идентификатор боеприпаса
  */
class PacketAmmunitionType : public PacketOutgoing
{
    enum
    {
        TYPE = 0x0200
    };
public:
    PacketAmmunitionType(const quint8 seqNumber = 0, const quint8 ID = 0)
    {
        mID = ID;
        mSeq = seqNumber;
    }

    virtual QString name() override { return QString("Тип боеприпаса"); }
    virtual CanFrame frame() const override
    {
        CanFrame frame;
        QByteArray payload;

        frame.id = TYPE;
        payload.append(mSeq).append(mID);
        frame.fromByteArray(payload);

        return frame;
    }

    void setSeqNumber(const quint8 seqNumber, const quint8 ID)
    {
        mID = ID;
        mSeq = seqNumber;
    }

private:
    quint8 mID;
    quint8 mSeq;
};

class PacketAmmunitionQuantity : public PacketOutgoing
{
    enum
    {
        TYPE = 0x0466
    };
public:
    PacketAmmunitionQuantity(const quint8 ID = 0, const quint16 quantity = 0)
    {
        mID = ID;
        mQuantity = quantity;
    }
    virtual QString name() override { return QString("Количество боеприпаса"); }
    virtual CanFrame frame() const override
    {
        CanFrame frame;
        QByteArray payload;

        frame.id = TYPE;
        payload.append(mID).append((quint8)(mQuantity & 0x00FF)).append((quint8)(mQuantity >> 8));
        frame.fromByteArray(payload);

        return frame;
    }

    void setAmmunition(const quint8 ID = 0, const quint16 quantity = 0)
    {
        mID = ID;
        mQuantity = quantity;
    }

private:
    quint8 mID;
    quint16 mQuantity;
};

class PacketDirectionFiring : public PacketOutgoing
{
    enum
    {
        TYPE = 0x0469
    };
public:
    PacketDirectionFiring(const quint8 direction = 0, const quint8 timeMs = 0)
    {
        mDirection = direction;
        mTime = timeMs;
    }

    virtual QString name() override { return QString("Направление обстрела"); }
    virtual CanFrame frame() const override
    {
        CanFrame frame;
        QByteArray payload;

        frame.id = TYPE;
        payload.append(mDirection).append(mTime);
        frame.fromByteArray(payload);

        return frame;
    }

    void setDirection(const quint8 direction = 0, const quint8 timeMs = 0)
    {
        mDirection = direction;
        mTime = timeMs;
    }
private:
    quint8 mDirection;
    quint8 mTime;
};

class PacketLandingStatus : public PacketOutgoing
{
    enum
    {
        TYPE = 0x0501
    };
public:
    PacketLandingStatus(const quint16 landing_id = 0, const quint8 landing_status = 0)
    {
        mLanding = landing_id;
        mStatus = landing_status;
    }

    virtual QString name() override { return QString("Новый статус десанта"); }
    virtual CanFrame frame() const override
    {
        CanFrame frame;
        QByteArray payload;

        frame.id = TYPE;

        payload.append((quint8)(mLanding & 0x00FF))
               .append((quint8)(mLanding >> 8))
               .append(mStatus);

        frame.fromByteArray(payload);

        return frame;
    }

private:
    quint16 mLanding = 0;
    quint8 mStatus = 0;
};


class PacketDeviceOnline : public PacketIncoming
{
    enum
    {
        BYTE_DEVICE_TYPE = 0,
        BYTE_WEAPON,
        BYTE_DEVICE_NUMBER
    };
public:
    virtual bool parseFrame(const CanFrame &frame) override;
    virtual void printInfo() override;

    quint8 type() const { return mType; }
    quint8 weapon() const { return mType; }
    quint8 number() const { return mType; }

private:
    quint8 mType = 0;
    quint8 mWeapon = 0;
    quint8 mNumber = 0;
};

class PacketLandingSynchronization : public PacketIncoming
{
    enum
    {
        BYTE_LANDING_ID = 0
    };
public:
    virtual bool parseFrame(const CanFrame &frame);

    quint8 landing() const { return mLanding; }

private:
    quint8 mLanding = 0;
};

class PacketAmmunitionSalvo : public PacketIncoming
{
    enum
    {
        BYTE_AMMUNITION_ID = 0,
    };
public:
    virtual bool parseFrame(const CanFrame &frame) override;
    virtual void printInfo() override;

    quint8 ammunition() const { return mAmmunition; }

private:
    quint8 mAmmunition = 0;
};

class PacketCharging : public PacketIncoming
{
public:
    virtual bool parseFrame(const CanFrame &frame) override;
    virtual void printInfo() override;

};

class PacketLandingFound : public PacketIncoming
{
    enum
    {
        BYTE_ID_LOW = 0,
        BYTE_ID_HIGH,
        BYTE_STATUS,
        BYTE_ID_SENSOR
    };
public:
    virtual bool parseFrame(const CanFrame &frame) override;
    virtual void printInfo() override;

    quint16 landing() const { return mLanding; }
    quint8 status() const { return mStatus; }
    quint8 sensor() const { return mSensor; }

private:
    quint16 mLanding = 0;
    quint8 mStatus = 0;
    quint8 mSensor = 0;
};

}

class Packet {
public:
    enum {
        BYTE_SIZE = 1,
        BYTE_TYPE = 2
    };

    virtual QString name() { return QString(); }
    virtual void printInfo() {}

    virtual ~Packet() = default;

};

class PacketIncoming : public Packet
{
public:
    enum {
        BYTE_ID_FIRST_HALF = 3,
        BYTE_ID_SECOND_HALF
    };

    virtual ~PacketIncoming() = default;
    virtual bool parseData(const QByteArray &data) = 0;

    virtual quint16 checkSum() const { return 0; }
    virtual quint16 address() const { return -1; }
};

class PacketOutgoing : public Packet
{
public:
    virtual ~PacketOutgoing() = default;
    virtual QByteArray data() = 0;

    QString toString() { return QString().fromLatin1(data().toHex()); }

    void setAddress(const quint16 address) { mAddress = address; }
    quint16 address() const { return mAddress; }

private:
    quint16 mAddress = 0;
};

class PacketStatusQuery : public PacketIncoming
{
public:
    virtual bool parseData(const QByteArray &data) override;
    virtual quint16 checkSum() const override { return mCheckSum; }
    virtual QString name() override { return "Запрос статуса"; }

private:
    quint16 mCheckSum = 0;
};

class PacketPolygonCenter : public PacketIncoming
{
public:
    virtual bool parseData(const QByteArray &data) override;
    virtual quint16 checkSum() const override { return mCheckSum; }
    virtual QString name() override { return "Координата центра полигона"; }
    virtual void printInfo() override;

    float Latitude() const { return mLat; }
    float Longitude() const { return mLon; }

private:
    quint16 mCheckSum = 0;
    float mLat = 0;
    float mLon = 0;
};

class PacketKill : public PacketIncoming
{
public:
    virtual QString name() override { return "Команда -Убить-"; }
    virtual bool parseData(const QByteArray &data) override;

    virtual quint16 checkSum() const override { return mCheckSum; }
    virtual quint16 address() const override { return mAddress; }

private:
    quint16 mCheckSum = 0;
    quint16 mAddress = 0;
};

class PacketReload : public PacketIncoming
{
public:
    virtual QString name() override { return "Команда -Оживить-"; }
    virtual bool parseData(const QByteArray &data) override;

    virtual quint16 checkSum() const override { return mCheckSum; }
    virtual quint16 address() const override { return mAddress; }

private:
    quint16 mCheckSum = 0;
    quint16 mAddress = 0;
};

class PacketMiss : public PacketIncoming
{
public:
    virtual QString name() override { return "Команда -Промах-"; }
    virtual bool parseData(const QByteArray &data) override;

    virtual quint16 checkSum() const override { return mCheckSum; }
    virtual quint16 address() const override { return mAddress; }

private:
    quint16 mCheckSum = 0;
    quint16 mAddress = 0;
};

class PacketAmmunition : public PacketIncoming
{
public:
    virtual QString name() override { return "Команда -Установить боекомплект-"; }
    virtual bool parseData(const QByteArray &data) override;

    virtual quint16 checkSum() const override { return mCheckSum; }
    virtual quint16 address() const override { return mAddress; }

private:
    quint16 mCheckSum = 0;
    quint16 mAddress = 0;
};

class PacketConfirmMessage : public PacketIncoming
{
public:
    enum {
        BYTE_PACKET_KEY_FIRST_HALF = 5,
        BYTE_PACKET_KEY_SECOND_HALF = 6
    };
    virtual QString name() override { return "Подтверждение о доставке сообщения"; }
    virtual bool parseData(const QByteArray &data) override;

    virtual quint16 checkSum() const override { return mCheckSum; }
    virtual quint16 address() const override { return mAddress; }

    quint16 key() const { return mKey; }

private:
    quint16 mCheckSum = 0;
    quint16 mAddress = 0;
    quint16 mKey = 0;
};

class PacketWeaponShot : public PacketOutgoing
{
public:
    virtual QByteArray data() override;

    void setState(const quint8 state) { mState = state; }
    void setGunType(const quint8 gunType) { mGunType = gunType; }
    void setShotNumber(const quint16 shotNumber) { mShotNumber = shotNumber; }


private:
    quint8 mState = 0;
    quint8 mGunType = 0;
    quint16 mShotNumber = 0;
};

class PacketStatus : public PacketOutgoing
{
public:
    PacketStatus(const quint16 address, const quint8 status) { setAddress(address); mStatus = status; }

    virtual QByteArray data() override;

    void setStatus(const quint8 status) { mStatus = status; }

private:
    quint8 mStatus = 0;
};

class PacketPosition : public PacketOutgoing
{
public:
    PacketPosition(const quint16 address,
                   const quint16 azimuth,
                   const float latitude,
                   const float longitude) { setAddress(address); mAzimuth = azimuth; mLatitude = latitude; mLongitude = longitude; }
    PacketPosition() {}
    virtual QByteArray data() override;
    virtual QString name() override { return "Координаты и азимут"; }

    void setAzimuth(const qint16 azimuth) { mAzimuth = azimuth; }
    void setLatitude(const float latitude) { mLatitude = latitude; }
    void setLongitude(const float longitude) { mLongitude = longitude; }

private:
    qint16 mAzimuth;
    float mLatitude;
    float mLongitude;
};

class PacketFlashSensor : public PacketOutgoing
{
public:
    QByteArray data() override;

    void setSensor(const qint8 sensor) { mSensor = sensor; }
    void setFighter(const qint16 fighter) { mFighter = fighter; }
    void setWeapon(const qint16 gun) { mWeapon = gun; }
    void setAmmunition() {}
    void setShotNumber(const qint16 shot) { mShot = shot; }
    void setX(const qint16 x) { mX = x; }
    void setY(const qint16 y) { mY = y; }

    void appendLaser(qint8 laser) { mLasers.append(laser); }

private:
    QList<qint8> mLasers;     /* Список лучей */
    qint8 mSensor = 0;        /* Номер датчика */
    qint16 mFighter = 0;       /* ID стрелявшего бойца */
    qint8 mWeapon = 0;         /* Тип оружия */
    qint8 mAmmunition = 0;     /* Тип боеприпаса */
    qint16 mShot = 0;          /* Номер выстрела */
    qint16 mX = 0;             /* координата бойца оХ */
    qint16 mY = 0;             /* координата бойца оУ */
};

class PacketAddressChange : public PacketIncoming
{
public:
    enum {
        BYTE_PACKET_KEY_FIRST_HALF = 5,
        BYTE_PACKET_KEY_SECOND_HALF = 6
    };
    virtual QString name() override { return "Команда -Изменить идентификатор-"; }
    virtual bool parseData(const QByteArray &data) override;

    virtual quint16 checkSum() const override { return mCheckSum; }
    virtual quint16 address() const override { return mAddress; }

    quint16 newAddress() const { return mNewAddress; }

private:
    quint16 mCheckSum = 0;
    quint16 mAddress = 0;
    quint16 mNewAddress = 0;
};

class PacketDeviceInfo : public PacketOutgoing
{
public:
    QByteArray data() override;
    virtual QString name() override { return "Информация о подключенных устройствах"; }

    void setSensors(QList <qint8> sensors) { mListSensor = sensors; }

private:
    QList <qint8> mListSensor;
};

class PacketConfiguration : public PacketIncoming
{
public:
    virtual QString name() override { return "Параметры настройки устройства"; }
    virtual bool parseData(const QByteArray &data) override;

    virtual quint16 checkSum() const override { return mCheckSum; }
    virtual quint16 address() const override { return mAddress; }

private:
    quint16 mCheckSum = 0;
    quint16 mAddress = 0;
};

namespace CRC {

const unsigned short crc16Table[256] = {
    0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50A5, 0x60C6, 0x70E7,
    0x8108, 0x9129, 0xA14A, 0xB16B, 0xC18C, 0xD1AD, 0xE1CE, 0xF1EF,
    0x1231, 0x0210, 0x3273, 0x2252, 0x52B5, 0x4294, 0x72F7, 0x62D6,
    0x9339, 0x8318, 0xB37B, 0xA35A, 0xD3BD, 0xC39C, 0xF3FF, 0xE3DE,
    0x2462, 0x3443, 0x0420, 0x1401, 0x64E6, 0x74C7, 0x44A4, 0x5485,
    0xA56A, 0xB54B, 0x8528, 0x9509, 0xE5EE, 0xF5CF, 0xC5AC, 0xD58D,
    0x3653, 0x2672, 0x1611, 0x0630, 0x76D7, 0x66F6, 0x5695, 0x46B4,
    0xB75B, 0xA77A, 0x9719, 0x8738, 0xF7DF, 0xE7FE, 0xD79D, 0xC7BC,
    0x48C4, 0x58E5, 0x6886, 0x78A7, 0x0840, 0x1861, 0x2802, 0x3823,
    0xC9CC, 0xD9ED, 0xE98E, 0xF9AF, 0x8948, 0x9969, 0xA90A, 0xB92B,
    0x5AF5, 0x4AD4, 0x7AB7, 0x6A96, 0x1A71, 0x0A50, 0x3A33, 0x2A12,
    0xDBFD, 0xCBDC, 0xFBBF, 0xEB9E, 0x9B79, 0x8B58, 0xBB3B, 0xAB1A,
    0x6CA6, 0x7C87, 0x4CE4, 0x5CC5, 0x2C22, 0x3C03, 0x0C60, 0x1C41,
    0xEDAE, 0xFD8F, 0xCDEC, 0xDDCD, 0xAD2A, 0xBD0B, 0x8D68, 0x9D49,
    0x7E97, 0x6EB6, 0x5ED5, 0x4EF4, 0x3E13, 0x2E32, 0x1E51, 0x0E70,
    0xFF9F, 0xEFBE, 0xDFDD, 0xCFFC, 0xBF1B, 0xAF3A, 0x9F59, 0x8F78,
    0x9188, 0x81A9, 0xB1CA, 0xA1EB, 0xD10C, 0xC12D, 0xF14E, 0xE16F,
    0x1080, 0x00A1, 0x30C2, 0x20E3, 0x5004, 0x4025, 0x7046, 0x6067,
    0x83B9, 0x9398, 0xA3FB, 0xB3DA, 0xC33D, 0xD31C, 0xE37F, 0xF35E,
    0x02B1, 0x1290, 0x22F3, 0x32D2, 0x4235, 0x5214, 0x6277, 0x7256,
    0xB5EA, 0xA5CB, 0x95A8, 0x8589, 0xF56E, 0xE54F, 0xD52C, 0xC50D,
    0x34E2, 0x24C3, 0x14A0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
    0xA7DB, 0xB7FA, 0x8799, 0x97B8, 0xE75F, 0xF77E, 0xC71D, 0xD73C,
    0x26D3, 0x36F2, 0x0691, 0x16B0, 0x6657, 0x7676, 0x4615, 0x5634,
    0xD94C, 0xC96D, 0xF90E, 0xE92F, 0x99C8, 0x89E9, 0xB98A, 0xA9AB,
    0x5844, 0x4865, 0x7806, 0x6827, 0x18C0, 0x08E1, 0x3882, 0x28A3,
    0xCB7D, 0xDB5C, 0xEB3F, 0xFB1E, 0x8BF9, 0x9BD8, 0xABBB, 0xBB9A,
    0x4A75, 0x5A54, 0x6A37, 0x7A16, 0x0AF1, 0x1AD0, 0x2AB3, 0x3A92,
    0xFD2E, 0xED0F, 0xDD6C, 0xCD4D, 0xBDAA, 0xAD8B, 0x9DE8, 0x8DC9,
    0x7C26, 0x6C07, 0x5C64, 0x4C45, 0x3CA2, 0x2C83, 0x1CE0, 0x0CC1,
    0xEF1F, 0xFF3E, 0xCF5D, 0xDF7C, 0xAF9B, 0xBFBA, 0x8FD9, 0x9FF8,
    0x6E17, 0x7E36, 0x4E55, 0x5E74, 0x2E93, 0x3EB2, 0x0ED1, 0x1EF0
};

const char PACKET_START_BYTE = 0xAA;                        // Начальный байт в пакете
const int PACKET_OVERHEAD_BYTES = 4;                        // Служебная информация (0xAA  SIZE  CRC8+CRC8)
const int PACKET_MIN_BYTES = PACKET_OVERHEAD_BYTES + 1;     // Минимальный размер пакета
const int PACKET_MAX_BYTES = 255;                           // Максимальный размер пакета

quint16 calculateCRC(QByteArray payload);
QByteArray wrap(QByteArray payload);

}

#endif // PACKET_CLIENT_H
