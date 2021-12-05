import serial
import time
import math
from uproto import uproto_runtime, uproto_message

# serialPort = serial.Serial(
#     port="COM3", baudrate=9600, timeout=2,
# )
# time.sleep(3)
# packet = bytearray()
# packet.append(0x35)
# packet.append(0x35)
# packet.append(0x35)
# packet.append(0x35)

# serialPort.write(packet)

# start = time.time()
# while time.time() - start < 10:
#     while serialPort.in_waiting > 0:
#         input = serialPort.read()
#         print(input)
#         #print('Receiving: ', hex(input & 0xFF))
#     time.sleep(0.1)


# runtime = uproto_runtime()

# message = uproto_message()
# message.message_properties |= 0b00000010
# message.resource_id.real_value = 56
# message.payload_length.real_value = 3
# message.payload = [0, 0, 0]

# runtime.write_message_to_serial(message)

# start = time.time()
# step = 0.1
# #direction = step
# fi = 0

# while 1:
#     runtime.execute()
#     #time.sleep(0.01)

#     #if time.time() - start > 0.2:
#     value = int(127*(math.sin(fi * math.pi) + 1))

#     message.payload[0] = value
#     message.payload[1] = value
#     message.payload[2] = value

#     runtime.write_message_to_serial(message)

#     fi += step
#     if fi > 10:
#         fi -= math.pi
#     # if step >= 150:
#     #     direction = -step
#     # elif message.payload[2] <= 50:
#     #     direction = step

#     #time.sleep(0.00001)







runtime = uproto_runtime()

message = uproto_message()
message.message_properties |= 0b00000010
message.resource_id.real_value = 55
message.payload = [
    0x01,
    11,
    255,
    0, 100
]
message.payload_length.real_value = len(message.payload)

while 1:
    message.payload[1] = 9
    message.payload[2] = 255
    runtime.write_message_to_serial(message)
    message.payload[1] = 10
    runtime.write_message_to_serial(message)
    message.payload[1] = 11
    runtime.write_message_to_serial(message)
    time.sleep(1)

    message.payload[1] = 9
    message.payload[2] = 0
    runtime.write_message_to_serial(message)
    message.payload[1] = 10
    runtime.write_message_to_serial(message)
    message.payload[1] = 11
    runtime.write_message_to_serial(message)
    time.sleep(1)

