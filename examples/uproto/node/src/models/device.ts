import { getModelForClass, prop, defaultClasses } from '@typegoose/typegoose';

export class Device extends defaultClasses.TimeStamps {
    @prop({ required: true, unique: true, index: true })
    public name!: string;

    @prop({ required: true })
    public type!: string;

    @prop()
    public configuration: any;
}

export const DeviceModel = getModelForClass(Device);
