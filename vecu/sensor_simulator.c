/**
 * Minimal Sensor Simulator for vECU
 * Sends simple JSON packages with random sensor variations
 * Author: Auto-generated
 * Version: 1.0 - Minimal Implementation
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

// Configuration
#define VECU_IP "127.0.0.1"
#define VECU_PORT 8081
#define UPDATE_INTERVAL 1000000  // 1 second in microseconds

// Sensor data structure
typedef struct {
    float speed;        // km/h
    float battery;      // percentage
    float temperature;  // celsius
} SensorData;

// Global variables
SensorData sensors = {0.0, 100.0, 20.0};  // Initial values
int sock_fd = -1;

// Function prototypes
int connect_to_vecu(void);
void update_sensors(void);
void send_sensor_data(void);
float random_variation(float base, float range);

int main(void) {
    printf("Starting Minimal Sensor Simulator...\n");
    
    // Initialize random seed
    srand(time(NULL));
    
    // Connect to vECU
    if (connect_to_vecu() < 0) {
        printf("Failed to connect to vECU\n");
        return -1;
    }
    
    printf("Connected to vECU at %s:%d\n", VECU_IP, VECU_PORT);
    printf("Sending sensor data every second...\n");
    
    // Main simulation loop
    while (1) {
        update_sensors();
        send_sensor_data();
        usleep(UPDATE_INTERVAL);
    }
    
    // Cleanup
    if (sock_fd >= 0) {
        close(sock_fd);
    }
    
    return 0;
}

int connect_to_vecu(void) {
    struct sockaddr_in server_addr;
    
    // Create socket
    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0) {
        perror("Socket creation failed");
        return -1;
    }
    
    // Setup server address
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(VECU_PORT);
    
    if (inet_pton(AF_INET, VECU_IP, &server_addr.sin_addr) <= 0) {
        perror("Invalid address");
        return -1;
    }
    
    // Connect to server
    if (connect(sock_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        return -1;
    }
    
    return 0;
}

void update_sensors(void) {
    // Update speed (0-120 km/h with small random changes)
    sensors.speed += random_variation(0, 2.0);
    if (sensors.speed < 0) sensors.speed = 0;
    if (sensors.speed > 120) sensors.speed = 120;
    
    // Update battery (0-100% with small discharge)
    sensors.battery += random_variation(-0.1, 0.2);
    if (sensors.battery < 0) sensors.battery = 0;
    if (sensors.battery > 100) sensors.battery = 100;
    
    // Update temperature (10-40°C with small variations)
    sensors.temperature += random_variation(0, 1.0);
    if (sensors.temperature < 10) sensors.temperature = 10;
    if (sensors.temperature > 40) sensors.temperature = 40;
}

void send_sensor_data(void) {
    char json_buffer[256];
    
    // Create JSON message
    snprintf(json_buffer, sizeof(json_buffer),
        "{"
        "\"timestamp\":%ld,"
        "\"speed\":%.2f,"
        "\"battery\":%.2f,"
        "\"temperature\":%.2f"
        "}\n",
        time(NULL),
        sensors.speed,
        sensors.battery,
        sensors.temperature
    );
    
    // Send data to vECU
    ssize_t bytes_sent = send(sock_fd, json_buffer, strlen(json_buffer), 0);
    if (bytes_sent < 0) {
        perror("Send failed");
        // Try to reconnect
        close(sock_fd);
        if (connect_to_vecu() < 0) {
            printf("Reconnection failed, exiting...\n");
            exit(1);
        }
    } else {
        printf("Sent: %s", json_buffer);
    }
}

float random_variation(float base, float range) {
    // Generate random variation: base ± range/2
    float random_factor = (float)rand() / RAND_MAX;  // 0.0 to 1.0
    return base + (random_factor - 0.5) * range;
}