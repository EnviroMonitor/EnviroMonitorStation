# Elements pinout list

## Wemos D1 mini

| Wemos PIN | Function            | Remarks                   |
|-----------|---------------------|---------------------------|
| 5V        | 5V supply           |                           |
| G         | Ground (-)          |                           |
| D4        | Heater on           | Connected to MOSFET gate  |
| D3        | DS18B20 data        |                           |
| D2        | I2C SCL line        |                           |
| D1        | I2C SDA line        |                           |
| RX        | Not used            |                           |
| TX        | Not used            |                           |
| 3V3       | 3V supply           |                           |
| D8        | PMS RST pin         |                           |
| D7        | PMS TXD pin         |                           |
| D6        | PMS RXD pin         |                           |
| D5        | PMS SET pin         |                           |
| D0        | DHTxx data pin      |                           |
| A0        | Not used            |                           |
| RST       | Not used            |                           |

## PMS3003 pinout

### Connector pins

```
----|
| *   - NC
| *   - NC
| *   - RST
| *   - TXD
| *   - RXD
| *   - SET
| *   - GND
| *   - 5VCC
----|
```

### Pinout description

| PMS PIN   | Color               | Function                  |
|-----------|---------------------|---------------------------|
| NC        | red                 | Not used                  |
| NC        | black               | Not used                  |
| RST       | yelow               | PMS reset (5V)            |
| TXD       | green               | Serial TX                 |
| RXD       | blue                | Serial RX                 |
| SET       | white               | PMS set (5V)              |
| GND       | orange              | Ground                    |
| 5VCC      | violet              | 5V supply                 |

