import { dynamic_test_positive_values, dynamic_test_values } from '../test_core';
import dynamic_value, { split_dynamic_to_array, get_dynamic_size } from '../../src/uutils/dynamic_value';

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

        let val = new dynamic_value();
        val.parse_dynamic_array(dynamic_array);

        expect(val.real_value).toEqual(data.real_value);
    })

    it.each(dynamic_test_values)('parses dynamic', (data) => {
        let val = new dynamic_value();
        val.parse_dynamic(data.dynamic_value);

        expect(val.real_value).toEqual(data.real_value);
    })

    it.each(dynamic_test_values)('calculates required dynamic size for real value', (data) => {
        let val = new dynamic_value(data.real_value);

        expect(val.get_serialized_length()).toEqual(data.dynamic_size);
    })

    it.each(dynamic_test_values)('converts to dynamic', (data) => {
        const dynamic = split_dynamic_to_array(data.dynamic_value);

        let val = new dynamic_value(data.real_value);

        expect(val.serialize()).toEqual(dynamic);
    })

})
