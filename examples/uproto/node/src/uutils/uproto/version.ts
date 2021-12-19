import { dynamic_parse_state, real_to_dynamic_array } from "../dynamic_value";

const uproto_protocol_version_major: number = 0x00;
const uproto_protocol_version_minor: number = 0x01;
const uproto_protocol_version_patch: number = 0x00;

export class uproto_version_info {
    constructor(public major: number = 0, public minor: number = 0, public patch: number = 0) {}
}

export const uproto_current_version: uproto_version_info = {
    major: uproto_protocol_version_major,
    minor: uproto_protocol_version_minor,
    patch: uproto_protocol_version_patch,
};

export function uproto_version_info_parse(buffer: number[]): uproto_version_info {
    let info = new uproto_version_info();

    let dynamic_state = new dynamic_parse_state();
    let state = 0;
    for (let data_byte of buffer) {
        if (dynamic_state.parse_dynamic_byte(data_byte)) {
            if (state == 0) {
                info.major = dynamic_state.processed_real;
            } else if (state == 1) {
                info.minor = dynamic_state.processed_real;
            } else if (state == 2) {
                info.patch = dynamic_state.processed_real;
            } else {
                break;
            }
            
            state++;
        }
    }

    return info;
}

export function uproto_version_info_serialize(info: uproto_version_info): number[] {
    let buffer = [];

    buffer.push(...real_to_dynamic_array(info.major));
    buffer.push(...real_to_dynamic_array(info.minor));
    buffer.push(...real_to_dynamic_array(info.patch));

    return buffer;
}
