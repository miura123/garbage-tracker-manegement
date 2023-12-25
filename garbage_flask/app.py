from flask import Flask, render_template, request
import serial
import RPi.GPIO as GPIO
import time

app = Flask(__name__)

# GPIOピン番号を設定
GPIO_PIN = 17  # 例として17番のGPIOピンを使用

@app.route('/')
def home():
    return render_template('index.html')


@app.route('/button_clicked', methods=['POST','GET'])
def button_clicked():
    
    current_state = GPIO.input(GPIO_PIN)
    print(current_state)
    new_state = not current_state
    GPIO.output(GPIO_PIN, new_state)
    time.sleep(2)
    
    ser = serial.Serial('/dev/ttyUSB0',115200,timeout=None)
    serial_data = ser.readline().decode('shift-jis').strip()
    
    while(1):
        # シリアル通信からデータを受信
        ser = serial.Serial('/dev/ttyUSB1',115200,timeout=None)
        serial_data = ser.readline().decode('shift-jis').strip()
        time.sleep(0.2)


    
    if serial_data == "0":
        html_name = "index_moeru.html"
    elif serial_data == "1":
        html_name = "index_pet.html"
    else :
        html_name = "index.html"

    print(html_name)
    
    return render_template(html_name)
    
def GPIO_setup():
    # GPIOのセットアップ
    GPIO.setmode(GPIO.BCM)
    GPIO.setup(GPIO_PIN, GPIO.OUT)

if __name__ == '__main__':
    GPIO_setup()
    app.run(debug=False, host='0.0.0.0', port=5000)
