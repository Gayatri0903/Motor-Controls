## Motors Controls

# v0.1
 * The MicroPython script uses Modbus ASCII communication over UART to operate an RMCS-2303 motor driver.
 * Before communication begins, buffers are cleaned and UART is set up at 9600 baud with certain TX/RX pins.
 * It specifies important characteristics like read interval, speed, acceleration, slave ID and pulses per rotation.
 * The LRC checksum, which is necessary for Modbus ASCII message validation is computed using a function.
 * For register access, another function creates Modbus ASCII read (Function 03) and write (Function 06) instructions.
 * To ensure proper motor direction, the program initially disables CCW rotation before turning on CW rotation.
 * It writes to the appropriate control registers to set the motor's acceleration and speed.
 * The encoder position (LSB + MSB) is continuously read by the main loop from the motor driver.
 * Encoder resolution and time interval are used to translate position changes into rotations and RPM.
 * The user can safely stop the motor at any time by pressing "s" thanks to a non-blocking keyboard check.