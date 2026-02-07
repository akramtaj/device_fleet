#include <iostream>
#include <memory>
#include <string>
#include <map>
#include <mutex>
#include <thread>
#include <chrono>
#include <uuid/uuid.h> // Ensure libuuid-dev is installed

#include <grpcpp/grpcpp.h>
#include "device_service.grpc.pb.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using fleet::DeviceManagement;
using fleet::RegisterRequest;
using fleet::DeviceResponse;
using fleet::ActionRequest;
using fleet::ActionResponse;
using fleet::ActionStatusRequest;
using fleet::ActionStatusResponse;
using fleet::DeviceStatus;

struct Device {
    std::string id;
    DeviceStatus status;
};

class DeviceManagementImpl final : public DeviceManagement::Service {
    std::map<std::string, Device> devices;
    std::map<std::string, std::string> action_statuses;
    std::mutex mu;

public:
    Status RegisterDevice(ServerContext* context, const RegisterRequest* request, DeviceResponse* response) override {
        std::lock_guard<std::mutex> lock(mu);
        devices[request->device_id()] = {request->device_id(), DeviceStatus::IDLE};
        response->set_device_id(request->device_id());
        response->set_status(DeviceStatus::IDLE);
        return Status::OK;
    }

    Status InitiateDeviceAction(ServerContext* context, const ActionRequest* request, ActionResponse* response) override {
        std::lock_guard<std::mutex> lock(mu);
        if (devices.find(request->device_id()) == devices.end()) {
            return Status(grpc::NOT_FOUND, "Device not found");
        }

        std::string action_id = "act-" + std::to_string(rand() % 10000);
        action_statuses[action_id] = "RUNNING";
        devices[request->device_id()].status = DeviceStatus::UPDATING;

        // Simulate long-running task asynchronously
        std::thread([this, action_id, dev_id = request->device_id()]() {
            std::this_thread::sleep_for(std::chrono::seconds(15));
            std::lock_guard<std::mutex> lock(mu);
            action_statuses[action_id] = "COMPLETED";
            devices[dev_id].status = DeviceStatus::IDLE;
        }).detach();

        response->set_action_id(action_id);
        return Status::OK;
    }

    Status GetDeviceActionStatus(ServerContext* context, const ActionStatusRequest* request, ActionStatusResponse* response) override {
        std::lock_guard<std::mutex> lock(mu);
        auto it = action_statuses.find(request->action_id());
        response->set_status(it != action_statuses.end() ? it->second : "UNKNOWN");
        return Status::OK;
    }
};

void RunServer() {
    std::string server_address("0.0.0.0:50051");
    DeviceManagementImpl service;
    ServerBuilder builder;
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);
    std::unique_ptr<Server> server(builder.BuildAndStart());
    std::cout << "Server listening on " << server_address << std::endl;
    server->Wait();
}

int main() {
    RunServer();
    return 0;
}
