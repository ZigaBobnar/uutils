class dynamic_test_data {
    constructor(
        public real_value: number,
        public dynamic_value: number,
        public dynamic_size: number) {}
}

const dynamic_test_values = [
    new dynamic_test_data(0x00, 0x00, 1),
    new dynamic_test_data(0x01, 0x01, 1),
    new dynamic_test_data(0x15, 0x15, 1),
    new dynamic_test_data(0x3F, 0x3F, 1),
    new dynamic_test_data(-1, 0xFF, 1),
    new dynamic_test_data(-64, 0xC0, 1),

    new dynamic_test_data(0x0040, 0x4000, 2),
    new dynamic_test_data(0x1595, 0x5555, 2),
    new dynamic_test_data(0x203F, 0x5FFF, 2),
    new dynamic_test_data(-65, 0xBFFF, 2),
    new dynamic_test_data(-5526, 0xAAAA, 2),
    new dynamic_test_data(-8256, 0xA000, 2),

    new dynamic_test_data(0x00002040, 0x60000000, 4),
    new dynamic_test_data(0x0AAACAEA, 0x6AAAAAAA, 4),
    new dynamic_test_data(0x1000203F, 0x6FFFFFFF, 4),
    new dynamic_test_data(-8257, 0x9FFFFFFF, 4),
    new dynamic_test_data(-178965227, 0x95555555, 4),
    new dynamic_test_data(-268443712, 0x90000000, 4),
];

const dynamic_test_positive_values = dynamic_test_values.filter(i => i.real_value >= 0);

class checksum_test_data {
    constructor(public data: number[], public result: number) {}
}

const checksum_test_values = [
    new checksum_test_data([ 0 ], 0),
    new checksum_test_data([ 0, 1, 2, 3, 4, 5, 6, 7 ], 28),
    new checksum_test_data([ 1, 2, 3, 4, 5 ], 15),
    new checksum_test_data([ 1 ], 1),
    new checksum_test_data([ 255 ], 255),
    new checksum_test_data([ 245, 255, 134, 151 ], 17),
    new checksum_test_data([ 127, 128 ], 255),
];

class payload_test_data {
    constructor(public payload: number[]) {}
}

const payload_test_values = [
    new payload_test_data([ 0x00, 0x00 ]),
];

export {
    dynamic_test_data,
    dynamic_test_values,
    dynamic_test_positive_values,
    checksum_test_data,
    checksum_test_values,
    payload_test_data,
    payload_test_values,
};
