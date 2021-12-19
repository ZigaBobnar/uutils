import SerialPort from 'serialport';
import { uproto_message } from '../uutils/uproto/message';
import { communicationDataReceiveListener, IUprotoCommunication } from './uproto-communication';

export class SerialCommunication implements IUprotoCommunication {
    port: SerialPort;
    private listener: communicationDataReceiveListener;

    openPort(path: string, baud: number) {
        this.port = new SerialPort(path, {
            baudRate: baud,
        }, (error) => {
            if (error) {
                console.error(`Serial open failed (${path}), ${error.message}`);
            }
        });

        this.port.on('data', this.receive);
    }

    write(data: number[]) {
        this.port.write(data, (error) => {
            if (error) {
                console.error(`Serial write failed (${this.port.path}), ${error.message}`);
            }
        });
    }

    sendMessage(message: uproto_message) {
        this.write(message.serialize());
    }

    setReceiveListener(listener: communicationDataReceiveListener) {
        this.listener = listener;
    }

    private receive(data) {
        console.log('Receiving', data);
        this.listener(data);
    }
}
