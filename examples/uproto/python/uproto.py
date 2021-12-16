from uutils import dynamic_value, dynamic_get_length
import serial
import time

uproto_message_start = 0b00110011
uproto_message_end = 0b11001100

def bin8(num):
    return '0b' + format(num, '08b')

class uproto_message:
    def __init__(self):
        self.message_properties: int = 0
        self.resource_id: dynamic_value = dynamic_value()
        self.payload_length: dynamic_value = dynamic_value()
        self.payload: [int] = []
        self.checksum: int = 0

    def serialize(self):
        result = bytearray()
        result.append(uproto_message_start)
        result.append(self.message_properties)
        result.extend(self.resource_id.serialize())
        self.payload_length.real_value = len(self.payload)
        result.extend(self.payload_length.serialize())
        result.extend(self.payload)

        if ((self.message_properties & 0b00000010) == 0):
            self.checksum = sum(result) & 0xFF
            result.append(self.checksum)

        result.append(uproto_message_end)

        return result

    def calculate_checksum(self):
        return (uproto_message_start +
            self.message_properties +
            sum(self.resource_id.serialize()) +
            sum(self.payload_length.serialize()) +
            sum(self.payload)
        ) & 0xFF

    def __str__(self):
        return ('[uproto_message]\n  message_properties=' + bin8(self.message_properties) +
            '\n  resource_id=' + hex(self.resource_id.real_value) +
            '\n  payload_length=' + str(self.payload_length.real_value) +
            '\n  payload=' + ','.join([(hex(p)) for p in self.payload]) + 
            '\n  checksum=' + hex(self.checksum) +
            '\n  checksum(calculated)=' + hex(self.calculate_checksum()))

class dynamic_parsing_state:
    def __init__(self, required: int):
        self.required = required
        self.cached = 0
        self.cache = []

class uproto_runtime:
    parser_state = 0
    ready_message: uproto_message = None
    parsing_message: uproto_message = None

    resource_id_state: dynamic_parsing_state = None
    payload_length_state: dynamic_parsing_state = None

    def __init__(self):
        self.serialPort = serial.Serial()
        self.serialPort.port = "COM3"
        self.serialPort.baudrate = 115200
        self.serialPort.baudrate = 9600
        self.serialPort.timeout = 2
        self.serialPort.setDTR(False)
        self.serialPort.open()

        #time.sleep(3)

    def write_message_to_serial(self, message: uproto_message):
        if (not self.serialPort.is_open):
            print('Serial port is not open\n')
            return

        data = message.serialize()
        print('Sending packet', data.hex('-'))
        self.serialPort.write(data)
        # for d in data:
        #     value = d & 0xFF
        #     print('Sending: ', hex(value))
        #     self.serialPort.write(value)
        #     time.sleep(0.01)


        #time.sleep(0.1)

    def send_request_await_response(self, request: uproto_message):
        if (not self.serialPort.is_open):
            print('Serial port is not open\n')
            return

        self.ready_message = None

        data = request.serialize()
        print('Sending packet', data.hex('-'))
        self.serialPort.write(data)

        #time.sleep(0.1)

        response = None
        while response == None:
            self.execute()
            if (self.ready_message):
                response = self.ready_message
                self.ready_message = None

        return response

    def execute(self):
        while self.serialPort.in_waiting > 0:
            input = self.serialPort.read(1)[0] & 0xFF
            print('Receiving: ', hex(input))
            self.parse_data(input)

    def parse_data(self, data: int):
        if (self.parser_state == 0):
            # Start
            if (data == uproto_message_start):
                self.parsing_message = uproto_message()
                self.parser_state = 1

        elif (self.parser_state == 1):
            # Message properties
            self.parsing_message.message_properties = data
            self.parser_state = 2

        elif (self.parser_state == 2):
            # Resource id
            if self.resource_id_state == None:
                self.resource_id_state = dynamic_parsing_state(dynamic_get_length(data))

                if self.resource_id_state.required == 1:
                    self.parsing_message.resource_id.parse_dynamic([data])
                    self.resource_id_state = None
                    self.parser_state = 3
                    return

            self.resource_id_state.cache[self.resource_id_state.cached] = data
            self.resource_id_state.cached += 1

            if self.resource_id_state.cached == self.resource_id_state.required:
                self.parsing_message.resource_id.parse_dynamic(self.resource_id_state.cache)
                self.resource_id_state = None
                self.parser_state = 3

        elif (self.parser_state == 3):
            # Payload length
            if self.payload_length_state == None:
                self.payload_length_state = dynamic_parsing_state(dynamic_get_length(data))

            if self.payload_length_state.required == 1:
                self.parsing_message.payload_length.parse_dynamic([data])
                self.payload_length_state = None
                self.parser_state = 4
                return

            self.payload_length_state.cache[self.payload_length_state.cached] = data
            self.payload_length_state.cached += 1

            if self.payload_length_state.cached == self.payload_length_state.required:
                self.parsing_message.payload_length.parse_dynamic(self.payload_length_state.cache)
                self.payload_length_state = None
                self.parser_state = 4

        elif (self.parser_state == 4):
            # Payload
            if self.parsing_message.payload_length.real_value == 0:
                self.parser_state = 5
                self.parse_data(data)
                return

            self.parsing_message.payload.append(data)

            if len(self.parsing_message.payload) >= self.parsing_message.payload_length.real_value:
                self.parser_state = 5

        elif (self.parser_state == 5):
            # Checksum
                if (self.parsing_message.message_properties & 0b00000010) == 0b00000010:
                    self.parser_state = 6
                    self.parse_data(data)
                    return
                    
                self.parsing_message.checksum = data
                self.parser_state = 6

        elif (self.parser_state == 6):
            # Message end
            if data != uproto_message_end:
                print('Invalid message end detected', hex(data))

            self.ready_message = self.parsing_message
            self.parsing_message = None
            self.parser_state = 0
