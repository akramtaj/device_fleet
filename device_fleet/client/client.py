import grpc
import argparse
import device_service_pb2
import device_service_pb2_grpc

def run():
    parser = argparse.ArgumentParser(description="Fleet Management CLI")
    subparsers = parser.add_subparsers(dest="command")

    # Register command
    reg_parser = subparsers.add_parser("register")
    reg_parser.add_argument("--id", required=True)

    # Update command
    upd_parser = subparsers.add_parser("update")
    upd_parser.add_argument("--id", required=True)

    # Track command
    track_parser = subparsers.add_parser("track")
    track_parser.add_argument("--action_id", required=True)

    args = parser.parse_args()
    channel = grpc.insecure_channel('localhost:50051')
    stub = device_service_pb2_grpc.DeviceManagementStub(channel)

    if args.command == "register":
        response = stub.RegisterDevice(device_service_pb2.RegisterRequest(device_id=args.id))
        print(f"Device {response.device_id} registered with status {response.status}")
    
    elif args.command == "update":
        response = stub.InitiateDeviceAction(device_service_pb2.ActionRequest(device_id=args.id, action_type="SOFTWARE_UPDATE"))
        print(f"Update initiated. Action ID: {response.action_id}")

    elif args.command == "track":
        response = stub.GetDeviceActionStatus(device_service_pb2.ActionStatusRequest(action_id=args.action_id))
        print(f"Action Status: {response.status}")

if __name__ == "__main__":
    run()
