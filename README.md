# ESP32 WiFi Radio 🎵

An ESP32-based internet radio project that streams audio from online radio stations and displays information on a TFT screen. The project also includes touch functionality and volume control.

## 🚀 Features

- **Internet Radio Streaming**: Play audio streams from online radio stations.
- **TFT Display**: Display station names, song titles, and other information.
- **Touch Controls**: Adjust volume and navigate through the interface using touch.
- **WiFi Manager**: Easily configure WiFi credentials via a web portal.

## 🛠️ Prerequisites

Before running this project, ensure you have the following installed and configured:

### 1. **Partition Scheme**

- Use the **Huge App (3MB No OTA/1MB SPIFFS)** partition scheme in the Arduino IDE.
- ![Partition Scheme](https://github.com/user-attachments/assets/d275e473-3e01-4ba7-8bc0-8f9d7387a412)

### 2. **Required Libraries**

Install the following libraries via the Arduino Library Manager or their respective GitHub repositories:

- **[ESP32-audioI2S (v3.0.8)](https://github.com/schreibfaul1/ESP32-audioI2S/tree/3.0.8)**  
  Ensure you use version 3.0.8 for compatibility with the ESP32.

- **[WiFiManager](https://github.com/tzapu/WiFiManager)**  
  Simplifies WiFi configuration by providing a web portal.

- **[TFT_eSPI](https://github.com/Bodmer/TFT_eSPI)**  
  For handling the TFT display.

### 3. **Secrets.h**

- Create a `Secrets.h` file based on `Secrets.example`.
- Add your WiFi credentials and other configuration details.

## 🔧 Hardware Setup

### Required Components:

- **ESP32 Dev Module**
- **TFT Display** (compatible with the `TFT_eSPI` library)
- **I2S DAC** (for audio output)

### Pin Connections:

| **Component**         | **ESP32 Pin** |
| --------------------- | ------------- |
| **I2S LCK**           | GPIO 25       |
| **I2S DIN**           | GPIO 27       |
| **TFT Display CS**    | GPIO 5        |
| **TFT Display RESET** | GPIO 17       |
| **TFT Display DC**    | GPIO 16       |
| **TFT Display MOSI**  | GPIO 23       |
| **TFT Display SCK**   | GPIO 18       |
| **TFT Display T_CLK** | GPIO 18       |
| **TFT Display T_CS**  | GPIO 4        |
| **TFT Display T_DIN** | GPIO 23       |
| **TFT Display T_OUT** | GPIO 19       |
| **Touch IRQ Pin**     | GPIO 35       |

### 📸 Hardware Setup Example:

Below is an example of how the hardware should be connected:

![Hardware Setup Example](https://github.com/blackspyek/ESP32-WifiRadio/blob/main/circuit_image.png)

### 📸 Working Radio Example

Below are pictures showcasing the fully functional ESP32 WiFi Radio project:

![Working Radio Example - Not Connected to Wifi View](https://github.com/blackspyek/ESP32-WifiRadio/blob/main/NotConnected.jpg)

![Working Radio Example - Connected to Wifi View](https://github.com/blackspyek/ESP32-WifiRadio/blob/main/Connected.jpg)

![Working Radio Example - Choosing Radio Station View](https://github.com/blackspyek/ESP32-WifiRadio/blob/main/RadioStations.jpg)

