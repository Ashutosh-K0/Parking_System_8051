# 🚗 Automatic Parking System using 8051 Microcontroller

This project demonstrates an **Automatic Parking System** implemented using the **8051 microcontroller**. It features motor-controlled gates for vehicle entry and exit, LCD display for system status, and switches for simulating vehicle detection.

---

## 📌 Features

- 🔄 Automatic Gate Control (Entry and Exit)
- 📟 LCD Display for Status Messages
- 🔘 Switch-based Car Detection
- ⚙️ DC Motors for Gate Movement
- 💡 Simple and Cost-effective Design

---

## 🛠️ Components Used

- AT89C51 / 8051 Microcontroller
- 16x2 LCD Display
- 2 x DC Motors (Gate Control)
- Motor Driver IC (L293D or equivalent)
- Push Button Switches (for Entry & Exit)
- Resistors, Capacitors, Power Supply, Breadboard, Wires

---

## 🔧 Working Principle

1. **Vehicle Entry:**
   - Press the "Entry" switch to simulate a car arriving.
   - The entry gate opens automatically using a motor.
   - LCD displays: `Slots Available`.
   - Gate closes after a short delay.

2. **Vehicle Exit:**
   - Press the "Exit" switch to simulate a car leaving.
   - The exit gate opens.
   - LCD displays: `Slots Left`.
   - Gate closes automatically.


## 🚀 How to Run

1. Burn the hex code to 8051 microcontroller using a suitable programmer.
2. Connect the circuit as per the diagram.
3. Power up the board.
4. Press the Entry/Exit switches to simulate car movement.

---

## 🧠 Future Improvements

- Add IR sensors for vehicle detection instead of manual switches.
- Implement a parking slot counter.
- Connect to IoT cloud for remote monitoring.
- Add buzzer alerts or LED indicators.

---

## 📜 License

This project is open-source and available under the [MIT License](LICENSE).

---

## 🙌 Acknowledgements

Special thanks to open-source communities and microcontroller tutorials that inspired this project.

---
