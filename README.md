# KellerModbus
A library to use an Arduino as a master to control and communicate via modbus with the pressure and water level sensors produced by Keller International & [Keller America](https://www.kelleramerica.com).

This library depends on the [EnviroDIY/SensorModbusMaster](https://github.com/EnviroDIY/SensorModbusMaster) library.
The [EnviroDIY/ModularSensor](https://github.com/EnviroDIY/ModularSensors) library functions for Keller depend on this library.

The library has been tested to work with a Keller Nanolevel and a Keller Acculevel, which is a Keller Series 30, Class 5, Group 20 sensor water level transmitter with a software/firmware version of 5.20-12.28 (i.e. made after the 2012 in the 28th week). The library should also work with all Keller sensors of the same Series/Class/Group with the same or a later software version.

## <a name="functions"></a>Functions

This library only implements the following small subset of functions from those described in the [Keller Series 30 Communication Protocol Manual](https://www.kelleramerica.com/manuals-and-software/manuals/series30%20comm_protocol_e.pdf). These are:
* `getSerialNumber`
* `getValues`, for presssure and temperature

This library also calculates water depth via the following function:
* `calcWaterDepthM`, based on water pressure and temperature-dependency on water density, following equation 6 from [JonesHarris1992-NIST-DensityWater.pdf](https://github.com/EnviroDIY/KellerModbus/blob/master/doc/JonesHarris1992-NIST-DensityWater.pdf).

## <a name="license"></a>License
Software sketches and code are released under the BSD 3-Clause License -- See [LICENSE.md](https://github.com/EnviroDIY/ModularSensors/blob/master/LICENSE.md) file for details.

Documentation is licensed as [Creative Commons Attribution-ShareAlike 4.0](https://creativecommons.org/licenses/by-sa/4.0/) (CC-BY-SA) copyright.

## <a name="acknowledgments"></a>Acknowledgments
[EnviroDIY](http://envirodiy.org/)™ is presented by the Stroud Water Research Center, with contributions from a community of enthusiasts sharing do-it-yourself ideas for environmental science and monitoring.

[Anthony Aufdenakmpe](https://github.com/aufdenkampe) is the primary developer of this library.
We are grateful for the helpful technical assistance from the staff at [Keller America](https://www.kelleramerica.com).

This library has benefited from the support from the following funders:

* [Winona State University](https://www.winona.edu), [Southeastern Minnesota Water Resources Center](https://www.winona.edu/geoscience/waterresource.asp)
