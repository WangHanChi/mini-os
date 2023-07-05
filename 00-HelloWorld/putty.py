import serial
import sys

sys.argv.pop(0)
argv = sys.argv
argc = len(argv)

ser = serial.Serial(str(argv[0]), int(argv[1]))

def read_serial():
    return ser.readline().decode('utf-8').rstrip()

def write_serial(data):
    ser.write(data)

try:
    while True:
        # 讀取 serial 資料
        received_data = read_serial()
        if received_data is not None:
            print(received_data)

            while True:
                data_to_send = sys.stdin.buffer.read(1)
                write_serial(data_to_send)
                if data_to_send == b'\n':
                    print(read_serial())
                    break

except KeyboardInterrupt:
    ser.close()    # 清除序列通訊物件
    print('\nserial close!')