import datetime
import serial

SERIAL_PORT = 'COM9'
BAUDRATE = 115200


def read_line(line):
    line = line.decode('ascii')
    line = line.strip()
    return line


def set_date_time():
    print(now.month)
    print(now.day)
    print(now.weekday() + 1)
    print(now.hour)
    print(now.minute)
    print(now.second)


def main():
    print('Arduino D1302 terminal example')

    ser = serial.Serial()
    ser.baudrate = int(BAUDRATE)
    ser.bytesize = 8
    ser.stopbits = 1
    ser.parity = serial.PARITY_NONE
    ser.timeout = 0.01
    ser.port = SERIAL_PORT
    ser.open()

    now = None

    while 1:
        line = read_line(ser.readline())
        if line:
            print(line)

            if line.find('Press S to set date and time') == 0:
                ser.write(b's\n')
            elif line.find('Year [00-99]:') == 0:
                now_last = datetime.datetime.now()
                while 1:
                    now = datetime.datetime.now()
                    if now_last != now:
                        break
                now += datetime.timedelta(0, 2)
                ser.write(str.encode('{}\n'.format(now.year - 2000)))
            elif line.find('Month [1-12]:') == 0:
                ser.write(str.encode('{}\n'.format(now.month)))
            elif line.find('Day month [1-31]:') == 0:
                ser.write(str.encode('{}\n'.format(now.day)))
            elif line.find('Day of the week [1=Mon-7=Sun]:') == 0:
                ser.write(str.encode('{}\n'.format(now.weekday() + 1)))
            elif line.find('Hour [0-23]:') == 0:
                ser.write(str.encode('{}\n'.format(now.hour)))
            elif line.find('Minute [0-59]:') == 0:
                ser.write(str.encode('{}\n'.format(now.minute)))
            elif line.find('Second [0-59]:') == 0:
                ser.write(str.encode('{}\n'.format(now.second)))

if __name__ == '__main__':
    main()
