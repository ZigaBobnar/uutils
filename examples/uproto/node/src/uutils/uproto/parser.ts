import { dynamic_parse_state } from "../dynamic_value";
import { uproto_message, uproto_message_end, uproto_message_start, uproto_parse_status } from "./message";

export enum uproto_message_parser_state {
    message_start,
    message_properties,
    resource_id,
    payload_length,
    payload,
    checksum,
    message_end,
}

export enum uproto_message_parser_result {
    ok,
    message_ready,
    queue_full,
    unknown_error,
}

export class uproto_parser {
    ready_message: uproto_message = null;
    parsing_message: uproto_message = null;

    state: uproto_message_parser_state = uproto_message_parser_state.message_start;

    message_properties_state: dynamic_parse_state = new dynamic_parse_state();
    resource_id_state: dynamic_parse_state = new dynamic_parse_state();
    payload_length_state: dynamic_parse_state = new dynamic_parse_state();

    current_payload_position: number = 0;
    current_message_payload_length: number = 0;

    parse_single(value: number): uproto_message_parser_result {
        value = value & 0xFF;

        if (this.state == uproto_message_parser_state.message_start) {
            if (value == uproto_message_start) {
                this.parsing_message = new uproto_message();
                this.parsing_message.parse_status = uproto_parse_status.parsing;

                this.state = uproto_message_parser_state.message_properties;
            }
        } else if (this.state == uproto_message_parser_state.message_properties) {
            if (this.message_properties_state.parse_dynamic_byte(value)) {
                this.parsing_message.message_properties = this.message_properties_state.processed_real;
                this.state = uproto_message_parser_state.resource_id;
            }
        } else if (this.state == uproto_message_parser_state.resource_id) {
            if (this.resource_id_state.parse_dynamic_byte(value)) {
                this.parsing_message.resource_id = this.resource_id_state.processed_real;
                this.state = uproto_message_parser_state.payload_length;
            }
        } else if (this.state == uproto_message_parser_state.payload_length) {
            if (this.payload_length_state.parse_dynamic_byte(value)) {
                this.parsing_message.payload_length = this.payload_length_state.processed_real;

                if (this.parsing_message.payload_length < 0) {
                    this.state = uproto_message_parser_state.message_start;

                    return uproto_message_parser_result.unknown_error;
                }

                this.state = this.parsing_message.payload_length > 0 ?
                    uproto_message_parser_state.payload :
                    uproto_message_parser_state.checksum;
            }
        } else if (this.state == uproto_message_parser_state.payload) {
            if (this.current_payload_position == 0) {
                if (this.parsing_message.payload_length < 0) {
                    // Invalid state
                    this.state = uproto_message_parser_state.message_start;

                    return uproto_message_parser_result.unknown_error;
                }

                this.current_message_payload_length = this.parsing_message.payload_length;
                this.parsing_message.payload = [];
            }

            this.parsing_message.payload[this.current_payload_position] = value;
            this.current_payload_position++;

            if (this.current_payload_position >= this.current_message_payload_length) {
                this.state = uproto_message_parser_state.checksum;
                this.current_payload_position = 0;
            }

        } else if (this.state == uproto_message_parser_state.checksum) {
            if (!this.parsing_message.has_checksum()) {
                this.state = uproto_message_parser_state.message_end;
                return this.parse_single(value);
            }

            this.parsing_message.checksum = value;
            this.state = uproto_message_parser_state.message_end;
        } else if (this.state == uproto_message_parser_state.message_end) {
            this.ready_message = this.parsing_message;
            this.parsing_message = null;

            this.state = uproto_message_parser_state.message_start;

            if (value == uproto_message_end) {
                this.ready_message.parse_status = uproto_parse_status.ok;
            } else {
                this.ready_message.parse_status = uproto_parse_status.end_invalid;
            }

            return uproto_message_parser_result.message_ready;
        } else {
            return uproto_message_parser_result.unknown_error;
        }

        return uproto_message_parser_result.ok;
    }

    has_message_ready(): boolean {
        return this.ready_message != null;
    }

    get_ready_message(): uproto_message {
        if (this.ready_message == null) {
            return null;
        }

        let message = this.ready_message;
        this.ready_message = null;

        return message;
    }
}
