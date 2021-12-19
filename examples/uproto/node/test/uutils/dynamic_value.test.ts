import { dynamic_test_values } from '../test_core';
import { split_dynamic_to_array, get_dynamic_size, dynamic_array_to_real, dynamic_to_real, real_to_dynamic_get_required_size, real_to_dynamic_array } from '../../src/uutils/dynamic_value';

describe('dynamic_value', () => {

    it.each(dynamic_test_values)('gets correct size from dynamic', (data) => {
        let size = get_dynamic_size(data.dynamic_value);

        expect(size).toBe(data.dynamic_size);
    })

    it.each(dynamic_test_values)('splits to dynamic correct array size', (data) => {
        let val = split_dynamic_to_array(data.dynamic_value);

        expect(val.length).toEqual(data.dynamic_size);
    })

    it.each(dynamic_test_values)('parses dynamic array', (data) => {
        const dynamic_array = split_dynamic_to_array(data.dynamic_value);

        expect(dynamic_array_to_real(dynamic_array)).toEqual(data.real_value);
    })

    it.each(dynamic_test_values)('parses dynamic', (data) => {
        expect(dynamic_to_real(data.dynamic_value)).toEqual(data.real_value);
    })

    it.each(dynamic_test_values)('calculates required dynamic size for real value', (data) => {
        expect(real_to_dynamic_get_required_size(data.real_value)).toEqual(data.dynamic_size);
    })

    it.each(dynamic_test_values)('converts to dynamic', (data) => {
        const dynamic = split_dynamic_to_array(data.dynamic_value);

        expect(real_to_dynamic_array(data.real_value)).toEqual(dynamic);
    })

    it.each(dynamic_test_values)('converts to dynamic and back', (data) => {
        const input = data.real_value;

        const dynamic = real_to_dynamic_array(input);
        const real = dynamic_array_to_real(dynamic);

        expect(real).toEqual(data.real_value);
    })

    it.each(dynamic_test_values)('converts to real and back', (data) => {
        const input = data.dynamic_value;
        const input_dynamic = split_dynamic_to_array(data.dynamic_value);

        const real = dynamic_to_real(input);
        const dynamic = real_to_dynamic_array(real);

        expect(dynamic).toEqual(input_dynamic);
    })

    it.each(dynamic_test_values)('converts multiple times', (data) => {
        const input_real = data.real_value;
        const input_dynamic_array = split_dynamic_to_array(data.dynamic_value);

        let real = input_real;
        for (let i = 0; i < 10; i++) {
            let dynamic1 = real_to_dynamic_array(real);
            let real1 = dynamic_array_to_real(dynamic1);
            let dynamic2 = real_to_dynamic_array(real1);
            let real2 = dynamic_array_to_real(dynamic2);
            let dynamic3 = real_to_dynamic_array(real2);

            expect(real2).toEqual(input_real);
            expect(dynamic3).toEqual(input_dynamic_array);

            real = real2;
        }
    })

})
