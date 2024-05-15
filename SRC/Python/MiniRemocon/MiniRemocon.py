import time
import board
import busio
import adafruit_ssd1306
from PIL import Image, ImageDraw, ImageFont
import smbus

# PCA9557 I2C address
PCA9557_ADDRESS = 0x18

# PCA9557 register addresses
INPUT_PORT = 0x00
OUTPUT_PORT = 0x01
POLARITY_INVERSION = 0x02
CONFIGURATION = 0x03

# Register bits
LD1 = 0x02
LD2 = 0x04
LD3 = 0x08

SW1 = 0x10
SW2 = 0x20
SW3 = 0x40
SW4 = 0x80

# Register initial values
OUTPUT_INIT_VALUE = 0x0F            # OUTPUT Default(1=OFF)
CONFIG_INIT_VALUE = 0xF0            # IIII OOOO

# I2CとSMBusの初期化
i2c = busio.I2C(board.SCL, board.SDA)
bus = smbus.SMBus(1)

# OLEDディスプレイの初期化
oled_width = 128
oled_height = 64
oled = adafruit_ssd1306.SSD1306_I2C(oled_width, oled_height, i2c)

# ディスプレイをクリア
oled.fill(0)
oled.show()

# 画像バッファの作成
image = Image.new("1", (oled_width, oled_height))

# 描画オブジェクトの作成
draw = ImageDraw.Draw(image)

# タイトルの描画
title_text = "MiniRemocon"
(title_width, title_height) = draw.textsize(title_text)
draw.text((20, 0), title_text, fill=255)

# バッファをOLEDに表示
oled.image(image)
oled.show()

def setup_pca9557():
    bus.write_byte_data(PCA9557_ADDRESS, CONFIGURATION, CONFIG_INIT_VALUE)
    bus.write_byte_data(PCA9557_ADDRESS, OUTPUT_PORT, OUTPUT_INIT_VALUE)

def read_input_port():
    return bus.read_byte_data(PCA9557_ADDRESS, INPUT_PORT)

def update_display(btn_states):
    draw.rectangle((0, 0, oled_width, oled_height), outline=0, fill=0)  # Clear display
    draw.text((20, 0), title_text, fill=255)  # Title
    draw.text((10,20), f"SW:", fill=255)
    for i, btn_state in enumerate(btn_states):
        if btn_state == 1:
            draw.text((30 + i * 15, 20), f"{i+1}", fill=255)  # Button states
    oled.image(image)
    oled.show()

def main():
    setup_pca9557()

    try:
        while True:
            input_state = read_input_port()
            btn_states = [(input_state >> (4+bit)) & 1 for bit in range(4)]
            print("SW:", " ".join(str(bit) for bit in btn_states))
            update_display(btn_states)

            leds = 0
            if input_state & SW1:
                leds += LD1
            if input_state & SW2:
                leds += LD2
            if input_state & SW3:
                leds += LD3
            if input_state & SW4:
                leds += (LD1 + LD2 + LD3)
            bus.write_byte_data(PCA9557_ADDRESS, OUTPUT_PORT, (leds ^ 0x0F))

            # time.sleep(0.1)
    except KeyboardInterrupt:
        print("\nFinish!")

if __name__ == '__main__':
    main()
