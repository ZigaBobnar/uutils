import dynamic_value, { dynamic_get_length } from './dynamic_value';

const uproto_message_start = 0b00110011;
const uproto_message_end = 0b11001100;

function to_bin8(value: number) {
    let result = value.toString(2);
    if (result.length < 8) {
        for (let i = 0; i < 8 - result.length; i++) {
            result = `0${result}`;
        }
    }

    return `0b${result}`;
}

class uproto_message {
    message_properties: number = 0;
    resource_id: dynamic_value = new dynamic_value();
    payload_length: dynamic_value = new dynamic_value();
    payload: number[] = [];
    checksum: number = 0;

    serialize() {
        this.payload_length.real_value = this.payload.length;

        let result = [
            uproto_message_start,
            this.message_properties,
            ...this.resource_id.serialize(),
            ...this.payload_length.serialize(),
            ...this.payload,
        ];

        if ((this.message_properties & 0b00000010) == 0) {
            this.checksum = result.reduce(i => i) & 0xFF;
            result.push(this.checksum);
        }

        result.push(uproto_message_end);
        
        return result;
    }

    calculate_checksum() {
        return (uproto_message_start +
            this.message_properties +
            this.resource_id.serialize().reduce(i => i) +
            this.payload_length.serialize().reduce(i => i) +
            this.payload.reduce(i => i)
        ) & 0xFF;
    }

    to_string() {
        return `[uproto_message]\n` +
            `  message_properties=${to_bin8(this.message_properties)}\n` +
            `  resource_id=0x${this.resource_id.real_value.toString(16)}\n` +
            `  payload_length=${this.payload_length.real_value}\n` +
            `  payload=${this.payload.map(i => `0x${i.toString(16)}`).join(' ')}\n` + 
            `  checksum=0x${this.checksum.toString(16)}\n` +
            `  checksum(calculated)=0x${this.calculate_checksum().toString(16)}\n`;
    }
}

class dynamic_parsing_state {
    required: number;
    cached: number = 0;
    cache: number[] = [];

    constructor(required: number) {
        this.required = required;
    }
}

class uproto_runtime {
    parser_state = 0;
    ready_message?: uproto_message = null;
    parsing_message?: uproto_message = null;

    resource_id_state?: dynamic_parsing_state = null;
    payload_length_state?: dynamic_parsing_state = null;

    constructor() {
        // TODO: Open connection.
    }

    parse_data(data: number) {
        if (this.parser_state == 0) {
            // Start

            if (data == uproto_message_start) {
                this.parsing_message = new uproto_message();
                this.parser_state = 1;
            }

        } else if (this.parser_state == 1) {
            // Message properties

            this.parsing_message.message_properties = data;
            this.parser_state = 2;

        } else if (this.parser_state == 2) {
            // Resource id

            if (this.resource_id_state == null) {
                this.resource_id_state = new dynamic_parsing_state(
                    dynamic_get_length(data)
                );

                if (this.resource_id_state.required == 1) {
                    this.parsing_message.resource_id.parse_dynamic([data]);
                    this.resource_id_state = null;
                    this.parser_state = 3;
                    return;
                }
            }

            this.resource_id_state.cache[this.resource_id_state.cached] = data;
            this.resource_id_state.cached += 1;

            if (this.resource_id_state.cached == this.resource_id_state.required) {
                this.parsing_message.resource_id.parse_dynamic(this.resource_id_state.cache);
                this.resource_id_state = null;
                this.parser_state = 3;
            }

        } else if (this.parser_state == 3) {
            // Payload length

            if (this.payload_length_state == null) {
                this.payload_length_state = new dynamic_parsing_state(
                    dynamic_get_length(data)
                );

                if (this.payload_length_state.required == 1) {
                    this.parsing_message.payload_length.parse_dynamic([data]);
                    this.payload_length_state = null;
                    this.parser_state = 4;
                    return;
                }
            }

            this.payload_length_state.cache[this.payload_length_state.cached] = data;
            this.payload_length_state.cached += 1;

            if (this.payload_length_state.cached == this.payload_length_state.required) {
                this.parsing_message.payload_length.parse_dynamic(this.payload_length_state.cache);
                this.payload_length_state = null;
                this.parser_state = 4;
            }

        } else if (this.parser_state == 4) {
            // Payload

            if (this.parsing_message.payload_length.real_value == 0) {
                this.parser_state = 5;
                this.parse_data(data);
                return;
            }

            this.parsing_message.payload.push(data);

            if (this.parsing_message.payload.length >= this.parsing_message.payload_length.real_value) {
                this.parser_state = 5;
            }

        } else if (this.parser_state == 5) {
            // Checksum

            if ((this.parsing_message.message_properties & 0b00000010) == 0b00000010) {
                this.parser_state = 6;
                this.parse_data(data);
                return;
            }

            this.parsing_message.checksum = data;
            this.parser_state = 6;

        } else if (this.parser_state == 6) {
            // Message end

            if (data != uproto_message_end) {
                console.error(`Invalid message end detected: 0x${data.toString(16)}`);
            }

            this.ready_message = this.parsing_message;
            this.parsing_message = null;
            this.parser_state = 0;
        }
    }
}

export {
    uproto_message_start,
    uproto_message_end,
    uproto_message,
};
