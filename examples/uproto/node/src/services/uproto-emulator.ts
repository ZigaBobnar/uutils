import { uproto_runtime } from '../uutils/uproto/runtime';
import { uproto_message } from '../uutils/uproto/message';
import { communicationDataReceiveListener, IUprotoCommunication } from './uproto-communication';

export class EmulatorCommuncation implements IUprotoCommunication {
    private runtime: uproto_runtime;
    private listener: communicationDataReceiveListener;

    constructor() {
        this.runtime = new uproto_runtime();
        this.runtime.application_identifier = "Node uproto runtime emulator";

        this.runtime.attach_respond_adapter((runtime, message) => {
            this.listener(message.serialize());
            return true;
        });
    }

    sendMessage(message: uproto_message) {
        // this.runtime.feed_message(message);

        const data = message.serialize();
        for (let d of data) {
            this.runtime.feed_data(d);
        }
    }

    setReceiveListener(listener: communicationDataReceiveListener) {
        this.listener = listener;
    }
}
