import { uproto_message } from "../uutils/uproto/message";
import { Device, DeviceModel } from "../models/device";
import { SerialCommunication } from "./serial-communication";
import { UprotoDevice } from "./uproto-communication";
import { EmulatorCommuncation } from "./uproto-emulator";

export class UprotoService {
    devices: UprotoDevice[] = [];

    async initialize() {
        let dbDevices = await DeviceModel.find({});

        for (let d of dbDevices) {
            this.addDevice(d);
        }
    }

    addDevice(device: Device) {
        if (device.type == 'serial') {
            const communication = new SerialCommunication();
            communication.openPort(device.configuration.path, device.configuration.baud);
            this.devices.push(new UprotoDevice(device.name, device.type, communication));
        } else if (device.type == 'emulator') {
            const communication = new EmulatorCommuncation();
            this.devices.push(new UprotoDevice(device.name, device.type, communication));
        } else {
            console.error('Unsupported device', device);
        }
    }

    sendToDevice(device_name: string, message: uproto_message) {
        for (let d of this.devices) {
            if (d.name == device_name) {
                d.sendMessage(message);
                return;
            }
        }
    }
}

export const uprotoService = new UprotoService();
