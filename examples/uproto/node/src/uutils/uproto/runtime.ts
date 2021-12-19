import os from "os";
import { real_to_dynamic_array } from "../dynamic_value";
import { uproto_message, uproto_message_end, uproto_message_start } from "./message";
import { uproto_message_parser_result, uproto_parser } from "./parser";
import { uproto_current_version, uproto_version_info, uproto_version_info_serialize } from "./version";

export const uproto_system_resource_ids = {
    do_nothing: 0x00,
    echo_message: 0x01,
    system_initialize: 0x02,
    system_status: 0x03,
    protocol_version: 0x04,
    application_version: 0x05,
    device_identifier_string: 0x06,
    application_identifier_string: 0x07,
};

export type uproto_runtime_adapter = {(runtime: uproto_runtime, message: uproto_message): boolean};

export class uproto_runtime {
    device_identifier = os.hostname();
    application_identifier = 'Generic Node uproto runtime';
    application_version = new uproto_version_info(1, 0, 0);

    parser: uproto_parser = new uproto_parser();

    receive_adapters: uproto_runtime_adapter[] = [];
    respond_adapters: uproto_runtime_adapter[] = [];

    constructor() {
        this.attach_receive_adapter(this.system_receive_adapter_action);
    }

    feed_data(value: number) {
        const result = this.parser.parse_single(value);

        if (result == uproto_message_parser_result.ok) {
            return;
        } else if (result == uproto_message_parser_result.message_ready) {
            const message = this.parser.get_ready_message();

            for (let adapter of this.receive_adapters) {
                if (adapter(this, message)) {
                    break;
                }
            }
        } else if (result == uproto_message_parser_result.queue_full) {
            const message = this.parser.get_ready_message();

            for (let adapter of this.receive_adapters) {
                if (adapter(this, message)) {
                    break;
                }
            }

            this.feed_data(value);
        } else if (result == uproto_message_parser_result.unknown_error) {
        } else {
        }
    }

    feed_message(message: uproto_message) {
        for (let adapter of this.receive_adapters) {
            if (adapter(this, message)) {
                return;
            }
        }
    }

    respond_with_message(message: uproto_message) {
        for (let adapter of this.respond_adapters) {
            if (adapter(this, message)) {
                return;
            }
        }
    }

    attach_receive_adapter(adapter: uproto_runtime_adapter, push_back = true) {
        for (let a of this.receive_adapters) {
            if (adapter == a) {
                return;
            }
        }

        if (push_back) {
            this.receive_adapters.push(adapter);
        } else {
            this.receive_adapters.unshift(adapter);
        }
    }

    remove_receive_adapter(adapter: uproto_runtime_adapter) {
        for (let a of this.receive_adapters) {
            if (adapter == a) {
                this.receive_adapters.splice(this.receive_adapters.indexOf(a), 1);
                return;
            }
        }
    }

    attach_respond_adapter(adapter: uproto_runtime_adapter, push_back = true) {
        for (let a of this.respond_adapters) {
            if (adapter == a) {
                return;
            }
        }
        
        if (push_back) {
            this.respond_adapters.push(adapter);
        } else {
            this.respond_adapters.unshift(adapter);
        }
    }

    remove_respond_adapter(adapter: uproto_runtime_adapter) {
        for (let a of this.respond_adapters) {
            if (adapter == a) {
                this.respond_adapters.splice(this.respond_adapters.indexOf(a), 1);
                return;
            }
        }
    }

    system_receive_adapter_action(runtime: uproto_runtime, message: uproto_message) {
        if (message.is_request()) {
            if (message.resource_id == uproto_system_resource_ids.do_nothing) {
                return true;
            } else if (message.resource_id == uproto_system_resource_ids.echo_message) {
                let response = message.create_response_from_this();
                response.payload = message.payload;

                runtime.respond_with_message(response);

                return true;
            } else if (message.resource_id == uproto_system_resource_ids.system_initialize) {
                return true;
            } else if (message.resource_id == uproto_system_resource_ids.system_status) {
                let response = message.create_response_from_this();
                response.payload = [0x01];

                runtime.respond_with_message(response);

                return true;
            } else if (message.resource_id == uproto_system_resource_ids.protocol_version) {
                let response = message.create_response_from_this();
                response.payload = uproto_version_info_serialize(uproto_current_version);

                runtime.respond_with_message(response);

                return true;
            } else if (message.resource_id == uproto_system_resource_ids.application_version) {
                let response = message.create_response_from_this();
                response.payload = uproto_version_info_serialize(runtime.application_version);

                runtime.respond_with_message(response);

                return true;
            } else if (message.resource_id == uproto_system_resource_ids.device_identifier_string) {
                let response = message.create_response_from_this();
                response.payload = [];
                let id = runtime.device_identifier;
                for (let i = 0; i < id.length; i++) {
                    response.payload.push(id.charCodeAt(i));
                }

                runtime.respond_with_message(response);

                return true;
            } else if (message.resource_id == uproto_system_resource_ids.application_identifier_string) {
                let response = message.create_response_from_this();
                response.payload = [];
                let id = runtime.application_identifier;
                for (let i = 0; i < id.length; i++) {
                    response.payload.push(id.charCodeAt(i));
                }

                runtime.respond_with_message(response);

                return true;
            }
        }

        return false;
    }

    console_receive_adapter_action(runtime: uproto_runtime, message: uproto_message) {
        print_message_to_console(message, true);

        return false;
    }

    console_respond_adapter_action(runtime: uproto_runtime, message: uproto_message) {
        print_message_to_console(message, false);

        return false;
    }
}

function print_message_to_console(message: uproto_message, is_receiving: boolean) {
    console.info(`=> Runtime ${is_receiving ? 'receiving' : 'sending'} message`);
    console.info(`  Message start: 0x${hex(uproto_message_start)}`);
    console.info(`  Properties: 0x${hex(message.message_properties)} (Dynamic: 0x${real_to_dynamic_array(message.message_properties)})`);
    console.info(`  Resource id: 0x${hex(message.resource_id)} (Dynamic: 0x${real_to_dynamic_array(message.resource_id)})`);
    console.info(`  Payload length: 0x${hex(message.payload_length)} (Dynamic: 0x${real_to_dynamic_array(message.payload_length)})`);
    if (message.payload_length <= 0) {
        console.info(`  Payload: (none)`);
    } else {
        let x = `  Payload: `;

        for (let a of message.payload) {
            x += `0x${hex(a)} `;
        }

        console.info(x);
    }
    console.info(`  Checksum: 0x${hex(message.checksum)}`);
    console.info(`  Calculated checksum: 0x${hex(message.calculate_checksum())}`);
    console.info(`  Message end: 0x${hex(uproto_message_end)}`);
    console.info(``);
}

function hex(value: number) {
    return value.toString(16);
}
