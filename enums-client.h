/****************************************************************************************
*
*				ОА НПО 'РусБиТех'
*
            (с) Copyright 2019, Rusbitech, Moscow
*
*  Все права защищены. Данная программа не предназначена для публикации и копирования.
*  Программа состоит из конфиденциальных, содержащих коммерческую тайну ОА НПО 'РусБиТех'
*  материалов. Любые попытки или участие в изменении кода программы строго ЗАПРЕЩЕНЫ.
*
****************************************************************************************/
/**
 @file	   enums-client.h
 @author   Ухваркин Сергей <s.ukhvarkin@rusbitech.ru>
 @date     01.11.2019
 @version  1.0
 @brief    Реализация и описание класса %{Cpp:License:ClassName}
**/

#ifndef ENUMS_CLIENT_H
#define ENUMS_CLIENT_H

enum class KitState : unsigned char
{
    ALIVE   = 0x00,
    KILL    = 0x01
};

enum class DeviceMode : unsigned char
{
    ALLRIGHT    = 0x00,
    EMPTY       = 0x01,
    SERVICE     = 0x02,
    SHOT        = 0x03
};

enum class MessageCommandClient : unsigned char
{
    COMMAND_GUN_SHOT        = 0x01,     /* Событие выстрела */
    COMMAND_SENSOR          = 0x02,     /* Данные от бойца на сервер при сигнале от датчиков  */
    COMMAND_STATUS          = 0x03,     /* Данные о состоянии бойца  */
    COMMAND_CENTER          = 0x04,     /* Координаты центра полигона  */
    COMMAND_KILL_USER       = 0x05,     /* Убить конкретного бойца */
    COMMAND_POSITION        = 0x06,     /* Географические координаты бойца */
    COMMAND_RELOAD_ALL      = 0x07,     /* Оживить всех *deprecated  future replace others*/
    COMMAND_RELOAD_USER     = 0x08,     /* Оживить конкретного бойца */
    COMMAND_MISS            = 0x09,     /* Промах */
    COMMAND_CONFIRMATION    = 0x0B,     /* Подтверждение пакета */
    COMMAND_AMMUNITION      = 0x0A,     /* Пакет с боекомплектом */
    COMMAND_DEVICES         = 0x0C,     /* Получить информацию о подключенных устройствах (poll devices)*/
    COMMAND_CONF_CHANGE     = 0x0D,     /* Назначить конфугурацию и  адрес устройству*/
    COMMAND_CONFIGURATION   = 0x0E,     /* Назначить конфигурацию устройству */
    COMMAND_BLE             = 0xAA      /* Данные от BLE */
};

#endif // ENUMS_CLIENT_H
