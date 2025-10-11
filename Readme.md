# SOAFEE compliant Virtual Engine Control Unit and Dashboard

This is a Basic Virtual ECU (VECU) that is made to run in a docker container. It is [Soafee](https://www.soafee.io/) compliant. Alongside the VECU is a Human-Machine Interface (HMI), which is built and runs in a seperate docker container, on a local webpage.

## Features

### VECU
- Modular architecture - separated functions for power, brakes, diagnostics, and ADAS
- Service-oriented -  HTTP-based communication
- Containerised - Docker-based deployment
- Interface - JSON-based external communication
- Health Monitoring - health checks

### HMI
- Separate frontend - container for UI
- Standard HTTP methods
- Uodates quickly - polling for data

### Base ECU Functions

Power, brake control, ADAS on/off, diagnosistics, some sensor data (battery, temperature)

## Network Architecture

Containers are started and communicate of the defined "soafee-network" docker bridge.

```
 -----------------
|   Browser/HMI   | 
|  (Nginx/HTML)   | Docker container: virtual_ecu-hmi
|  (Port 80)      |
 --------+--------
         |
         │ HTTP/JSON
         │
 --------+--------
│  Virtual ECU    │
│  (C)            │ Docker container: virtual_ecu-vecu
|  (Port 8081)    |
 -----------------
```

## Build and Use

### Build both containers
```
docker compose build
```
### Start the system
```
docker compose up -d
```
### View logs
```
docker compose logs -f
```
### Access the Dashboard
Open web browser and navigate to:
http://localhost

### Stop the system
```
docker compose down
```

---

### Independent Docker builds (if required)
For HMI:
```
docker build --file hmi/hmi.Dockerfile -t hmi .
```
For VECU:
```
docker build --file vecu/vecu.Dockerfile -t soafee_vecu .
```
### Sensor simulator
build:
```
make sensor_simulator
```
run:
```
./sensor_simulator
```
## Development plan and known issues

- Containers starting unhealthy
- no yaml 
- hmi on all localhost ports
- only base functionality
- larger docker containers
