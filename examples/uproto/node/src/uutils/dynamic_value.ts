const byte_clamp = 0xFF;

const uint32_max_value = 0xFFFFFFFF;
const uint16_max_value = 0xFFFF;
const uint8_max_value = 0xFF;

const dynamic_1b_max = 0x3F;
const dynamic_1b_min = -64;
const dynamic_2b_max = 0x203F;
const dynamic_2b_min = -8256;
const dynamic_4b_max = 0x1000203F;
const dynamic_4b_min = -268443712;

const dynamic_2b_pos_offset = 0x0040;
const dynamic_2b_data_mask = 0x1FFF;
const dynamic_2b_preamble = 0b01000000;
const dynamic_4b_pos_offset = 0x00002040;
const dynamic_4b_data_mask = 0x0FFFFFFF;
const dynamic_4b_preamble = 0b01100000;

export function get_dynamic_array_size(dynamic_array: number[]): number {
    return get_dynamic_size_from_preamble(dynamic_array[0]);
}

export function get_dynamic_size(dynamic_value: number): number {
    const preamble = find_dynamic_preamble_byte(dynamic_value);
    return get_dynamic_size_from_preamble(preamble);
}

export function get_dynamic_size_from_preamble(preamble_byte: number): number {
    if ((preamble_byte & 0b10000000) == 0b10000000) {
        preamble_byte = ~preamble_byte;
    }

    return ((preamble_byte & 0b01111000) == 0b01111000) ? 16 :
        ((preamble_byte & 0b01110000) == 0b01110000) ? 8 :
        ((preamble_byte & 0b01100000) == 0b01100000) ? 4 :
        ((preamble_byte & 0b01000000) == 0b01000000) ? 2 :
        1;
}

export function find_dynamic_preamble_byte(dynamic_value: number): number {
    const size =
        dynamic_value > uint32_max_value ? 8 :
        dynamic_value > uint16_max_value ? 4 :
        dynamic_value > uint8_max_value ? 2 :
        1;

    return (
        (size == 1) ? (dynamic_value) & byte_clamp :
        (size == 2) ? (dynamic_value >> 8) & byte_clamp :
        (size == 4) ? (dynamic_value >> 24) & byte_clamp :
        0x00);
}

export function split_dynamic_to_array(dynamic_value: number): number[] {
    const size = get_dynamic_size(dynamic_value);

    if (size == 1) {
        return [
            dynamic_value & byte_clamp,
        ];
    } else if (size == 2) {
        return [
            (dynamic_value >> 8) & byte_clamp,
            (dynamic_value) & byte_clamp,
        ];
    } else if (size == 4) {
        return [
            (dynamic_value >> 24) & byte_clamp,
            (dynamic_value >> 16) & byte_clamp,
            (dynamic_value >> 8) & byte_clamp,
            (dynamic_value) & byte_clamp,
        ];
    } else {
        throw 'Invalid dynamic size';
    }
}

class dynamic_value {
    real_value: number;

    constructor(real_value = 0) {
        this.real_value = real_value;
    }

    parse_dynamic(dynamic_value: number): void {
        const dynamic_array = split_dynamic_to_array(dynamic_value);

        this.parse_dynamic_array(dynamic_array);
    }

    parse_dynamic_array(dynamic_array: number[]): number {
        const size = get_dynamic_size_from_preamble(dynamic_array[0]);
        const is_negative = (dynamic_array[0] & 0b10000000) == 0b10000000;

        if (!is_negative) {
            if (size == 1) {
                this.real_value = dynamic_array[0];
            } else if (size == 2) {
                this.real_value = ((
                    ((dynamic_array[0] & 0b00111111) << 8) |
                    dynamic_array[1]) +
                    64);
            } else if (size == 4) {
                this.real_value = ((
                    ((dynamic_array[0] & 0b00011111) << 24) |
                    ((dynamic_array[1]) << 16) |
                    ((dynamic_array[2]) << 8) |
                    (dynamic_array[3])) +
                    8256);
            } else {
                throw 'Invalid size';
            }
        } else {
            if (size == 1) {
                this.real_value = -(~dynamic_array[0] & 0b00111111) - 1;
            } else if (size == 2) {
                this.real_value = (
                    (((~(~dynamic_array[0] & 0b00111111)) | 0b11000000) << 8) |
                    (dynamic_array[1])) -
                    64;
            } else if (size == 4) {
                console.log(dynamic_array);
                this.real_value = (-(
                    ((255-(255-(255-dynamic_array[0] & 0b00011111) | 0b11100000)) << 24) |
                    ((255-dynamic_array[1]) << 16) |
                    ((255-dynamic_array[2]) << 8) |
                    ((255-dynamic_array[3]))) -
                    8257);
            } else {
                throw 'Invalid size';
            }
        }

        return size;
    }

    serialize(): number[] {
        const size = this.get_serialized_length();

        if (this.real_value >= 0) {
            if (size == 1) {
                return [ this.real_value ];
            } else if (size == 2) {
                let val = this.real_value - dynamic_2b_pos_offset;
                val &= dynamic_2b_data_mask;
                val |= dynamic_2b_preamble << 8;

                return [
                    (val >> 8) & byte_clamp,
                    val & byte_clamp
                ];
            } else if (size == 4) {
                let val = this.real_value - dynamic_4b_pos_offset;
                val &= dynamic_4b_data_mask;
                val |= dynamic_4b_preamble << 24;

                return [
                    (val >> 24) & byte_clamp,
                    (val >> 16) & byte_clamp,
                    (val >> 8) & byte_clamp,
                    val & byte_clamp
                ];
            } else {
                throw 'Invalid size';
            }
        } else {
            if (size == 1) {
                return [
                    ((this.real_value << 8) >>> 8) & 0xFF
                ];
            } else if (size == 2) {
                const real_val = this.real_value + dynamic_2b_pos_offset;
                let val = [
                    ((((real_val) >> 8) << 8) >>> 8) & 0xFF,
                    (((real_val) << 8) >>> 8) & 0xFF,
                ];
                val[0] = ~((~val[0]) | dynamic_2b_preamble);
                
                return val;
            } else if (size == 4) {
                const real_val = this.real_value + dynamic_4b_pos_offset;
                let val = [
                    ((((real_val) >> 24) << 8) >>> 8) & 0xFF,
                    ((((real_val) >> 16) << 8) >>> 8) & 0xFF,
                    ((((real_val) >> 8) << 8) >>> 8) & 0xFF,
                    (((real_val) << 8) >>> 8) & 0xFF,
                ];

                val[0] = ~((~val[0]) | dynamic_4b_preamble);

                return val;
            } else {
                throw 'Invalid size';
            }
        }
    }

    get_serialized_length() {
        return (
            this.real_value > dynamic_4b_max || this.real_value < dynamic_4b_min ? 8 :
            this.real_value > dynamic_2b_max || this.real_value < dynamic_2b_min ? 4 :
            this.real_value > dynamic_1b_max || this.real_value < dynamic_1b_min ? 2 :
            1
        );
    }
}

export default dynamic_value;
