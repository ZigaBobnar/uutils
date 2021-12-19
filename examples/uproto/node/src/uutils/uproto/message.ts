import { real_to_dynamic_array } from "../dynamic_value";

export const uproto_message_start = 0b00110011;
export const uproto_message_end = 0b11001100;

export enum uproto_parse_status {
    unknown,
    ok,
    parsing,
    end_invalid,
}

export class uproto_message {
    message_properties: number;
    resource_id: number;
    payload_length: number;
    payload: number[];
    checksum: number;

    parse_status: uproto_parse_status = uproto_parse_status.unknown;

    create_response_from_this(): uproto_message {
        let message = new uproto_message();
        message.message_properties = this.message_properties;
        message.set_as_response_type();
        message.resource_id = this.resource_id;

        return message;
    }

    is_request(): boolean {
        return (this.message_properties & 0b00000001) == 0;
    }

    is_response(): boolean {
        return (this.message_properties & 0b00000001) == 0b00000001;
    }

    set_as_request_type() {
        this.message_properties &= ~0b00000001;
    }
    
    set_as_response_type() {
        this.message_properties |= 0b00000001;
    }

    has_checksum(): boolean {
        return (this.message_properties & 0b00000010) == 0b00000010;
    }

    skips_checksum(): boolean {
        return (this.message_properties & 0b00000010) == 0;
    }

    calculate_checksum(): number {
        let checksum = 0;
        checksum += uproto_message_start;
        checksum += real_to_dynamic_array(this.message_properties).reduce(x => x);
        checksum += real_to_dynamic_array(this.resource_id).reduce(x => x);
        checksum += real_to_dynamic_array(this.payload_length).reduce(x => x);
        for (let x of this.payload) {
            checksum += x;
        }

        return checksum % 0xFF;
    }

    is_checsum_valid(): boolean {
        return this.calculate_checksum() == this.checksum;
    }

    serialize(): number[] {
        let message = [];
        this.payload_length = this.payload?.length ?? 0;

        message.push(uproto_message_start);

        message.push(...real_to_dynamic_array(this.message_properties));
        message.push(...real_to_dynamic_array(this.resource_id));
        message.push(...real_to_dynamic_array(this.payload_length));

        if (this.payload_length > 0) {
            message.push(...this.payload);
        }

        if (this.has_checksum()) {
            message.push(this.calculate_checksum());
        }

        message.push(uproto_message_end);

        return message;
    }
}
