# Simple firmware for INA219 measurement unit with Arduino and LCD

- INA219 breakout, i2c
- 16x2 LCD, i2c
- 2 buttons
- controlled by attiny85

Uses PlatformIO as build platform.

## Notes:

- Uses TinyWire as i2c implementation. There are modified libraries for INA219 and LCD communicatin included in the repository since they had to be modified in order to work with TinyWire.
- Attiny85 should be using 16Mhz internal oscillator
-More informatio [can be found in a blog post](https://josef-adamcik.cz/electronics/learning-the-hard-way-simple-diy-power-masurement-unit-with-ina219.html), [schematic is available on EasyEDA](https://easyeda.com/josefadamcik/ina219measurementunit).

## License

MIT, see [LICENSE](LICENSE).
This repository also contains modified copies and libraries (see lib directory) which have their own licences.
