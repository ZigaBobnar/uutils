import { uproto_runtime } from "../uutils/uproto/runtime";
import { uproto_message } from "../uutils/uproto/message";
import { CommunicationLogModel } from "../models/communication_log";

export type communicationDataReceiveListener = {(data: number[]): void};

export interface IUprotoCommunication {
    sendMessage(message: uproto_message);

    setReceiveListener(listener: communicationDataReceiveListener);
}

export class UprotoDevice {
    private runtime: uproto_runtime;
    private communication: IUprotoCommunication;

    constructor(public readonly name: string, public readonly type: string, communication: IUprotoCommunication) {
        this.communication = communication;
        this.runtime = new uproto_runtime();

        this.communication.setReceiveListener(this.receiveFromCommunication);
        this.runtime.attach_respond_adapter((runtime, message) => {
            this.communication.sendMessage(message);
            return true;
        });

        this.runtime.attach_receive_adapter((runtime, message) => {
            CommunicationLogModel.create({
                deviceName: this.name,
                direction: 'in',
                messageData: new Uint8Array(message.serialize()),
            }, (err) => {
            });

            return false;
        });
    }

    sendMessage(message: uproto_message) {
        CommunicationLogModel.create({
            deviceName: this.name,
            direction: 'out',
            messageData: new Uint8Array(message.serialize()),
        }, (err) => {
        });

        this.communication.sendMessage(message);
    }

    private receiveFromCommunication(data: number[]) {
        for (let d of data) {
            this.runtime.feed_data(d);
        }
    }
}
