from machine import UART, Pin
import time
import sys
import select

# -----------------------------
# Configuration
# -----------------------------
SLAVE_ID = 7                  # RMCS-2303 Slave ID (1-7)
PULSES_PER_ROTATION = 334     # Encoder lines from register 40011
INTERVAL = 0.05               # Seconds between reads
MOTOR_SPEED = 2200            # Initial speed command
ACCELERATION = 20000

# UART setup
uart = UART(0, baudrate=9600, tx=Pin(12), rx=Pin(13))
time.sleep(0.1)
uart.read()  # Clear buffer

print("=== RMCS-2303 Digital Speed Control ===")
print("Type 's' + Enter to stop the motor at any time.")

# -----------------------------
# LRC Calculation (Modbus ASCII)
# -----------------------------
def calc_lrc(msg_bytes):
    lrc = 0
    for b in msg_bytes:
        lrc = (lrc + b) & 0xFF
    lrc = ((-lrc) & 0xFF)
    return lrc

# -----------------------------
# Build ASCII Modbus command
# -----------------------------
def build_ascii_command(slave_id, func, reg_addr, value=None, num_regs=None):
    if func == 3:  # Read
        reg = reg_addr - 40001
        msg_bytes = bytearray([
            slave_id, func,
            (reg >> 8) & 0xFF, reg & 0xFF,
            (num_regs >> 8) & 0xFF, num_regs & 0xFF
        ])
    elif func == 6:  # Write single register
        reg = reg_addr - 40001
        msg_bytes = bytearray([
            slave_id, func,
            (reg >> 8) & 0xFF, reg & 0xFF,
            (value >> 8) & 0xFF, value & 0xFF
        ])
    else:
        raise ValueError("Unsupported function code")
    
    lrc = calc_lrc(msg_bytes)
    ascii_msg = ':' + ''.join('{:02X}'.format(b) for b in msg_bytes) + '{:02X}\r\n'.format(lrc)
    return ascii_msg

# -----------------------------
# Parse ASCII Modbus response
# -----------------------------
def parse_ascii_response(resp):
    if resp is None:
        return None
    try:
        resp_str = resp.decode().strip()[1:]  # Remove ':' and CRLF
        data_hex = resp_str[6:-2]            # Skip Slave + Func + ByteCount, remove LRC
        if len(data_hex) < 8:
            return None
        lsb = int(data_hex[0:4], 16)
        msb = int(data_hex[4:8], 16)
        position = (msb << 16) | lsb
        return position
    except:
        return None

# -----------------------------
# -----------------------------
# Step 1: Disable CCW (0x0108 = 264)
# -----------------------------
command = build_ascii_command(SLAVE_ID, 6, 40003, value=0x0108)
uart.write(command.encode())
time.sleep(0.1)
uart.read()  # discard response

# -----------------------------
# Step 2: Enable CW (0x0101 = 257)
# -----------------------------
command = build_ascii_command(SLAVE_ID, 6, 40003, value=0x0101)
uart.write(command.encode())
time.sleep(0.1)
uart.read()  # discard response

# -----------------------------
# Step 3: Set Acceleration
# -----------------------------
command = build_ascii_command(SLAVE_ID, 6, 40015, value=ACCELERATION)
uart.write(command.encode())
time.sleep(0.2)
uart.read()  # discard response

# -----------------------------
# Step 4: Set Motor Speed
# -----------------------------
command = build_ascii_command(SLAVE_ID, 6, 40013, value=MOTOR_SPEED)
uart.write(command.encode())
time.sleep(0.2)
uart.read()  # discard response


# -----------------------------
# Step 5: Read Position Continuously
# -----------------------------
previous_position = None

while True:
    # Read LSB + MSB position feedback
    command = build_ascii_command(SLAVE_ID, 3, 40021, num_regs=2)
    uart.write(command.encode())
    time.sleep(0.05)
    
    resp = None
    if uart.any():
        resp = uart.read()
    
    position = parse_ascii_response(resp)
    
    if position is not None:
        if previous_position is None:
            previous_position = position
            continue
        delta = position - previous_position
        rotations = delta / PULSES_PER_ROTATION
        rpm = rotations / INTERVAL * 60
        print("Position: {:>10} | ΔRotations: {:>6.2f} | RPM: {:>6.2f}".format(
            position, rotations, rpm))
        previous_position = position
    else:
        print("No data received...")

    # -----------------------------
    # Check for user input to stop motor
    # -----------------------------
    # Non-blocking check for 's' from shell
    if sys.stdin in select.select([sys.stdin], [], [], 0)[0]:
        user_input = sys.stdin.readline().strip()
        if user_input.lower() == 's':
            print("Stopping motor...")
            # Write to Mode 7, Control Byte 01 → Stop motor (maintain position)
            stop_cmd = build_ascii_command(SLAVE_ID, 6, 40003, value=0x0701)
            uart.write(stop_cmd.encode())
            time.sleep(0.1)
            uart.read()  # discard response
            print("Motor stopped.")
            break

    time.sleep(INTERVAL)
