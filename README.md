# ORBIT – ESP32 Rotary Angle Measurement System

## 1. Project Overview

This is an ESP32-based rotational angle measurement system that measures the angular displacement and direction of a rotating shaft using a rotary encoder.

The system combines embedded programming, sensor interfacing, Wi-Fi communication and a browser-based monitoring interface.

The ESP32 captures rotary encoder signals using GPIO interrupts and quadrature decoding. It processes the signals to calculate angular position, encoder count, total revolutions and rotational direction.

The measured data is transmitted through an HTTP API over Wi-Fi and visualized through a web interface called **ORBIT – Rotational Measurement Studio**.

## 2. System Architecture

The project consists of two major components:

### A. ESP32 Embedded System
- Reads rotary encoder signals through GPIO pins.
- Uses interrupt-based quadrature decoding.
- Calculates angular position and revolution count.
- Detects clockwise and anticlockwise rotation.
- Creates a Wi-Fi access point.
- Hosts an HTTP server to provide measurement data in JSON format.

### B. Web Interface
- Connects to the ESP32 through its HTTP API.
- Displays real-time angular measurements.
- Visualizes rotational position using a circular gauge.
- Tracks measurement time using a stopwatch.
- Records important measurement points using flags.
- Exports recorded measurements as CSV files.

## 3. Features

### Embedded System
- Quadrature rotary encoder decoding.
- Interrupt-based pulse detection.
- Real-time angle calculation.
- Clockwise and anticlockwise direction detection.
- Revolution counting.
- ESP32 Wi-Fi access point.
- HTTP API with JSON responses.
- Zero-position reset API.

### Web Interface

**Live Measurement Dashboard**
- Circular gauge with a rotating needle.
- Real-time angle display (0°–360°).
- Rotational direction indicator.
- Encoder pulse count.
- Total revolution count.
- Cumulative angular displacement.

**Recent Angle Trace**
- Graphical representation of recent angular readings.
- Displays the history of up to 70 recent samples.

**Run Stopwatch**
- Start, pause, resume and reset controls.
- Records elapsed measurement time.
- Supports keyboard spacebar control for starting and pausing.

**Measurement Flags**
- Save important angular positions during a measurement run.
- Each flag records:
  - Stopwatch time.
  - Angular position.
  - Total angular displacement.
  - Encoder count.
  - Rotational direction.
- View saved measurements in a list.

**Connection and Calibration**
- Configurable ESP32 API address.
- Automatic data polling every 250 milliseconds.
- Connection status indicator.
- Two angle calculation modes:
  - Angle reported directly by ESP32.
  - Angle calculated from encoder count.
- Adjustable counts-per-revolution calibration.
- Saves connection and calibration preferences in browser local storage.

**Responsive Interface**
- Designed for desktop and mobile screens.
- Provides a clean measurement dashboard.

## 4. Hardware Requirements

| Component | Description |
|---|---|
| ESP32 Dev Board | Main microcontroller |
| Rotary Encoder | Angular position measurement |
| Jumper Wires | Circuit connections |
| USB Cable | Programming and power supply |

## 5. Circuit Connections

| Rotary Encoder | ESP32 |
|---|---|
| Channel A | GPIO 25 |
| Channel B | GPIO 26 |
| VCC | 3.3V |
| GND | GND |

The ESP32 uses internal pull-up resistors for the encoder signal pins.

## 6. Software Requirements

- Arduino IDE
- ESP32 Board Package
- Web browser (Chrome, Edge, Firefox, etc.)
- VS Code with Live Server extension (recommended for hosting the web interface locally)

### Libraries Used

ESP32 firmware:
- `WiFi.h`
- `WebServer.h`
- `math.h`

Web interface:
- HTML5
- CSS3
- JavaScript
- SVG for graphical visualizations
- Fetch API for HTTP communication
- Local Storage for saving interface settings

## 7. Working Principle

### Step 1: Encoder Signal Acquisition

The rotary encoder generates two digital output signals, A and B, with a phase difference.

The ESP32 monitors both signals using GPIO interrupts triggered on CHANGE events.

### Step 2: Quadrature Decoding

The ESP32 reads the states of both channels and uses a transition lookup table to determine the direction and count of each valid transition.

This allows the system to track both rotational direction and encoder position.

### Step 3: Angle Calculation

The firmware calculates the angular position using:

\[
\text{Revolutions}=\frac{\text{Encoder Count}}{\text{Counts Per Revolution}}
\]

\[
\text{Angle}=\text{Revolutions}\times360^\circ
\]

The displayed angular position is normalized to 0°–360°, while cumulative angular displacement is retained separately.

The current firmware uses 80 counts per revolution as an initial calibration estimate.

### Step 4: Wi-Fi Communication

The ESP32 creates its own Wi-Fi access point and runs an HTTP server.

The web interface sends HTTP requests to retrieve the measurement data.

### Step 5: Web-Based Visualization

JavaScript periodically fetches the ESP32 measurement data and updates the dashboard.

The interface displays the angle, encoder count, revolutions, direction and recent angle trace.

Users can also operate the stopwatch, record measurement flags and export the recorded data.

## 8. Wi-Fi Configuration

The ESP32 creates an independent Wi-Fi access point.

| Parameter | Value |
|---|---|
| Network Name (SSID) | ESP32-Angle |
| Password | 12345678 |
| IP Address | 192.168.4.1 |
| HTTP Port | 80 |

Connect your computer or mobile device to this Wi-Fi network before accessing the measurement system.

**Note:** Change the default Wi-Fi password before deploying the system in a real environment.

Request:

`http://192.168.4.1/data`

Response:

```json
{
  "angle": 125.50,
  "count": 28,
  "revolutions": 0.3500,
  "direction": "CLOCKWISE"
}
```

The `/data` endpoint also supports browser CORS requests.

## 9. How to Run the Project

### Part A: Upload ESP32 Firmware

1. Install Arduino IDE.
2. Install the ESP32 board package.
3. Connect the rotary encoder to the ESP32 according to the circuit connections.
4. Open `Angle_Rotation.ino`.
5. Select the appropriate ESP32 board and COM port.
6. Upload the program.
7. Open Serial Monitor at 115200 baud.
8. Verify that the ESP32 access point is created.

### Part B: Launch the Web Interface

1. Open `Web_Interface.html` in VS Code.
2. Install the Live Server extension if it is not already installed.
3. Right-click the HTML file and select **Open with Live Server**.
4. Connect your computer to the ESP32 Wi-Fi network:
   - SSID: `ESP32-Angle`
   - Password: `12345678`
5. In the web interface, enter the ESP32 API address:

   `http://192.168.4.1`

6. Click **Connect**.
7. Once connected, the dashboard starts displaying measurement data.

### Part C: Perform Measurements

1. Rotate the encoder and observe the live angle gauge.
2. Monitor the rotational direction and encoder count.
3. Start the stopwatch before beginning a measurement run.
4. Press **Flag** to record important angular positions.
5. Pause or resume the stopwatch whenever required.
6. Export the recorded flags using the **Export CSV** button.

## 10. Calibration

Accurate angle measurement depends on the counts-per-revolution value.

The firmware currently uses:

`COUNTS_PER_REVOLUTION = 80.0`

This is an initial estimate for a 20-detent encoder using four quadrature transitions per detent.

For better accuracy:

1. Rotate the shaft through exactly one complete revolution.
2. Record the corresponding encoder count change.
3. Update the firmware calibration value.

The web interface also provides an alternative calculation mode using the encoder count and a configurable counts-per-revolution value.

## 11. Demonstration

Refer the Video  attached in the repository for demonstartion.

## 12. Applications

- Angular displacement measurement.
- Rotary position monitoring.
- Embedded measurement systems.
- Robotics and motion-control prototypes.
- Industrial automation experiments.
- Real-time sensor monitoring.

## 13. Future Improvements

- Data logging with persistent storage.
- Advanced graphical analysis of rotational movement.
- Improved encoder calibration.
- Measurement history and session management.
- Integration with additional sensors.
- Wireless monitoring through external networks.


## Authors

- Santhosh P

- Pravin J

- Sanjeev S C