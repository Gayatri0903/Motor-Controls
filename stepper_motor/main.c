#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "driver/gpio.h"
#include "driver/uart.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// ================= PIN CONFIG =================
#define X_PULSE 16
#define X_DIR   17

#define YP_PULSE 13
#define YP_DIR   14

#define YM_PULSE 11
#define YM_DIR   10

#define RELAY1 1
#define RELAY2 2
#define RELAY3 3
#define RELAY4 4
#define RELAY5 5

// ================= MOTOR CONFIG =================
#define STEPS_PER_REV 200      // 1.8 degree motor
#define MICROSTEP     1        // change if driver microstepping enabled

// ================= GLOBAL FLAGS =================
volatile bool stop_all = false;

// software position tracking (in steps)
volatile int pos_x = 0;
volatile int pos_yp = 0;
volatile int pos_ym = 0;

// ================= GPIO INIT =================
void init_gpio()
{
    int pins[] = {
        X_PULSE, X_DIR,
        YP_PULSE, YP_DIR,
        YM_PULSE, YM_DIR,
        RELAY1, RELAY2, RELAY3, RELAY4, RELAY5
    };

    for (int i = 0; i < sizeof(pins)/sizeof(pins[0]); i++) {
        gpio_reset_pin(pins[i]);
        gpio_set_direction(pins[i], GPIO_MODE_OUTPUT);
        gpio_set_level(pins[i], 0);
    }
}

// ================= STEPPER ROTATION FUNCTION =================
void run_stepper_rotations(
    int pulse_pin,
    int dir_pin,
    int direction,
    float rotations,
    int speed_us,
    volatile int *position
)
{
    int total_steps = (int)(rotations * STEPS_PER_REV * MICROSTEP);

    gpio_set_level(dir_pin, direction);

    for (int i = 0; i < total_steps; i++) {
        if (stop_all) break;

        gpio_set_level(pulse_pin, 1);
        ets_delay_us(speed_us);
        gpio_set_level(pulse_pin, 0);
        ets_delay_us(speed_us);

        *position += (direction ? 1 : -1);
    }
}

// ================= RELAY CONTROL =================
void relay_control(int relay, int state)
{
    gpio_set_level(relay, state);
}

// ================= UART INIT =================
void uart_init()
{
    uart_config_t cfg = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };

    uart_driver_install(UART_NUM_0, 2048, 0, 0, NULL, 0);
    uart_param_config(UART_NUM_0, &cfg);
}

// ================= COMMAND TASK =================
void command_task(void *arg)
{
    char data[128];

    while (1) {
        int len = uart_read_bytes(UART_NUM_0, data, sizeof(data)-1, pdMS_TO_TICKS(100));
        if (len <= 0) continue;

        data[len] = 0;

        // -------- STOP ALL --------
        if (strstr(data, "STOP")) {
            stop_all = true;
            printf("ALL MOTORS STOPPED\n");
        }

        // -------- ZERO POSITION --------
        else if (strstr(data, "ZERO")) {
            stop_all = false;
            pos_x = pos_yp = pos_ym = 0;
            printf("ZERO POSITION SET\n");
        }

        // -------- MOTOR X --------
        else if (strncmp(data, "X ", 2) == 0) {
            char dir[4];
            float rot;
            int speed;

            sscanf(data, "X %s %f %d", dir, &rot, &speed);
            stop_all = false;

            run_stepper_rotations(
                X_PULSE, X_DIR,
                strcmp(dir, "CW") == 0,
                rot, speed, &pos_x
            );
        }

        // -------- MOTOR Y+ --------
        else if (strncmp(data, "YP", 2) == 0) {
            char dir[4];
            float rot;
            int speed;

            sscanf(data, "YP %s %f %d", dir, &rot, &speed);
            stop_all = false;

            run_stepper_rotations(
                YP_PULSE, YP_DIR,
                strcmp(dir, "CW") == 0,
                rot, speed, &pos_yp
            );
        }

        // -------- MOTOR Y- --------
        else if (strncmp(data, "YM", 2) == 0) {
            char dir[4];
            float rot;
            int speed;

            sscanf(data, "YM %s %f %d", dir, &rot, &speed);
            stop_all = false;

            run_stepper_rotations(
                YM_PULSE, YM_DIR,
                strcmp(dir, "CW") == 0,
                rot, speed, &pos_ym
            );
        }

        // -------- RELAYS --------
        else if (strstr(data, "R1 ON"))  relay_control(RELAY1, 1);
        else if (strstr(data, "R1 OFF")) relay_control(RELAY1, 0);

        else if (strstr(data, "R2 ON"))  relay_control(RELAY2, 1);
        else if (strstr(data, "R2 OFF")) relay_control(RELAY2, 0);

        else if (strstr(data, "R3 ON"))  relay_control(RELAY3, 1);
        else if (strstr(data, "R3 OFF")) relay_control(RELAY3, 0);

        else if (strstr(data, "R4 ON"))  relay_control(RELAY4, 1);
        else if (strstr(data, "R4 OFF")) relay_control(RELAY4, 0);

        else if (strstr(data, "R5 ON"))  relay_control(RELAY5, 1);
        else if (strstr(data, "R5 OFF")) relay_control(RELAY5, 0);
    }
}

// ================= MAIN =================
void app_main()
{
    init_gpio();
    uart_init();
    xTaskCreate(command_task, "command_task", 4096, NULL, 5, NULL);
}
 