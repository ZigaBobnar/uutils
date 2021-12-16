import { getModelForClass, prop, defaultClasses } from '@typegoose/typegoose';

class Device extends defaultClasses.TimeStamps {
    @prop({ required: true, unique: true, index: true })
    public name!: string;

    @prop({ required: true })
    public type!: string;
}

const DeviceModel = getModelForClass(Device);

export { DeviceModel }
