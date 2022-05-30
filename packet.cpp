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
 @file	   packet.cpp
 @author   Ухваркин Сергей <s.ukhvarkin@rusbitech.ru>
 @date     22.09.2020
 @version  1.0
 @brief    Реализация и описание класса Packet
**/

#include <bitset>
#include "packet.h"

QByteArray bitsToBytes(const QBitArray &__bits)
{
    //
    QByteArray bytes;
    bytes.resize(__bits.count() / 8);    // Резервируем место
    bytes.fill(0);                           // Заполняем всё нулями

    // Конвертируем из QBitArray в QByteArray
    for(int b = 0; b < __bits.count(); ++b)
    {
        bytes[b/8] = (bytes.at(b/8) | ( (__bits.testBit(b) ? 1 : 0)<<(7-(b%8)) ) );
        // Конструкция (7-(b%8)) даёт прямой порядок. Для обратного (b%8)
    }

    return bytes;
}


bool BLE::PacketVerticalSensor::parseData(const QByteArray &data)
{
    if(data.size()!=18) return false;
    if(data[BYTE_SIZE]!=0x0E) return false;
    if(data[BYTE_TYPE]!=0x02) return false;

    if(quint8((data[6]&0xE0)>>5) != 0) return false;
    if(quint8((data[9]&0xE0)>>5) != 1) return false;
    if(quint8((data[12]&0xE0)>>5) != 2) return false;
    if(quint8((data[15]&0xE0)>>5) != 3) return false;

    setSource(data);

    mSensorID = quint8(data[BYTE_ID_SENSOR]);
    mAmmunition = quint8((data[6]&0x1F));
    mFighterID = ((quint16(data[7])<<3) | (quint16(data[8]&0xE0)>>5));
    mLaserNumber = quint8(data[8]&0x1F);
    mShotNumber = ((((quint16)data[10])<<3) | (((quint16)data[11])>>5));
    mX = ((((quint16)data[13])<<3) | (((quint16)data[14])>>5));
    mWeapon = quint8((data[15]&0x1F));
    mY = ((((quint16)data[16])<<3) | (((quint16)data[17])>>5));

    return true;
}

void BLE::PacketVerticalSensor::printInfo()
{
    qInfo() << "---! Пакет с вертикальной засветкой !---";
    qDebug() << "raw: " << source().toHex();

    qDebug() << "\tНомер датчика:" << sensorID()
     << "\n\tТип оружия: " << weapon()
     << "\n\tТип боеприпаса: " << ammunition()
     << "\n\tНомер выстрела: " << shotNumber()
     << "\n\tНомер лазера: " << laserNumber()
     << "\n\tX: " << x() << "\n\tY: " << y()
     << "\n\tИдентификатор стрелка: " << fighterID() << endl;
}


bool BLE::PacketGorizontalSensor::parseData(const QByteArray &data)
{
    if(data.size()!=12) return false;
    if(data[BYTE_SIZE]!=0x08) return false;
    if(data[BYTE_TYPE]!=0x02) return false;

    if(quint8((data[9]&0xE0)>>5) != 4) return false;

    setSource(data);

    mKitNumber = (((data[BYTE_ID_KIT_HIGH] << 8) & 0xFF00) | (data[BYTE_ID_KIT_LOW] & 0x00FF));
    mKitStatus = quint8(data[BYTE_KIT_STATUS]);

    mSensorID = quint8(data[BYTE_ID_SENSOR]);
    mShotNumber= quint8((data[9]&0x1F));
    mFighterID = ((((quint16)data[10])<<3) | (((quint16)data[11])>>5));
    mLaserNumber = quint8(data[11]&0x1F);

    return true;
}

void BLE::PacketGorizontalSensor::printInfo()
{
    qInfo() << "---! Пакет с горизонтальной засветкой !---";
    qDebug() << "raw: " << source().toHex();

    qDebug() << "\tНомер датчика: "<< sensorID()
             << "\n\tНомер выстрела: " << shotNumberLow()
             << "\n\tНомер лазера: " << laserNumber()
             << "\n\tИдентификатор стрелка: " << fighterID()
             << "\n\tИдентификатор комплекта: " << kitNumber()
             << "\n\tСтатус комплекта: " << kitStatus()<< endl;
}

bool BLE::PacketGunState::parseData(const QByteArray &data)
{
    if(data.size()!=19) return false;
    if(data[BYTE_SIZE]!=0x0F) return false;
    if(data[BYTE_TYPE]!=(quint8)TypeDevice::GUN) return false;

    setSource(data);

    mKitNumber = (((data[BYTE_ID_KIT_HIGH] << 8) & 0xFF00) | (data[BYTE_ID_KIT_LOW] & 0x00FF));
    mKitStatus = data[BYTE_KIT_STATUS];
    mAmmunition = data[BYTE_AMMO_TYPE];
    mGun = data[BYTE_GUN_TYPE];
    mGunState = data[BYTE_GUN_STATE];
    mShotNumber = (((data[BYTE_SHOT_NUMBER_HIGH] << 8) & 0xFF00) | (data[BYTE_SHOT_NUMBER_LOW] & 0x00FF));
    mGeoX = (((data[BYTE_X_HIGH] << 8) & 0xFF00) | (data[BYTE_X_LOW] & 0x00FF));
    mGeoY = (((data[BYTE_Y_HIGH] << 8) & 0xFF00) | (data[BYTE_Y_LOW] & 0x00FF));
    mBattery = data[BYTE_BATTERY];

    return true;
}

void BLE::PacketGunState::printInfo()
{
    qInfo() << "---! Пакет с данными из оружия !---";
    qDebug() << "raw: " << source().toHex();

    qDebug() << "\tНомер комплекта: "<< kitNumber()
             << "\n\tСтатус комплекта: " << kitStatus()
             << "\n\tТип оружия: " << gun()
             << "\n\tСтатус оружия: " << gunState()
             << "\n\tТип вооружения: " << ammunition()
             << "\n\tНомер выстрела: " << shotNumber()
             << "\n\tКоордината квадрата Х: " << geoX()
             << "\n\tКоордината квадрата У: " << geoY()
             << "\n\tЗаряд аккумулятора: " << battery() << endl;
}

quint16 CRC::calculateCRC(QByteArray payload)
{
    unsigned short crc = 0xFFFF;
    unsigned short len = payload.size();
    char *pcBlock = payload.data();

    while (len--)
        crc = (crc << 8) ^ crc16Table[(crc >> 8) ^ *pcBlock++];

    return crc;
}

QByteArray CRC::wrap(QByteArray payload)
{
    QByteArray sender;
    sender.append(PACKET_START_BYTE).append(payload.size() + PACKET_OVERHEAD_BYTES);
    sender.append(payload);

    quint16 checkSum = calculateCRC(sender);

    sender.append((quint8)(checkSum & 0x00FF));
    sender.append((quint8)(checkSum >> 8));

    return sender;
}

QByteArray BLE::PacketPoll::data()
{
    QByteArray data;
    data
            .append(0xAA)
            .append(0xAA)
            .append(0xAA)
            .append(0x01)
            .append((quint8)TypeDevice::ALL);

    setSource(data);

    return data;
}

QByteArray BLE::PacketPollGun::data()
{
    QByteArray data;
    data
            .append(0xAA)
            .append(0xAA)
            .append(0xAA)
            .append(0x01)
            .append((quint8)TypeDevice::GUN);

    setSource(data);

    return data;
}

QByteArray BLE::PacketPollSensor::data()
{
    QByteArray data;
    data
            .append(0xAA)
            .append(0xAA)
            .append(0xAA)
            .append(0x01)
            .append(mType);

    setSource(data);

    return data;
}

QByteArray BLE::PacketPollSensorByID::data()
{
    QByteArray data;
    data
            .append(0xAA)
            .append(0xAA)
            .append(0xAA)
            .append(0x02)
            .append(mType)
            .append(mID);

    setSource(data);

    return data;
}

QByteArray BLE::PacketPollSensorByProcessor::data()
{
    QByteArray data;
    data
            .append(0xAA)
            .append(0xAA)
            .append(0xAA)
            .append(0x09)
            .append(mType)
            .append(mID >> 8*0)
            .append(mID >> 8*1)
            .append(mID >> 8*2)
            .append(mID >> 8*3)
            .append(mID >> 8*4)
            .append(mID >> 8*5)
            .append(mID >> 8*6)
            .append(mID >> 8*7);

    setSource(data);

    return data;
}

QByteArray BLE::PacketSearchSensors::data()
{
    QByteArray data;
    data
            .append(0xAA)
            .append(0xAA)
            .append(0xAA)
            .append(0x01)
            .append((quint8)TypeDevice::ALL_FROM_PROCESSOR_NAME);

    setSource(data);

    return data;
}

QByteArray BLE::PacketPollUSB::data()
{
    QByteArray data;
    data
            .append(0xBB)
            .append(0xBB)
            .append(0xBB)
            .append(0x01)
            .append((quint8)TypeDevice::ALL);

    setSource(data);

    return data;
}

QByteArray BLE::PacketPollBPD::data()
{
    QByteArray data;
    data
            .append(0xBB)
            .append(0xBB)
            .append(0xBB)
            .append(0x01)
            .append((quint8)TypeDevice::BPD);

    setSource(data);

    return data;
}

bool BLE::PacketSensorFlashPart0::parseData(const QByteArray &data)
{
    if(data.size()!=15) return false;
    if(data[BYTE_SIZE]!=0x0B) return false;
    if(quint8((data[BYTE_FIRST_DATA]&0xE0)>>5) != 0) return false;

    setSource(data);

    mKitNumber = (((data[BYTE_ID_KIT_HIGH] << 8) & 0xFF00) | (data[BYTE_ID_KIT_HIGH] & 0x00FF));
    mKitStatus = data[BYTE_KIT_STATUS];
    mSensor = data[BYTE_SENSOR_ID];
    mSensorState = data[BYTE_SENSOR_STATE];
    mBattery = data[BYTE_SENSOR_BATTERY];

    mAmmunition = quint8((data[BYTE_FIRST_DATA]&0x1F));
    mFighterID = ((quint16(data[BYTE_SECOND_DATA])<<3) | (quint16(data[BYTE_THIRD_DATA]&0xE0)>>5));
    mLaserNumber = quint8(data[BYTE_THIRD_DATA]&0x1F);

    mType = data[BYTE_TYPE];

    return true;
}

bool BLE::PacketSensorFlashPart1::parseData(const QByteArray &data)
{
    if(data.size()!=15) return false;
    if(data[BYTE_SIZE]!=0x0B) return false;
    if(quint8((data[BYTE_FIRST_DATA]&0xE0)>>5) != 1) return false;

    setSource(data);

    mKitNumber = (((data[BYTE_ID_KIT_HIGH] << 8) & 0xFF00) | (data[BYTE_ID_KIT_HIGH] & 0x00FF));
    mKitStatus = data[BYTE_KIT_STATUS];
    mSensor = data[BYTE_SENSOR_ID];
    mSensorState = data[BYTE_SENSOR_STATE];
    mBattery = data[BYTE_SENSOR_BATTERY];

    mX_Low = quint8((data[BYTE_FIRST_DATA]&0x1F));
    mShotNumber = ((quint16(data[BYTE_SECOND_DATA])<<3) | (quint16(data[BYTE_THIRD_DATA]&0xE0)>>5));
    mLaserNumber = quint8(data[BYTE_THIRD_DATA]&0x1F);

    mType = data[BYTE_TYPE];

    return true;
}

bool BLE::PacketSensorFlashPart2::parseData(const QByteArray &data)
{
    if(data.size()!=15) return false;
    if(data[BYTE_SIZE]!=0x0B) return false;
    if(quint8((data[BYTE_FIRST_DATA]&0xE0)>>5) != 2) return false;

    setSource(data);

    mKitNumber = (((data[BYTE_ID_KIT_HIGH] << 8) & 0xFF00) | (data[BYTE_ID_KIT_HIGH] & 0x00FF));
    mKitStatus = data[BYTE_KIT_STATUS];
    mSensor = data[BYTE_SENSOR_ID];
    mSensorState = data[BYTE_SENSOR_STATE];
    mBattery = data[BYTE_SENSOR_BATTERY];

    mY_Low  = quint8((data[BYTE_FIRST_DATA]&0x1F));
    mX = ((quint16(data[BYTE_SECOND_DATA])<<3) | (quint16(data[BYTE_THIRD_DATA]&0xE0)>>5));
    mLaserNumber = quint8(data[BYTE_THIRD_DATA]&0x1F);

    mType = data[BYTE_TYPE];

    return true;
}

bool BLE::PacketSensorFlashPart3::parseData(const QByteArray &data)
{
    if(data.size()!=15) return false;
    if(data[BYTE_SIZE]!=0x0B) return false;
    if(quint8((data[BYTE_FIRST_DATA]&0xE0)>>5) != 3) return false;

    setSource(data);

    mKitNumber = (((data[BYTE_ID_KIT_HIGH] << 8) & 0xFF00) | (data[BYTE_ID_KIT_HIGH] & 0x00FF));
    mKitStatus = data[BYTE_KIT_STATUS];
    mSensor = data[BYTE_SENSOR_ID];
    mSensorState = data[BYTE_SENSOR_STATE];
    mBattery = data[BYTE_SENSOR_BATTERY];

    mWeapon  = quint8((data[BYTE_FIRST_DATA]&0x1F));
    mY = ((quint16(data[BYTE_SECOND_DATA])<<3) | (quint16(data[BYTE_THIRD_DATA]&0xE0)>>5));
    mLaserNumber = quint8(data[BYTE_THIRD_DATA]&0x1F);

    mType = data[BYTE_TYPE];

    return true;
}

bool BLE::PacketSensorFlashPart4::parseData(const QByteArray &data)
{
    if(data.size()!=15) return false;
    if(data[BYTE_SIZE]!=0x0B) return false;
    if(quint8((data[BYTE_FIRST_DATA]&0xE0)>>5) != 4) return false;

    setSource(data);

    mKitNumber = (((data[BYTE_ID_KIT_HIGH] << 8) & 0xFF00) | (data[BYTE_ID_KIT_HIGH] & 0x00FF));
    mKitStatus = data[BYTE_KIT_STATUS];
    mSensor = data[BYTE_SENSOR_ID];
    mSensorState = data[BYTE_SENSOR_STATE];
    mBattery = data[BYTE_SENSOR_BATTERY];

    mShotNumber  = quint8((data[BYTE_FIRST_DATA]&0x1F));
    mFighterID = ((quint16(data[BYTE_SECOND_DATA])<<3) | (quint16(data[BYTE_THIRD_DATA]&0xE0)>>5));
    mLaserNumber = quint8(data[BYTE_THIRD_DATA]&0x1F);

    mType = data[BYTE_TYPE];

    return true;
}

QByteArray BLE::PacketSearchBPD::data()
{
    QByteArray data;
    data
            .append(0xBB)
            .append(0xBB)
            .append(0xBB)
            .append(0x04)
            .append((quint8)TypeDevice::BPD)
            .append((quint8)(mKitNumber & 0x00FF))
            .append((quint8)(mKitNumber  >> 8))
            .append((char(0)));

    setSource(data);

    return data;
}

QByteArray BLE::PacketBpdControl::data()
{
    QByteArray data;
    data
            .append(0xBB)
            .append(0xBB)
            .append(0xBB)
            .append(0x06)
            .append((quint8)TypeDevice::BPD)
            .append((quint8)(mKitNumber & 0x00FF))
            .append((quint8)(mKitNumber  >> 8))
            .append((quint8)(0x01))
            .append((quint8)(mNewKitNumber & 0x00FF))
            .append((quint8)(mNewKitNumber  >> 8));

    setSource(data);

    return data;
}

bool BLE::PacketBpdState::parseData(const QByteArray &data)
{
    if(data.size()!=9) return false;
    if(data[BYTE_SIZE]!=0x05) return false;
    if(data[BYTE_TYPE]!=(quint8)TypeDevice::BPD) return false;

    setSource(data);

    mKitNumber = (((data[BYTE_ID_KIT_HIGH] << 8) & 0xFF00) | (data[BYTE_ID_KIT_LOW] & 0x00FF));
    mStatus = data[BYTE_KIT_STATUS];

    return true;
}

void BLE::PacketBpdState::printInfo()
{
    qInfo() << "---! Текущее состояние БПД !---";
    qDebug() << "raw: " << source().toHex();

    qDebug() << "\tНомер комплекта: "<< kitNumber()
             << "\n\tСтатус устройства: " << status() << endl;
}

bool BLE::PacketSensorStateByProcessor::parseData(const QByteArray &data)
{
    if(data.size()!=20) return false;
    if(data[BYTE_SIZE]!=0x10) return false;

    setSource(data);

    mType = data[BYTE_TYPE];
    mKitNumber = (((data[BYTE_ID_KIT_HIGH] << 8) & 0xFF00) | (data[BYTE_ID_KIT_LOW] & 0x00FF));
    mKitStatus = data[BYTE_KIT_STATUS];
    mSensor = data[BYTE_SENSOR_ID];

    mProcessor = (((quint64)data[BYTE_SENSOR_PROCESSOR_NUMBER_LOW] << 56) & 0xFF00000000000000U)
            | (((quint64)data[BYTE_SENSOR_PROCESSOR_NUMBER_LOW + 1] << 48) & 0x00FF000000000000U)
            | (((quint64)data[BYTE_SENSOR_PROCESSOR_NUMBER_LOW + 2] << 40) & 0x0000FF0000000000U)
            | (((quint64)data[BYTE_SENSOR_PROCESSOR_NUMBER_LOW + 3] << 32) & 0x000000FF00000000U)
            | ((data[BYTE_SENSOR_PROCESSOR_NUMBER_LOW + 4] << 24) & 0x00000000FF000000U)
            | ((data[BYTE_SENSOR_PROCESSOR_NUMBER_LOW + 5] << 16) & 0x0000000000FF0000U)
            | ((data[BYTE_SENSOR_PROCESSOR_NUMBER_LOW + 6] <<  8) & 0x000000000000FF00U)
            | (data[BYTE_SENSOR_PROCESSOR_NUMBER_HIGH] & 0x00000000000000FFU);

    mSensorState = data[BYTE_SENSOR_STATE];
    mBattery = data[BYTE_SENSOR_BATTERY];

    return true;
}

void BLE::PacketSensorStateByProcessor::printInfo()
{
    qInfo() << "---! Датчик !---";
    qDebug() << "raw: " << source().toHex();

    qDebug() << "\tНомер комплекта: "<< kitNumber()
             << "\n\tСтатус комплекта: " << kitStatus()
             << "\n\tНомер процессора датчика: " << processorNumber()
             << "\n\tНомер датчика: " << sensor()
             << "\n\tТип датчика: " << type()
             << "\n\tСостояние датчика: " << sensorState()
             << "\n\tЗаряд аккумулятора: " << battery() << endl;
}

bool BLE::PacketSensorState::parseData(const QByteArray &data)
{
    if(data.size()!=12) return false;
    if(data[BYTE_SIZE]!=0x08) return false;

    setSource(data);

    mType = data[BYTE_TYPE];
    mKitNumber = (((data[BYTE_ID_KIT_HIGH] << 8) & 0xFF00) | (data[BYTE_ID_KIT_LOW] & 0x00FF));
    mKitStatus = data[BYTE_KIT_STATUS];
    mSensor = data[BYTE_SENSOR_ID];
    mSensorState = data[BYTE_SENSOR_STATE];
    mBattery = data[BYTE_SENSOR_BATTERY];

    return true;
}

void BLE::PacketSensorState::printInfo()
{
    qInfo() << "---! Датчик !---";
    qDebug() << "raw: " << source().toHex();

    qDebug() << "\tНомер комплекта: "<< kitNumber()
             << "\n\tСтатус комплекта: " << kitStatus()
             << "\n\tНомер датчика: " << sensor()
             << "\n\tТип датчика: " << type()
             << "\n\tСостояние датчика: " << sensorState()
             << "\n\tЗаряд аккумулятора: " << battery() << endl;
}

QByteArray BLE::PacketSensorControlStatus::data()
{
    QByteArray data;
    data
            .append(0xAA)
            .append(0xAA)
            .append(0xAA)
            .append(0x06)
            .append(mType)
            .append((quint8)(mKitNumber & 0x00FF))
            .append((quint8)(mKitNumber  >> 8))
            .append(mSensor)
            .append((quint8)(0x01))
            .append(mStatus);

    setSource(data);

    return data;
}

QByteArray BLE::PacketSensorControlKitNumber::data()
{
    QByteArray data;
    data
            .append(0xAA)
            .append(0xAA)
            .append(0xAA)
            .append(0x07)
            .append(mType)
            .append((quint8)(mKitNumber & 0x00FF))
            .append((quint8)(mKitNumber  >> 8))
            .append(mSensor)
            .append((quint8)(0x02))
            .append((quint8)(mNewKitNumber & 0x00FF))
            .append((quint8)(mNewKitNumber  >> 8));

    setSource(data);

    return data;
}

QByteArray BLE::PacketSensorControlId::data()
{
    QByteArray data;
    data
            .append(0xAA)
            .append(0xAA)
            .append(0xAA)
            .append(0x06)
            .append(mType)
            .append((quint8)(mKitNumber & 0x00FF))
            .append((quint8)(mKitNumber  >> 8))
            .append(mSensor)
            .append((quint8)(0x03))
            .append((quint8)(mNewSensorId));

    setSource(data);

    return data;
}

QByteArray BLE::PacketSensorControlIdByProcessor::data()
{
    QByteArray data;
    data
            .append(0xAA)
            .append(0xAA)
            .append(0xAA)
            .append(0x0D)
            .append(mType)
            .append((quint8)(mKitNumber & 0x00FF))
            .append((quint8)(mKitNumber  >> 8))
            .append((quint8)(mSensor >> 8*7))
            .append((quint8)(mSensor >> 8*6))
            .append((quint8)(mSensor >> 8*5))
            .append((quint8)(mSensor >> 8*4))
            .append((quint8)(mSensor >> 8*3))
            .append((quint8)(mSensor >> 8*2))
            .append((quint8)(mSensor>> 8*1))
            .append((quint8)(mSensor >> 8*0))
            .append((quint8)(0x03))
            .append(mNewSensorId);

    setSource(data);

    return data;
}

QByteArray BLE::PacketSensorControlKitStatus::data()
{
    QByteArray data;
    data
            .append(0xAA)
            .append(0xAA)
            .append(0xAA)
            .append(0x06)
            .append(mType)
            .append((quint8)(mKitNumber & 0x00FF))
            .append((quint8)(mKitNumber  >> 8))
            .append(mSensor)
            .append((char)(0))
            .append(mKitStatus);

    setSource(data);

    return data;
}

QByteArray BLE::PacketGunControlKitNumber::data()
{
    QByteArray data;
    data
            .append(0xAA)
            .append(0xAA)
            .append(0xAA)
            .append(0x06)
            .append(0x01)
            .append((quint8)(oldKit & 0x00FF))
            .append((quint8)(oldKit  >> 8))
            .append(0x04)
            .append((quint8)(nextKit & 0x00FF))
            .append((quint8)(nextKit  >> 8));

    setSource(data);

    return data;
}

QByteArray BLE::PacketGunControlKitStatus::data()
{
    QByteArray data;
    data
            .append(0xAA)
            .append(0xAA)
            .append(0xAA)
            .append(0x05)
            .append(0x01)
            .append((quint8)(mKitNumber & 0x00FF))
            .append((quint8)(mKitNumber  >> 8))
            .append(char(0))
            .append(mKitStatus);

    setSource(data);

    return data;
}

QByteArray BLE::PacketGunControlStatus::data()
{
    QByteArray data;
    data
            .append(0xAA)
            .append(0xAA)
            .append(0xAA)
            .append(0x05)
            .append(0x01)
            .append((quint8)(mKitNumber & 0x00FF))
            .append((quint8)(mKitNumber  >> 8))
            .append(0x01)
            .append(mStatus);

    setSource(data);

    return data;
}

QByteArray BLE::PacketGunControlShotNumber::data()
{
    QByteArray data;
    data
            .append(0xAA)
            .append(0xAA)
            .append(0xAA)
            .append(0x06)
            .append(0x01)
            .append((quint8)(mKitNumber & 0x00FF))
            .append((quint8)(mKitNumber  >> 8))
            .append(0x02)
            .append((quint8)(mShotNumber & 0x00FF))
            .append((quint8)(mShotNumber  >> 8));

    setSource(data);

    return data;
}

QByteArray BLE::PacketGunControlCoord::data()
{
    QByteArray data;
    data
            .append(0xAA)
            .append(0xAA)
            .append(0xAA)
            .append(0x08)
            .append(0x01)
            .append((quint8)(mKitNumber & 0x00FF))
            .append((quint8)(mKitNumber  >> 8))
            .append(0x03)
            .append((quint8)(mX & 0x00FF))
            .append((quint8)(mX  >> 8))
            .append((quint8)(mY & 0x00FF))
            .append((quint8)(mY  >> 8));

    setSource(data);

    return data;
}

QByteArray BLE::PacketBpdReboot::data()
{
    QByteArray data;
    data
            .append(0xBB)
            .append(0xBB)
            .append(0xBB)
            .append(0x04)
            .append((quint8)TypeDevice::BPD)
            .append((quint8)(mKitNumber & 0x00FF))
            .append((quint8)(mKitNumber  >> 8))
            .append(0x03);

    setSource(data);

    return data;
}

QByteArray BLE::PacketBpdControlKitStatus::data()
{
    QByteArray data;
    data
            .append(0xBB)
            .append(0xBB)
            .append(0xBB)
            .append(0x05)
            .append((quint8)TypeDevice::BPD)
            .append((quint8)(mKitNumber & 0x00FF))
            .append((quint8)(mKitNumber  >> 8))
            .append(0x02)
            .append(mKitStatus);

    setSource(data);

    return data;
}

QByteArray BLE::PacketKSUIControl::data()
{
    QByteArray data;
    data
            .append(0xAA)
            .append(0xAA)
            .append(0xAA)
            .append(0x05)
            .append(0x09)
            .append((quint8)(mKit & 0x00FF))
            .append((quint8)(mKit  >> 8))
            .append(mState)
            .append(char(0x00));

    setSource(data);

    return data;
}

bool BLE::PacketKsuiStateReq::parseData(const QByteArray &data)
{
    if(data.size() != 8) return false;
    if(data[BYTE_SIZE] != 0x04) return false;
    if(data[BYTE_TYPE] != (quint8)TypeDevice::PIB) return false;
    if(data[BYTE_ID_KIT_HIGH + 1] != 0x06) return false;

    mKitNumber = (((data[BYTE_ID_KIT_HIGH] << 8) & 0xFF00) | (data[BYTE_ID_KIT_LOW] & 0x00FF));

    setSource(data);

    return true;
}

void BLE::PacketKsuiStateReq::printInfo()
{
    qInfo() << "---! Запрос состояния вычислителя !---";
    qDebug() << "raw: " << source().toHex();

    qDebug() << "\tНомер комплекта: "<< kitNumber() << endl;
}

bool BLE::PacketKsuiState::parseData(const QByteArray &data)
{
    if(data.size() != 9) return false;
    if(data[BYTE_SIZE] != 0x05) return false;
    if(data[BYTE_TYPE] != (quint8)TypeDevice::KSUI) return false;

    mKitNumber = (((data[BYTE_ID_KIT_HIGH] << 8) & 0xFF00) | (data[BYTE_ID_KIT_LOW] & 0x00FF));

    setSource(data);

    mStatus = data[BYTE_KIT_STATUS];

    return true;
}

void BLE::PacketKsuiState::printInfo()
{
    qInfo() << "---! Ответ о состоянии вычислителя !---";
    qDebug() << "raw: " << source().toHex();

    qDebug() << "\tНомер комплекта: "<< kitNumber() << endl;
}

bool BLE::PacketPibState::parseData(const QByteArray &data)
{
    if(data.size()!=9) return false;
    if(data[BYTE_SIZE]!=0x05) return false;
    if(data[BYTE_TYPE]!=(quint8)TypeDevice::PIB) return false;
    if(data[BYTE_ID_KIT_HIGH + 1] != char(0x00)) return false;

    mKitNumber = (((data[BYTE_ID_KIT_HIGH] << 8) & 0xFF00) | (data[BYTE_ID_KIT_LOW] & 0x00FF));

    setSource(data);

    mStatus = data[BYTE_KIT_STATUS];

    return true;
}

void BLE::PacketPibState::printInfo()
{
    qInfo() << "---! Запрос от ПИБ состояния вычислителя !---";
    qDebug() << "raw: " << source().toHex();

    qDebug() << "\tНомер комплекта: "<< kitNumber() << endl;
}

QByteArray BLE::PacketSearchPIB::data()
{
    QByteArray data;
    data
            .append(0xBB)
            .append(0xBB)
            .append(0xBB)
            .append(0x04)
            .append((quint8)TypeDevice::PIB)
            .append((quint8)(mKitNumber & 0x00FF))
            .append((quint8)(mKitNumber  >> 8))
            .append((char(0)));

    setSource(data);

    return data;
}

QByteArray BLE::PacketPibControl::data()
{
    QByteArray data;
    data
            .append(0xBB)
            .append(0xBB)
            .append(0xBB)
            .append(0x06)
            .append((quint8)TypeDevice::PIB)
            .append((quint8)(mKitNumber & 0x00FF))
            .append((quint8)(mKitNumber  >> 8))
            .append((quint8)(0x01))
            .append((quint8)(mNewKitNumber & 0x00FF))
            .append((quint8)(mNewKitNumber  >> 8));

    setSource(data);

    return data;
}

QByteArray BLE::PacketPibControlKitStatus::data()
{
    QByteArray data;
    data
            .append(0xBB)
            .append(0xBB)
            .append(0xBB)
            .append(0x05)
            .append((quint8)TypeDevice::PIB)
            .append((quint8)(mKitNumber & 0x00FF))
            .append((quint8)(mKitNumber  >> 8))
            .append(0x02)
            .append(mKitStatus);

    setSource(data);

    return data;
}

QByteArray BLE::PacketPibReboot::data()
{
    QByteArray data;
    data
            .append(0xBB)
            .append(0xBB)
            .append(0xBB)
            .append(0x04)
            .append((quint8)TypeDevice::PIB)
            .append((quint8)(mKitNumber & 0x00FF))
            .append((quint8)(mKitNumber  >> 8))
            .append(0x03);

    setSource(data);

    return data;
}

QByteArray BLE::PacketPibImitGrenade::data()
{
    QByteArray data;
    data
            .append(0xBB)
            .append(0xBB)
            .append(0xBB)
            .append(0x04)
            .append((quint8)TypeDevice::PIB)
            .append((quint8)(mKitNumber & 0x00FF))
            .append((quint8)(mKitNumber  >> 8))
            .append(0x04);

    setSource(data);

    return data;
}

QByteArray BLE::PacketPibImitDesant::data()
{
    QByteArray data;
    data
            .append(0xBB)
            .append(0xBB)
            .append(0xBB)
            .append(0x04)
            .append((quint8)TypeDevice::PIB)
            .append((quint8)(mKitNumber & 0x00FF))
            .append((quint8)(mKitNumber  >> 8))
            .append(0x05);

    setSource(data);

    return data;
}

QByteArray BLE::PacketPibLoadBKCommand::data()
{
    QByteArray data;
    data
            .append(0xBB)
            .append(0xBB)
            .append(0xBB)
            .append(0x04)
            .append((quint8)TypeDevice::PIB)
            .append((quint8)(mKitNumber & 0x00FF))
            .append((quint8)(mKitNumber  >> 8))
            .append(0x0A);

    setSource(data);

    return data;
}

QByteArray BLE::PacketPibShotCommand::data()
{
    QByteArray data;
    data
            .append(0xBB)
            .append(0xBB)
            .append(0xBB)
            .append(0x04)
            .append((quint8)TypeDevice::PIB)
            .append((quint8)(mKitNumber & 0x00FF))
            .append((quint8)(mKitNumber  >> 8))
            .append(0x0B);

    setSource(data);

    return data;
}

QByteArray BLE::PacketPibKillCommand::data()
{
    QByteArray data;
    data
            .append(0xBB)
            .append(0xBB)
            .append(0xBB)
            .append(0x04)
            .append((quint8)TypeDevice::PIB)
            .append((quint8)(mKitNumber & 0x00FF))
            .append((quint8)(mKitNumber  >> 8))
            .append(0x0C);

    setSource(data);

    return data;
}

QByteArray BLE::PacketPibLiveCommand::data()
{
    QByteArray data;
    data
            .append(0xBB)
            .append(0xBB)
            .append(0xBB)
            .append(0x04)
            .append((quint8)TypeDevice::PIB)
            .append((quint8)(mKitNumber & 0x00FF))
            .append((quint8)(mKitNumber  >> 8))
            .append(0x0D);

    setSource(data);

    return data;
}

QByteArray BLE::PacketPibImitLIS::data()
{
    QByteArray data;
    data
            .append(0xBB)
            .append(0xBB)
            .append(0xBB)
            .append(0x04)
            .append((quint8)TypeDevice::PIB)
            .append((quint8)(mKitNumber & 0x00FF))
            .append((quint8)(mKitNumber  >> 8))
            .append(0x08);

    setSource(data);

    return data;
}

QByteArray BLE::PacketPibControlBPD::data()
{
    QByteArray data;
    data
            .append(0xBB)
            .append(0xBB)
            .append(0xBB)
            .append(0x04)
            .append((quint8)TypeDevice::PIB)
            .append((quint8)(mKitNumber & 0x00FF))
            .append((quint8)(mKitNumber  >> 8))
            .append(0x06);

    setSource(data);

    return data;
}

QByteArray BLE::PacketPibFindedDesant::data()
{
    QByteArray data;
    data
            .append(0xBB)
            .append(0xBB)
            .append(0xBB)
            .append(0x06)
            .append((quint8)TypeDevice::PIB)
            .append((quint8)(mKitNumber & 0x00FF))
            .append((quint8)(mKitNumber  >> 8))
            .append(0x09)
            .append((quint8)(mDesantNumber & 0x00FF))
            .append((quint8)(mDesantNumber  >> 8));

    setSource(data);

    return data;
}

bool BLE::PacketPibRequestDesantSensor::parseData(const QByteArray &data)
{
    if(data.size() != 14) return false;
    if(data[BYTE_SIZE] != 0x0A) return false;
    if(data[BYTE_TYPE] != 0x28) return false;

    mIdDesant = (((data[7] << 8) & 0xFF00) | (data[6] & 0x00FF));

    setSource(data);

    mIdDevice = data[8];

    return true;
}

bool BLE::PacketPibDevicesRequest::parseData(const QByteArray &data)
{
    if(data.size() != 5) return false;
    if(data[BYTE_SIZE] != 0x01) return false;
    if(data[BYTE_TYPE] != char(0xff)) return false;

    setSource(data);

    return true;
}

void BLE::PacketPibDevicesRequest::printInfo()
{
    qInfo() << "---! Запрос от БПД !---";
    qDebug() << "raw: " << source().toHex();
}

void BLE::PacketPibRequestDesantSensor::printInfo()
{
    qInfo() << "---! Запрос от ДД !---";
    qDebug() << "raw: " << source().toHex();
}

bool BLE::PacketPibReceiveWeaponKitState::parseData(const QByteArray &data)
{
    if(data.size() != 9) return false;
    if(data[BYTE_SIZE] != 0x05) return false;
    if(data[BYTE_TYPE] != 0x01) return false;
    if(data[7] != char(0x00)) return false;

    mKitNumber = (((data[BYTE_ID_KIT_HIGH] << 8) & 0xFF00) | (data[BYTE_ID_KIT_LOW] & 0x00FF));

    setSource(data);

    mStatus = data[8];

    return true;
}

void BLE::PacketPibReceiveWeaponKitState::printInfo()
{
    qInfo() << "---! Статус комплекта оружия !---";
    qDebug() << "raw: " << source().toHex();
}

bool BLE::PacketPibReceiveWeaponDeviceState::parseData(const QByteArray &data)
{
    if(data.size() != 9) return false;
    if(data[BYTE_SIZE] != 0x05) return false;
    if(data[BYTE_TYPE] != 0x01) return false;
    if(data[7] != 0x01) return false;

    mKitNumber = (((data[BYTE_ID_KIT_HIGH] << 8) & 0xFF00) | (data[BYTE_ID_KIT_LOW] & 0x00FF));

    setSource(data);

    mStatus = data[8];

    return true;
}

void BLE::PacketPibReceiveWeaponDeviceState::printInfo()
{
    qInfo() << "---! Статус устройства оружия !---";
    qDebug() << "raw: " << source().toHex();
}

bool BLE::PacketPibShotCommandRecv::parseData(const QByteArray &data)
{
    if(data.size() != 8) return false;
    if(data[BYTE_SIZE] != 0x04) return false;
    if(data[BYTE_TYPE] != (quint8)TypeDevice::PIB) return false;
    if(data[BYTE_ID_KIT_HIGH + 1] != 0x0b) return false;

    mKitNumber = (((data[BYTE_ID_KIT_HIGH] << 8) & 0xFF00) | (data[BYTE_ID_KIT_LOW] & 0x00FF));

    setSource(data);

    return true;
}

void BLE::PacketPibShotCommandRecv::printInfo()
{
    qInfo() << "---! Выстрел от ПИБ !---";
    qDebug() << "raw: " << source().toHex();
}

bool BLE::PacketPibLoadBKCommandRecv::parseData(const QByteArray &data)
{
    if(data.size() != 8) return false;
    if(data[BYTE_SIZE] != 0x04) return false;
    if(data[BYTE_TYPE] != (quint8)TypeDevice::PIB) return false;
    if(data[BYTE_ID_KIT_HIGH + 1] != 0x0a) return false;

    mKitNumber = (((data[BYTE_ID_KIT_HIGH] << 8) & 0xFF00) | (data[BYTE_ID_KIT_LOW] & 0x00FF));

    setSource(data);

    return true;
}

void BLE::PacketPibLoadBKCommandRecv::printInfo()
{
    qInfo() << "---! Загрузка БК от ПИБ !---";
    qDebug() << "raw: " << source().toHex();
}

bool BLE::PacketPibLiveCommandRecv::parseData(const QByteArray &data)
{
    if(data.size() != 8) return false;
    if(data[BYTE_SIZE] != 0x04) return false;
    if(data[BYTE_TYPE] != (quint8)TypeDevice::PIB) return false;
    if(data[BYTE_ID_KIT_HIGH + 1] != 0x0d) return false;

    mKitNumber = (((data[BYTE_ID_KIT_HIGH] << 8) & 0xFF00) | (data[BYTE_ID_KIT_LOW] & 0x00FF));

    setSource(data);

    return true;
}

void BLE::PacketPibLiveCommandRecv::printInfo()
{
    qInfo() << "---! Оживить от ПИБ !---";
    qDebug() << "raw: " << source().toHex();
}

bool BLE::PacketPibKillCommandRecv::parseData(const QByteArray &data)
{
    if(data.size() != 8) return false;
    if(data[BYTE_SIZE] != 0x04) return false;
    if(data[BYTE_TYPE] != (quint8)TypeDevice::PIB) return false;
    if(data[BYTE_ID_KIT_HIGH + 1] != 0x0c) return false;

    mKitNumber = (((data[BYTE_ID_KIT_HIGH] << 8) & 0xFF00) | (data[BYTE_ID_KIT_LOW] & 0x00FF));

    setSource(data);

    return true;
}

void BLE::PacketPibKillCommandRecv::printInfo()
{
    qInfo() << "---! Убить от ПИБ !---";
    qDebug() << "raw: " << source().toHex();
}

bool BLE::PacketRfFlashGrenadeRgd::parseData(const QByteArray &data)
{
    if(data.size() != 8) return false;
    if(data[BYTE_SIZE] != 0x04) return false;
    if(data[BYTE_TYPE] != (quint8)TypeDevice::GRENADE_RGD) return false;

    m_power = data[5];
    m_type = (quint8)TypeDevice::GRENADE_RGD;
    m_GrenadeNumber = (((data[7] << 8) & 0xFF00) | (data[6] & 0x00FF));

    setSource(data);

    return true;
}

void BLE::PacketRfFlashGrenadeRgd::printInfo()
{
    qInfo() << "---! Засветка от РГД по радио!---";
    qDebug() << "raw: " << source().toHex();
}

bool BLE::PacketRfFlashGrenadeF1::parseData(const QByteArray &data)
{
    if(data.size() != 8) return false;
    if(data[BYTE_SIZE] != 0x04) return false;
    if(data[BYTE_TYPE] != (quint8)TypeDevice::GRENADE_F1) return false;

    m_power = data[5];
    m_type = (quint8)TypeDevice::GRENADE_F1;
    m_GrenadeNumber = (((data[7] << 8) & 0xFF00) | (data[6] & 0x00FF));

    setSource(data);

    return true;
}

void BLE::PacketRfFlashGrenadeF1::printInfo()
{
    qInfo() << "---! Засветка от Ф1 по радио!---";
    qDebug() << "raw: " << source().toHex();
}

bool BLE::PacketIrFlashGrenade::parseData(const QByteArray &data)
{
    if(data.size() != 14) return false;
    if(data[BYTE_SIZE] != 0x0a) return false;

    m_channel = data[5];
    m_type = data[6];
    m_GrenadeNumber = (((data[8] << 8) & 0xFF00) | (data[7] & 0x00FF));

    setSource(data);

    return true;
}

void BLE::PacketIrFlashGrenade::printInfo()
{
    qInfo() << "---! Засветка от гранаты по ИК!---";
    qDebug() << "raw: " << source().toHex();
}
