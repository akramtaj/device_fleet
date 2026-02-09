PROJECT REPORT: DEVICE FLEET MANAGEMENT SERVICE
1. Overview
This project implements a prototype backend service for managing a fleet of hardware devices. It utilizes a C++ backend for high-performance state management and a Python CLI for administrative interaction. Communication is facilitated via gRPC and Protocol Buffers.

2. Architecture & Interaction
* The Backend (C++): Serves as the single source of truth. It maintains an in-memory database of devices and their current operational states. It handles long-running tasks (like firmware updates) asynchronously using a thread-pool simulation to ensure the API remains responsive.
* The CLI (Python): A lightweight client that translates user commands into gRPC calls. It allows administrators to register devices, trigger updates, and poll for real-time status.
* The Contract (Protobuf): A shared .proto file ensures that both the C++ server and Python client agree on the data structures and service methods.

3. API Service Definition (fleet.proto)
The following definition outlines the interface between the client and server:
syntax = "proto3";
package fleet;
service FleetManagement {
rpc RegisterDevice (RegisterRequest) returns (DeviceResponse);
rpc SetDeviceStatus (StatusRequest) returns (DeviceResponse);
rpc GetDeviceInfo (DeviceRequest) returns (DeviceResponse);
rpc InitiateDeviceAction (ActionRequest) returns (ActionResponse);
rpc GetDeviceActionStatus (ActionStatusRequest) returns (ActionResponse);
rpc ListDevices (Empty) returns (DeviceList);
}

4. Setup and Build Instructions
A. Environment Prerequisites
The system is designed for Linux (Ubuntu/Debian).
* C++ Tools: sudo apt-get install build-essential libgrpc++-dev protobuf-compiler-grpc
* Python Tools: pip install grpcio grpcio-tools protobuf
B. Building the Service
Run the following command to generate the gRPC stubs and compile the C++ binary:
make

5. Usage Examples
Step 1: Start the Service
Execute the binary to start the gRPC server on port 50051:
./fleet_server
Step 2: Interact via CLI
* Register a New Device:
python3 client.py reg --id "SN-1002"
* Check Device Status:
python3 client.py status --id "SN-1002"
* Trigger Software Update:
python3 client.py update --id "SN-1002"
(The device will transition to 'UPDATING' for 15 seconds, then return to 'IDLE')
* List All Fleet Assets:
python3 client.py list

6. Assumptions and Future Improvements
Current Assumptions
1. In-Memory Storage: Device data is lost upon server restart (Simulation-only).
2. Insecure Channel: No SSL/TLS is used for this prototype.
3. Local Networking: Client and server are assumed to be on the same network.
Next Steps for Production
* Persistent Storage: Integrate PostgreSQL or SQLite to maintain device history.
* Security: Implement OAuth2 or JWT token-based authentication for the RegisterDevice and InitiateDeviceAction calls.
* Observability: Integrate Prometheus to track metrics like average update duration and device failure rates.


