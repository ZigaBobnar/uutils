preamble_2b_mode = 1 << 15
preamble_4b_mode = (1 << 31) | (1 << 30)
preamble_8b_mode = (1 << 63) | (1 << 62) | (1 << 61)

offset_2b_mode = 0x80
offset_4b_mode = 0x4080
offset_8b_mode = 0x20004080

uint8_mask = 0xFF

def dynamic_get_length(dynamic_first_byte: int):
    if ((dynamic_first_byte & preamble_8b_mode) == preamble_8b_mode):
        return 8
    elif ((dynamic_first_byte & preamble_4b_mode) == preamble_4b_mode):
        return 4
    elif ((dynamic_first_byte & preamble_2b_mode) == preamble_2b_mode):
        return 2
    else:
        return 1

class dynamic_value:
    def __init__(self, real_value = 0):
        self.real_value: int = real_value

    def parse_dynamic(self, dynamic_val: [int]):
        size = dynamic_get_length(dynamic_val[0])

        if (size == 1):
            self.real_value = dynamic_val[0]
        elif (size == 2):
            self.real_value = (
                (dynamic_val[0] ^ preamble_2b_mode) << 8 +
                dynamic_val[1] +
                offset_2b_mode
            )
        elif (size == 4):
            self.real_value = (
                (dynamic_val[0] ^ preamble_4b_mode) << 24 +
                dynamic_val[1] << 16 +
                dynamic_val[2] << 8 +
                dynamic_val[3] +
                offset_4b_mode
            )
        elif (size == 8):
            self.real_value = (
                (dynamic_val[0] ^ preamble_8b_mode) << 56 +
                dynamic_val[1] << 48 +
                dynamic_val[2] << 40 +
                dynamic_val[3] << 32 +
                dynamic_val[4] << 24 +
                dynamic_val[5] << 16 +
                dynamic_val[6] << 8 +
                dynamic_val[7] +
                offset_8b_mode
                )
        else:
            raise 'Invalid length'

        return size

    def serialize(self):
        size = self.get_serialized_length()

        if (size == 1):
            return [self.real_value]
        elif (size == 2):
            dynamic_value = (self.real_value - offset_2b_mode) | preamble_2b_mode

            return [
                (dynamic_value >> 8) & uint8_mask,
                dynamic_value & uint8_mask
            ]
        elif (size == 4):
            dynamic_value = (self.real_value - offset_4b_mode) | preamble_4b_mode

            return [
                (dynamic_value >> 24) & uint8_mask,
                (dynamic_value >> 16) & uint8_mask,
                (dynamic_value >> 8) & uint8_mask,
                dynamic_value & uint8_mask
            ]
        elif (size == 8):
            dynamic_value = (self.real_value - offset_8b_mode) | preamble_8b_mode

            return [
                (dynamic_value >> 56) & uint8_mask,
                (dynamic_value >> 48) & uint8_mask,
                (dynamic_value >> 40) & uint8_mask,
                (dynamic_value >> 32) & uint8_mask,
                (dynamic_value >> 24) & uint8_mask,
                (dynamic_value >> 16) & uint8_mask,
                (dynamic_value >> 8) & uint8_mask,
                dynamic_value & uint8_mask
            ]

        return [0]

    def get_serialized_length(self):
        if (self.real_value >= offset_8b_mode):
            return 8
        elif (self.real_value >= offset_4b_mode):
            return 4
        elif (self.real_value >= offset_2b_mode):
            return 2
        else:
            return 1
