# ESP32 Ozone Sensor (I2C) using ESP-IDF

🚀 This project demonstrates how to use an **ozone gas sensor** with **Espressif ESP32 series** chips using **ESP-IDF**. The sensor provides **ozone (O₃) concentration in PPB (Parts Per Billion)**. This example initializes the sensor, reads data periodically, and outputs it via UART.

---

## Supported Targets

| ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C6 | ESP32-H2 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----- | -------- | -------- | -------- | -------- | -------- | -------- | -------- |
| ✅     | ⚠️       | ✅        | ⚠️       | ⚠️       | ⚠️       | ✅        | ✅        |

> **Note:** This example is primarily tested on **ESP32**. Other targets may require hardware configuration adjustments.

---

## Features

- **I2C-based communication** with the ozone sensor
- **Supports automatic & passive measurement modes**
- **Periodic ozone concentration reading in PPB**
- **UART-based output** for easy monitoring
- **Moving average filter** for data smoothing

---

## Hardware Requirements

- **ESP32 development board** (e.g., ESP32-WROOM-32)
- **Ozone Gas Sensor (I2C)**
- **Breadboard and jumper wires**
- **USB cable** for power and programming

### Wiring Guide

| Ozone Sensor Pin | ESP32 Pin  | Description             |
| --------------- | ---------- | ----------------------- |
| **VCC**        | **3.3V**   | Power supply (3.3V)     |
| **GND**        | **GND**    | Ground                  |
| **SDA**        | **GPIO21** | I2C Data                |
| **SCL**        | **GPIO22** | I2C Clock               |

> **Note:** Ensure proper pull-up resistors (4.7kΩ recommended) are present on the I2C lines.

---

## Getting Started

### Prerequisites

- **ESP-IDF v4.4 or newer** installed
- **Basic understanding** of ESP-IDF project structure
- **Hardware setup** as per the wiring guide

### Installation

#### Clone the Repository

```bash
git clone https://github.com/MouNir9944/ESP32_Ozone_Sensor_IDF.git
cd ESP32_Ozone_Sensor_IDF
```

#### Set Target Chip (Replace `esp32` with your target if different)

```bash
idf.py set-target esp32
```

#### Configure Project (Optional)

```bash
idf.py menuconfig
```

> Adjust **I2C pin configuration** under *Example Configuration* if needed.

#### Build and Flash

```bash
idf.py build flash monitor
```

---

## Observe Output

After flashing, the ESP32 will initialize the sensor and start printing measurements:

```plaintext
I (324) Ozone Sensor: Initialized successfully
I (334) Ozone Sensor: Ozone concentration is 48 PPB
I (4334) Ozone Sensor: Ozone concentration is 52 PPB
...
```

---

## Configuration Options

### I2C Settings

Modify `main/main.c` to change default **I2C settings**:

```c
#define I2C_MASTER_SDA_IO         21      /* GPIO number for SDA */
#define I2C_MASTER_SCL_IO         22      /* GPIO number for SCL */
#define I2C_MASTER_FREQ_HZ        100000  /* I2C master clock frequency */
```

### Measurement Mode

Modify this line in `app_main()`:

```c
setModes(SEN0321_handle, MEASURE_MODE_PASSIVE);  // or MEASURE_MODE_AUTOMATIC
```

### Measurement Interval

Adjust the sampling interval by changing **`SAMPLE_DELAY_MS`** in `main/main.c`:

```c
#define SAMPLE_DELAY_MS 1000  /* 1-second interval */
```

---

## Troubleshooting

### Sensor Not Detected

✅ Verify **wiring connections**  
✅ Check **I2C address** with `i2c_scanner` example  
✅ Ensure **correct power supply** (3.3V)  

### Inconsistent Readings

✅ Allow **24-48 hours** for sensor stabilization  
✅ Ensure **adequate ventilation** around the sensor  
✅ Avoid **direct exposure** to high ozone sources  

### I2C Errors

✅ Confirm **pull-up resistors** are present (**4.7kΩ** recommended)  
✅ Reduce **I2C clock speed** if using long wires  

---

## Contributing

Contributions are welcome! Please open an **issue** or submit a **pull request** for:

- **Bug fixes**
- **Additional features**
- **Documentation improvements**

---

## License

This project is licensed under the **MIT License** - see [LICENSE](LICENSE) for details.

---

## Acknowledgements

- **Sensirion** for inspiration from their **SCD30 CO₂ sensor**
- **Espressif Systems** for the **ESP-IDF framework**

