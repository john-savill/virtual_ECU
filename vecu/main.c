/*
 * SOAFEE-Compliant Virtual ECU
 * Minimal modular implementation with basic ECU functionalities
 * 
 * main.c
 */

#include "main.h"

volatile sig_atomic_t running = 1;

// Signal handler for graceful shutdown
void signal_handler(int signum) {
    printf("\nReceived signal %d, shutting down...\n", signum);
    running = 0;
}

// ECU Functions
void ecu_power_on() {
    ecu_state.power_on = true;
    ecu_state.battery_voltage = 12.6;
    ecu_state.diagnostic_code = 0;
    strcpy(ecu_state.status_message, "ECU Powered ON");
    printf("[ECU] Power ON\n");
}

void ecu_power_off() {
    ecu_state.power_on = false;
    ecu_state.brake_active = false;
    ecu_state.adas_active = false;
    strcpy(ecu_state.status_message, "ECU Powered OFF");
    printf("[ECU] Power OFF\n");
}

void ecu_activate_brakes() {
    if (!ecu_state.power_on) {
        strcpy(ecu_state.status_message, "ERROR: ECU not powered on");
        return;
    }
    ecu_state.brake_active = true;   
    strcpy(ecu_state.status_message, "Brakes ACTIVATED");
    printf("[ECU] Brakes activated\n");
}

void ecu_deactivate_brakes() {
    if (!ecu_state.power_on) {
        strcpy(ecu_state.status_message, "ERROR: ECU not powered on");
        return;
    }
    ecu_state.brake_active = false;
    strcpy(ecu_state.status_message, "Brakes DEACTIVATED");
    printf("[ECU] Brakes deactivated\n");
}

void ecu_trigger_adas() {
    if (!ecu_state.power_on) {
        strcpy(ecu_state.status_message, "ERROR: ECU not powered on");
        return;
    }
    ecu_state.adas_active = !ecu_state.adas_active;
    if (ecu_state.adas_active) {
        strcpy(ecu_state.status_message, "ADAS ACTIVATED");
        printf("[ECU] ADAS activated\n");
    } else {
        strcpy(ecu_state.status_message, "ADAS DEACTIVATED");
        printf("[ECU] ADAS deactivated\n");
    }
}

void ecu_run_diagnostics() {
    printf("[ECU] Running diagnostics...\n");
    
    // Simulate diagnostic checks
    if (ecu_state.battery_voltage < 11.5) {
        ecu_state.diagnostic_code = 101; // Low battery
        strcpy(ecu_state.status_message, "DIAGNOSTIC: Low battery voltage");
    } else if (ecu_state.temperature_c > 85) {
        ecu_state.diagnostic_code = 102; // Overheating
        strcpy(ecu_state.status_message, "DIAGNOSTIC: High temperature");
    } else {
        ecu_state.diagnostic_code = 0; // All OK
        strcpy(ecu_state.status_message, "DIAGNOSTIC: All systems OK");
    }
    
    printf("[ECU] Diagnostic code: %d\n", ecu_state.diagnostic_code);
}

// Generate JSON telemetry
void generate_json_telemetry(char *buffer, size_t buffer_size) {
    snprintf(buffer, buffer_size,
        "{\"power_on\":%s,\"brake_active\":%s,\"adas_active\":%s,"
        "\"diagnostic_code\":%d,\"battery_voltage\":%.2f,"
        "\"temperature_c\":%d,\"status_message\":\"%s\",\"timestamp\":%ld}",
        ecu_state.power_on ? "true" : "false",
        ecu_state.brake_active ? "true" : "false",
        ecu_state.adas_active ? "true" : "false",
        ecu_state.diagnostic_code,
        ecu_state.battery_voltage,
        ecu_state.temperature_c,
        ecu_state.status_message,
        time(NULL)
    );
}

// Process commands from HMI
void process_command(const char *command) {
    printf("[ECU] Received command: %s\n", command);
    
    if (strstr(command, "power_on")) {
        ecu_power_on();
    } else if (strstr(command, "power_off")) {
        ecu_power_off();
    } else if (strstr(command, "brake_on")) {
        ecu_activate_brakes();
    } else if (strstr(command, "brake_off")) {
        ecu_deactivate_brakes();
    } else if (strstr(command, "toggle_adas")) {
        ecu_trigger_adas();
    } else if (strstr(command, "diagnostics")) {
        ecu_run_diagnostics();
    }
}

int main() {
    int server_fd, client_fd;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE] = {0};
    char json_response[BUFFER_SIZE];
    
    // Setup signal handlers
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    
    printf("=== SOAFEE Virtual ECU Starting ===\n");
    printf("[ECU] Initializing...\n");
    
    srand(time(NULL));
    
    // Create socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }
    
    // Set socket options
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("Setsockopt failed");
        exit(EXIT_FAILURE);
    }
    
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    
    // Bind socket
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }
    
    // Listen for connections
    if (listen(server_fd, 3) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }
    
    printf("[ECU] Listening on port %d\n", PORT);
    printf("[ECU] Ready to accept connections\n");
    
    // Auto power on
    ecu_power_on();
    
    // Main loop
    while (running) {
        fd_set readfds;
        struct timeval tv;
        
        FD_ZERO(&readfds);
        FD_SET(server_fd, &readfds);
        
        tv.tv_sec = 1;
        tv.tv_usec = 0;
        
        int activity = select(server_fd + 1, &readfds, NULL, NULL, &tv);
        
        if (activity > 0 && FD_ISSET(server_fd, &readfds)) {
            if ((client_fd = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
                perror("Accept failed");
                continue;
            }
            
            // Read command
            int valread = read(client_fd, buffer, BUFFER_SIZE);
            if (valread > 0) {
                buffer[valread] = '\0';
                process_command(buffer);
            }
            
            // Send telemetry response
            generate_json_telemetry(json_response, BUFFER_SIZE);
            
            char http_response[BUFFER_SIZE * 2];
            snprintf(http_response, sizeof(http_response),
                "HTTP/1.1 200 OK\r\n"
                "Content-Type: application/json\r\n"
                "Access-Control-Allow-Origin: *\r\n"
                "Content-Length: %ld\r\n"
                "\r\n%s",
                strlen(json_response), json_response);
            
            send(client_fd, http_response, strlen(http_response), 0);
            close(client_fd);
        }
    }
    
    close(server_fd);
    printf("[ECU] Shutdown complete\n");
    
    return 0;
}
