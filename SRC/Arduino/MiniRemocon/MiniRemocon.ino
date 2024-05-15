#include <LovyanGFX.hpp>      // lovyanGFXのヘッダを準備
#include <Wire.h>

//--------------------
//  Const / Define
//--------------------
#define SDA_PIN (26)          // SDA  M5ATOM
#define SCL_PIN (32)          // SCL  M5ATOM

//--  PCA9557 registers
#define REG_INPUT (0x00)
#define REG_OUTPUT (0x01)
#define REG_INVERSION (0x02)
#define REG_CONFIG (0x03)
#define PCA9557_ADDR (0x18)

#define IOMODE (0xF0)       // b7-0:SW4,SW3,SW2,SW1,LD3,LD2,LD1,None
#define OUT_DEFAULT (0x0F)  // ALL OFF

#define SW_ON (1)
#define SW_OFF (0)
#define LED_ON (0)
#define LED_OFF (1)

//--------------------
//  Private Variable
//--------------------
//--  PANEL
char ldPGre = LED_OFF;
char ldPYel = LED_OFF;
char ldPRed = LED_OFF;

char swP[4] = { SW_OFF, SW_OFF, SW_OFF, SW_OFF };
char swPb[4] = { SW_OFF, SW_OFF, SW_OFF, SW_OFF };
char swPL[4] = { SW_OFF, SW_OFF, SW_OFF, SW_OFF };
char swPLb[4] = { SW_OFF, SW_OFF, SW_OFF, SW_OFF };
uint8_t wInp[4] = { 0, 0, 0, 0 };
uint8_t wInpL[4] = { 0, 0, 0, 0 };

//  Variable
uint lCount = 0;

//--  LovyanGFX
class LGFX_SSD1306 : public lgfx::LGFX_Device {
  lgfx::Panel_SSD1306 _panel_instance;    // SSD1306を使用する場合
  lgfx::Bus_I2C _bus_instance;            // I2Cバスのインスタンス (ESP32のみ)

public:                                   // Constructor
  LGFX_SSD1306() {                        // コンストラクタ名はクラス名に合わせてLGFXからLGFX_SSD1306に変更してます。（クラス名と同じにする）
    {                                     // バス制御の設定を行います。
      auto cfg = _bus_instance.config();  // I2Cバス設定用の構造体を取得します。
      cfg.i2c_port = 0;                   // 使用するI2Cポートを選択 (0 or 1)
      cfg.freq_write = 400000;            // 送信時のクロック
      cfg.freq_read = 400000;             // 受信時のクロック
      cfg.pin_sda = SDA_PIN;              // SDAを接続しているピン番号
      cfg.pin_scl = SCL_PIN;              // SCLを接続しているピン番号
      cfg.i2c_addr = 0x3C;                // I2Cデバイスのアドレス

      _bus_instance.config(cfg);               // 設定値をバスに反映します。
      _panel_instance.setBus(&_bus_instance);  // バスをパネルにセットします。
    }
    {  // 表示パネル制御の設定を行います。
      // 以下の設定値はパネル毎に一般的な初期値が設定されていますので、不明な項目はコメントアウトして試してみてください。
      auto cfg = _panel_instance.config();  // 表示パネル設定用の構造体を取得します。
      cfg.panel_width = 128;                // 実際に表示可能な幅
      cfg.panel_height = 64;                // 実際に表示可能な高さ
      cfg.offset_rotation = 2;              // 回転方向の値のオフセット 0~7 (4~7は上下反転)
      // cfg.invert          = true;             // パネルの明暗が反転してしまう場合 trueに設定
      _panel_instance.config(cfg);  // 設定をパネルに反映
    }
    setPanel(&_panel_instance);  // 使用するパネルをセットします。
  }
};
static LGFX_SSD1306 lcd;          // LGFX_SSD1306のインスタンス（クラスLGFX_SSD1306を使ってlcdでいろいろできるようにする）を作成
static LGFX_Sprite canvas(&lcd);  // スプライトを使うためのLGFX_Spriteのインスタンスを作成

//----------------------------------------
//-- SetUP
void setup() {
  //--  Serial Setting
  Serial.begin(115200);
  Serial.print("Setup\n");

  //--  i2c
  pinMode(SDA_PIN, INPUT_PULLUP);        // SDA
  pinMode(SCL_PIN, INPUT_PULLUP);        // SCL
  Wire.begin(SDA_PIN, SCL_PIN, 400000);  // 400k

  //--  PANEL
  PCA9557_init();  // PCA9557

  lcd.init();                                      // 表示器初期化
  canvas.setTextWrap(false);                       // 右端到達時のカーソル折り返しを禁止(true)で許可
  canvas.createSprite(lcd.width(), lcd.height());  // スプライト用の仮想画面を画面幅を取得して準備

  show_Test();
}

//----------------------------------------
//--  loop
void loop() {
  lCount++;

  //--  INPUT
  PCA9557_INP();

  //--  LED
  ldPGre = LED_OFF;
  ldPYel = LED_OFF;
  ldPRed = LED_OFF;

  if (swP[0] == SW_ON) {
    ldPGre = LED_ON;
  }
  if (swP[1] == SW_ON) {
    ldPYel = LED_ON;
  }
  if (swP[2] == SW_ON) {
    ldPRed = LED_ON;
  }
  if (swP[3] == SW_ON) {
    ldPGre = LED_ON;
    ldPYel = LED_ON;
    ldPRed = LED_ON;
  }

  PCA9557_OUT();

  //--  OLED
  show_Main();
}

//----  OLED
//--  Main
void show_Main() {
  canvas.fillScreen(TFT_BLACK);
  canvas.setTextSize(1.0);
  canvas.setFont(&Font2);

  canvas.setCursor(24, 0);
  canvas.print("Mini Remocon");
  canvas.setCursor( 8, 14);
  canvas.print("www.mii-system.com");

  if (swP[0] == SW_ON) {
    canvas.setCursor(10, 32);
    canvas.print("A");
  }
  if (swP[1] == SW_ON) {
    canvas.setCursor(40, 32);
    canvas.print("B");
  }
  if (swP[2] == SW_ON) {
    canvas.setCursor(70, 32);
    canvas.print("C");
  }
  if (swP[3] == SW_ON) {
    canvas.setCursor(100, 32);
    canvas.print("D");
  }

  if (swPL[0] == SW_ON) {
    canvas.setCursor(10, 32);
    canvas.print("AL");
  }
  if (swPL[1] == SW_ON) {
    canvas.setCursor(40, 32);
    canvas.print("BL");
  }
  if (swPL[2] == SW_ON) {
    canvas.setCursor(70, 32);
    canvas.print("CL");
  }
  if (swPL[3] == SW_ON) {
    canvas.setCursor(100, 32);
    canvas.print("DL");
  }

  canvas.setCursor(80, 48);
  canvas.printf("%05d", lCount);
  canvas.pushSprite(0, 0);
  lcd.endWrite();  // SPIバス解放
}

//--  Test
void show_Test() {
  //  TITLE
  canvas.fillScreen(TFT_BLACK);
  canvas.setFont(&Font2);
  canvas.setCursor(0, 0);
  canvas.println("Mini Remocon");
  canvas.println();
  canvas.println(" presented by");
  canvas.println("  Mii-system");
  canvas.pushSprite(0, 0);
  delay(2000);

  //  Font
  canvas.fillScreen(TFT_BLACK);
  canvas.setTextSize(1.0);
  canvas.setCursor(0, 0, &Font0);
  canvas.print("ABCDEFG");
  canvas.setCursor(0, 8, &Font2);
  canvas.print("ABCDEFG");
  canvas.setCursor(0, 24, &Font4);
  canvas.print("ABCDEFG");
  canvas.pushSprite(0, 0);
  delay(2000);

  //  Font7
  canvas.fillScreen(TFT_BLACK);  // 背景塗り潰し
  canvas.setTextSize(1.0);
  canvas.setCursor(0, 0, &Font7);
  for (int i = 0; i < 100; i++) {
    canvas.setCursor(0, 0);
    int w = (i * i);
    canvas.printf("%04d", w);
    canvas.pushSprite(0, 0);
  }
  int w = 9999;
  canvas.setCursor(0, 0);
  canvas.printf("%04d", w);
  canvas.pushSprite(0, 0);

  lcd.endWrite();  // SPIバス解放
  delay(500);
}

//----  PCA9557
//--  Init
void PCA9557_init() {
  Wire.beginTransmission(PCA9557_ADDR);
  Wire.write(REG_OUTPUT);
  Wire.write(OUT_DEFAULT);
  Wire.endTransmission();

  Wire.beginTransmission(PCA9557_ADDR);
  Wire.write(REG_CONFIG);
  Wire.write(IOMODE);
  Wire.endTransmission();
}

//--  Output
void PCA9557_OUT() {
  uint8_t w = 0;

  bitWrite(w, 1, ldPGre);
  bitWrite(w, 2, ldPYel);
  bitWrite(w, 3, ldPRed);

  Wire.beginTransmission(PCA9557_ADDR);
  Wire.write(REG_OUTPUT);
  Wire.write(w);
  Wire.endTransmission();
}

//--  Input
void PCA9557_INP() {
  uint8_t w = 0;

  Wire.beginTransmission(PCA9557_ADDR);
  Wire.write(REG_INPUT);
  Wire.endTransmission(false);

  Wire.requestFrom(PCA9557_ADDR, 1);  // Read 1 byte
  w = Wire.read();

  wInp[0] = wInp[0] * 2 + bitRead(w, 4);
  wInp[1] = wInp[1] * 2 + bitRead(w, 5);
  wInp[2] = wInp[2] * 2 + bitRead(w, 6);
  wInp[3] = wInp[3] * 2 + bitRead(w, 7);

  for (int i = 0; i < 4; i++) {  // 4 switchs
    if (wInp[i] != 0xFF) {
      swP[i] = SW_OFF;
      wInpL[i] = 0;
    }  // ALL OFF
    if (wInp[i] == 0xFF) {
      swP[i] = SW_ON;
      wInpL[i]++;
    }  // ALL ON
    if (wInpL[i] >= 20) {
      swPL[i] = SW_ON;
      wInpL[i]--;
    }  // Long ON
    else { swPL[i] = SW_OFF; }
  }
}
