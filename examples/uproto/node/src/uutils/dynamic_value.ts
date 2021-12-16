const preambles = {
    preamble_2b_mode: 1 << 15,
    preamble_4b_mode: (1 << 31) | (1 << 30),
    preamble_8b_mode: (1 << 63) | (1 << 62) | (1 << 61),
};

const offsets = {
    offset_2b_mode: 0x80,
    offset_4b_mode: 0x4080,
    offset_8b_mode: 0x20004080,
};

function dynamic_get_length(dynamic_first_byte: number) {
    return ((dynamic_first_byte & preambles.preamble_8b_mode) == preambles.preamble_8b_mode) ? 8 :
        ((dynamic_first_byte & preambles.preamble_4b_mode) == preambles.preamble_4b_mode) ? 4 :
        ((dynamic_first_byte & preambles.preamble_2b_mode) == preambles.preamble_2b_mode) ? 2 :
        1;
}

class dynamic_value {
    real_value: number;

    constructor(real_value = 0) {
        this.real_value = real_value;
    }

    parse_dynamic(dynamic_value: number[]) {
        let size = dynamic_get_length(dynamic_get_length[0]);

        if (size == 1) {
            this.real_value = dynamic_value[0];
        } else if (size == 2) {
            this.real_value = (
                (dynamic_value[0] ^ preambles.preamble_2b_mode) << 8 +
                dynamic_value[1] +
                offsets.offset_2b_mode
            );
        } else if (size == 4) {
            this.real_value = (
                (dynamic_value[0] ^ preambles.preamble_4b_mode) << 24 +
                dynamic_value[1] << 16 +
                dynamic_value[2] << 8 +
                dynamic_value[3] +
                offsets.offset_4b_mode
            );
        } else if (size == 8) {
            this.real_value = (
                (dynamic_value[0] ^ preambles.preamble_8b_mode) << 56 +
                dynamic_value[1] << 48 +
                dynamic_value[2] << 40 +
                dynamic_value[3] << 32 +
                dynamic_value[4] << 24 +
                dynamic_value[5] << 16 +
                dynamic_value[6] << 8 +
                dynamic_value[7] +
                offsets.offset_2b_mode
            );
        } else {
            throw 'Invalid length';
        }

        return size;
    }

    serialize() {
        let size = this.get_serialized_length();

        if (size == 1) {
            return [this.real_value];
        } else if (size == 2) {
            let dynamic_value = (this.real_value - offsets.offset_2b_mode) | preambles.preamble_2b_mode;

            return [
                (dynamic_value >> 8) & 0xFF,
                dynamic_value & 0xFF,
            ];
        } else if (size == 4) {
            let dynamic_value = (this.real_value - offsets.offset_4b_mode) | preambles.preamble_4b_mode;

            return [
                (dynamic_value >> 24) & 0xFF,
                (dynamic_value >> 16) & 0xFF,
                (dynamic_value >> 8) & 0xFF,
                dynamic_value & 0xFF,
            ];
        } else if (size == 8) {
            let dynamic_value = (this.real_value - offsets.offset_8b_mode) | preambles.preamble_8b_mode;

            return [
                (dynamic_value >> 56) & 0xFF,
                (dynamic_value >> 48) & 0xFF,
                (dynamic_value >> 40) & 0xFF,
                (dynamic_value >> 32) & 0xFF,
                (dynamic_value >> 24) & 0xFF,
                (dynamic_value >> 16) & 0xFF,
                (dynamic_value >> 8) & 0xFF,
                dynamic_value & 0xFF,
            ];
        } else {
            return [0];
        }
    }

    get_serialized_length() {
        return (this.real_value >= offsets.offset_8b_mode) ? 8 :
            (this.real_value >= offsets.offset_4b_mode) ? 4 :
            (this.real_value >= offsets.offset_2b_mode) ? 2 :
            1;
    }
}

export default dynamic_value;
export {
    dynamic_get_length,
}
