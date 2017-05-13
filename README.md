# soil2ifttt-esp32

esp32 program to read an analog pin and post it to an [ifttt](www.ifttt.com) makers service endpoint. My use case is to periodically tweet the value of a soil moisture sensor. I'm using an [esp32 thing](https://learn.sparkfun.com/tutorials/esp32-thing-hookup-guide) and an YL-69 moisture sensor.

__NB!__ It matters a great deal which ADC pin you use for analogRead() because [including WiFi.h apparently disables all ADC2_* channels](https://github.com/espressif/arduino-esp32/issues/102)!


If you platformio installed, you should just be able to run `platformio run --target upload` in the project root to upload it to your esp32 board.
