## Motors Controls
# Component Selection
    1.Controller RP2040-Zero 
    2.RHINO 60-RPM 40KgCm DC Planetary Geared High Precision Encoder Servo Motor
    3.Motor Driver RMCS-2303

# Reasons Of Selection
    1.RP2040-Zero:
            The controller, RP2040-Zero, serves as the system's brain. For real-time monitoring and control, it reads encoder feedback via Modbus (UART) and transmits control directives (speed, direction, and acceleration) to the motor driver.

    2.RHINO 60-RPM 40KgCm DC Planetary Geared High Precision Encoder Servo Motor:
            Uses an integrated encoder to provide extremely precise position feedback while producing strong torque at low speeds. The planetary gearbox is perfect for robotics, automation, and positioning applications because it guarantees precise and smooth motion.    

    3.Motor Driver RMCS-2303:
            Interfaces that connect the motor to the controller.  In addition to providing the necessary power to the motor, it communicates with the RP2040 via Modbus ASCII and manages closed-loop control, encoder processing, direction control, acceleration, and protection.   

# Specifications Of the Component
    1.RP2040-Zero:
        * Dual-core ARM Cortex M0+ processor, flexible clock running up to 133 MHz
        * Built-in 264KB SRAM and 2MB Flash           
        * USB1.1 host and device support
        * Low-power sleep and dormant modes
        * Drag-and-drop programming using mass storage over USB
        * 29 GPIO pins of RP2040

    2.RHINO 60-RPM 40KgCm DC Planetary Geared High Precision Encoder Servo Motor:    
        * Operating Voltage : 12V DC
        * Motor Speed at Output Shaft(RPM) : 66RPM
        * Stall Torque(Kgcm) : 100Kgcm but not exceed greater than 75 Kgcm
        * Rated Torque(Kgcm) : 40Kgcm 
        * Gear Ratio : 1:270
        * CPR at output Shaft Of Motor : 3,60,720

    3.Motor Driver RMCS-2303:
        * Supply Voltage : 10-30V DC
        * It has short-circuit protection for the motor outputs, over-voltage and under-voltage protection and will survive accidental motor    disconnects while powered-up
        * This drive is configured using MODBUS ASCII protocol via UART

# Register Mapping
    * Refer Documents 
