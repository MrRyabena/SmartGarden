# SmartGarden

Система автоматизации плантации, теплицы, зимнего сада. Проект рассчитан на использование для автоматизации как полноразмерного производства, так и для садоводов-любителей. Система датчиков и агрегатов легко масштабируется без изменений логики работы системы.

## Функционал

- Контроль влажности почвы, автополив
- Контроль светового дня, досвечивание растений
- Контроль температуры и влажности воздуха
- Светодиодная индикация
- Управление сенсорной кнопкой и через графический интерфейс (вебсайт или приложение)
- Интеграция в модульную систему умного дома, возможность подключить дополнительные устройства (датчики, насосы, вентиляторы, нагреватели)

## Макет

Макет состоит из напечатанного на 3D-принтере корпуса. Внутри корпуса расположены:

- резервуар с водой;
- помпа для подачи воды;
- блок управляющей электроники;
- датчик температуры и влажности воздуха;
- сенсорная кнопка;
- светодиодный индикатор;

Корпус представляет собой подставку-оболочку для горшка с комнатным растением. Снаружи корпуса расположены:

- лампа и датчик света, для досвечивания растения;
- датчик влажности почвы и трубка автополива;

### Индикация и управление

В интерфейсе приложения можно настроить время светового дня, периоды включения подсветки и количество воды для полива.

- Статичная цветная индикация на устройстве показывает влажность почвы:
  - `Blue` -- растение полито
  - `Green` -- умеренная влажность
  - `Yellow`-`Red` -- пересыхание почвы, требуется полив
- Динамическое мигание индикатора:
  - `Red & Blue` -- если в системе автополива закончилась вода

Сенсорная кнопка:

- Долгое удержание -- полив
- Короткое нажатие -- включить/выключить лампу

## Компоненты

|Name|Type|Datasheet|Size|
|--|--|--|--|
|ESP8266|microcontroller|[Datasheet (Espressif)](https://www.espressif.com/sites/default/files/documentation/0a-esp8266ex_datasheet_en.pdf)|25x50x15|
|Soil moisture sensor v2.0|capacitive soil moisture sensor|[Datasheet](https://wiki.dfrobot.com/Capacitive_Soil_Moisture_Sensor_SKU_SEN0193)|23x100x1.5|
|DHT22|temperature and humidity sensor|[DHT22 (AM2302) Datasheet](https://www.sparkfun.com/datasheets/Sensors/Temperature/DHT22.pdf)|16x6x30|
|GL5528|photo-resistance (lightness sensor)|[GL5528 Datasheet](https://passionelectronique.fr/wp-content/uploads/datasheet-photoresistance-LDR-GL5528-CdS.pdf?pseSrc=pgPhotoresistance)||
|TTP223|capacitive touch sensor IC|[TTP223 Datasheet](https://static.chipdip.ru/lib/928/DOC012928792.pdf)|12x14x2|
|WS2812b|ARGB LED for indication|[Datasheet (Worldsemi)](https://cdn-shop.adafruit.com/datasheets/WS2812B.pdf)|d32x3|
|Water Pump|5V water pump||110x35x35|
|USB type C|power input||15x12x5|
|Power Station 5V 1A||||
