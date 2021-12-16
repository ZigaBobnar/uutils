import { getModelForClass, prop, defaultClasses } from '@typegoose/typegoose';

class CommunicationLog extends defaultClasses.TimeStamps {
    @prop({ required: true })
    public deviceName!: string;

    @prop()
    public messageData: Uint8Array;
}

const CommunicationLogModel = getModelForClass(CommunicationLog);

export { CommunicationLogModel }
