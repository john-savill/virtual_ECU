/*
 * SOAFEE-Compliant Virtual ECU
 * Minimal modular implementation with basic ECU functionalities
 * 
 * main.h
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>

#define PORT 8081
#define BUFFER_SIZE 1024
#define UPDATE_INTERVAL_MS 1000

// ECU State Structure
typedef struct {
    bool power_on;
    bool brake_active;
    bool adas_active;
    int diagnostic_code;
    float battery_voltage;
    int temperature_c;
    char status_message[256];
} ECUState;

// Global ECU State
ECUState ecu_state = {
    .power_on = false,
    .brake_active = false,
    .adas_active = false,
    .diagnostic_code = 0,
    .battery_voltage = 0.0,
    .temperature_c = 0,
    .status_message = "ECU Initialized"
};