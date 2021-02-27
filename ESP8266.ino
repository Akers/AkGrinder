#include <U8g2lib.h>
#include <stdlib.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

// 初始化屏幕驱动
const uint8_t DISPLAY_WIDTH = 128;
const uint8_t DISPLAY_HEIGH = 64;
const uint8_t PIN_SCL = 4;
const uint8_t PIN_SDA = 5;

// U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0,PIN_SCL,PIN_SDA,U8X8_PIN_NONE);
// U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, SCL, SDA, /* reset=*/U8X8_PIN_NONE);
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/U8X8_PIN_NONE);

// 菜单定义
const uint8_t MENU_HOME = 0;
const uint8_t MENU_SETTING = 4;
// 当前菜单
uint8_t curMenuIdx = 0;

// 按钮GPIO
// D1 --> GPIO1
const uint8_t BTN_1 = 14;
// D2 --> GPIO16
const uint8_t BTN_2 = 12;
// D5 --> GPIO14
const uint8_t BTN_3 = 13;
// D6 --> GPIO12
const uint8_t BTN_4 = 15;

// 界面重绘
uint8_t menu_redraw_required = 1;

// 重量（g）
float curWeight = 9999.99;

// 定义按钮点击状态
const uint8_t BTN_STATUS_PRESSED = 1;
const uint8_t BTN_STATUS_RELEASE = 0;
// 记录按钮状态
uint8_t btn1State = BTN_STATUS_RELEASE;
uint8_t btn2State = BTN_STATUS_RELEASE;
uint8_t btn3State = BTN_STATUS_RELEASE;
uint8_t btn4State = BTN_STATUS_RELEASE;
uint8_t btn1State_l = BTN_STATUS_RELEASE;
uint8_t btn2State_l = BTN_STATUS_RELEASE;
uint8_t btn3State_l = BTN_STATUS_RELEASE;
uint8_t btn4State_l = BTN_STATUS_RELEASE;

// 欢迎屏幕
void hello()
{
  u8g2.setFontDirection(0); //设置字体方向
  u8g2.firstPage();
  do
  {
    /* all graphics commands have to appear within the loop body. */
    u8g2.setFont(u8g2_font_ncenB14_tr);
    u8g2.drawStr(12, 40, "AkGrinder");
  } while (u8g2.nextPage());
  // for (int x = 0; x <= 30; x++)
  // {
  //   }
}

// 在当前屏幕绘制重量
void drawWeight()
{
  int pos_x = 30;
  int char_width = 6;
  // 格式化重量，保留小数点后2位
  char weightStr[10];
  sprintf(weightStr, "%.2f", curWeight);
  // 设置重量字体
  // u8g2.setFont(u8g2_font_VCR_OSD_tn);
  u8g2.setFont(u8g2_font_profont29_tn);
  int weightStrWidth = u8g2.getStrWidth(weightStr);
  pos_x = DISPLAY_WIDTH / 2 - weightStrWidth / 2;
  u8g2.setCursor(pos_x, 40);
  u8g2.print(weightStr);
  u8g2.setFont(u8g2_font_pcsenior_8f);
  u8g2.setCursor(pos_x + weightStrWidth + 3, 40);
  u8g2.print("g");
}

// 绘制首页菜单
void drawMenu(uint8_t s)
{
  switch (s)
  {
  case MENU_HOME:
    u8g2.setFont(u8g2_font_wqy12_t_gb2312);
    u8g2.setCursor(1, 63);
    u8g2.print("预设1");
    u8g2.setCursor(36, 63);
    u8g2.print("预设2");
    u8g2.setCursor(75, 63);
    u8g2.print("手动");
    u8g2.setCursor(106, 63);
    u8g2.print("菜单");
    break;
  case MENU_SETTING:
    u8g2.clear();
    do
    {
      u8g2.setFont(u8g2_font_wqy12_t_gb2312);
      u8g2.setCursor(5, 20);
      u8g2.print("模式配置");
      u8g2.setCursor(5, 40);
      u8g2.print("预设管理");
      u8g2.setFont(u8g2_font_open_iconic_arrow_1x_t);
      u8g2.drawGlyph(1, 63, 0x0050);
      u8g2.drawGlyph(41, 63, 0x0053);
      u8g2.drawGlyph(103, 63, 0x0051);
      u8g2.setFont(u8g2_font_open_iconic_check_1x_t);
      u8g2.drawGlyph(75, 63, 0x0040);
    } while (u8g2.nextPage());
    break;
  default:
    u8g2.setFont(u8g2_font_wqy12_t_gb2312);
    u8g2.setCursor(1, 63);
    u8g2.print("预设1");
    u8g2.setCursor(41, 63);
    u8g2.print("预设2");
    u8g2.setCursor(75, 63);
    u8g2.print("手动");
    u8g2.setCursor(103, 63);
    u8g2.print("菜单");
  }
}

// 绘制首页
void drawHome()
{
  u8g2.setFontDirection(0); //设置字体方向
  u8g2.firstPage();
  do
  {
    /* all graphics commands have to appear within the loop body. */
    u8g2.setFont(u8g2_font_wqy12_t_gb2312);
    size_t statusPos = 5;
    u8g2.setCursor(statusPos, 9);
    u8g2.print("模式");
    statusPos += 12 * 2;
    u8g2.setCursor(statusPos + 2, 9);
    u8g2.print(":");
    statusPos += 6;
    u8g2.setCursor(statusPos, 9);
    u8g2.print("智能");
    statusPos += 12 * 2;
    drawWeight();
    drawMenu(MENU_HOME);
  } while (u8g2.nextPage());
}

// 重绘UI
void re_draw_ui()
{
  if (1 == menu_redraw_required)
  {
    switch (curMenuIdx)
    {
    case MENU_HOME:
      drawHome();
      break;
    case MENU_SETTING:
      drawMenu(MENU_SETTING);
      break;
    }

    menu_redraw_required = 0;
  }
}

// 返回上一级菜单
void go_back()
{
  curMenuIdx = curMenuIdx / 10;
  menu_redraw_required = 1;
  re_draw_ui();
}

// 按钮4点击处理
void btn4_clicked()
{
  Serial.printf("btn4_clicked: curMenuIdx: %d, menu_redraw_required: %d \n", curMenuIdx, menu_redraw_required);
  switch (curMenuIdx)
  {
  case MENU_HOME:
    curMenuIdx = MENU_SETTING;
    menu_redraw_required = 1;
    re_draw_ui();
    break;
  default:
    go_back();
    break;
  }
}

// the setup function runs once when you press reset or power the board
void setup()
{
  // initialize digital pin LED_BUILTIN as an output.
  // pinMode(PIN_SCL, OUTPUT);
  // pinMode(PIN_SDA, OUTPUT);
  pinMode(BTN_1, INPUT);
  digitalWrite(BTN_1, LOW);
  pinMode(BTN_2, INPUT);
  digitalWrite(BTN_2, LOW);
  pinMode(BTN_3, INPUT);
  digitalWrite(BTN_3, LOW);
  pinMode(BTN_4, INPUT);
  digitalWrite(BTN_4, LOW);
  pinMode(LED_BUILTIN, OUTPUT);
  // 调试串口
  Serial.begin(115200);
  Serial.println();
  u8g2.begin();
  u8g2.enableUTF8Print(); // UTF8
  hello();
  delay(900);
  re_draw_ui();
  // Serial.printf("SCL: %d, SDA: %d", SCL, SDA);
  digitalWrite(LED_BUILTIN, HIGH); // turn the LED on (HIGH is the voltage level)                  // wait for a second
}

// the loop function runs over and over again forever
void loop()
{
  btn1State = digitalRead(BTN_1);
  if (btn1State_l != btn1State)
  {
    btn1State_l = btn1State;
    if (btn1State == 1)
    {
      digitalWrite(LED_BUILTIN, HIGH);
      Serial.printf("按钮状态： BTN_1: %d, BTN_2: %d, BTN_3: %d, BTN_4: %d \n", btn1State, btn2State, btn3State, btn4State);
      Serial.printf("菜单按钮： %d  \n", curMenuIdx);
      curMenuIdx = 1;
      menu_redraw_required = 1;
    }
  }
  btn2State = digitalRead(BTN_2);
  if (btn2State_l != btn2State)
  {
    Serial.printf("按钮状态： BTN_1: %d, BTN_2: %d, BTN_3: %d, BTN_4: %d \n", btn1State, btn2State, btn3State, btn4State);
    Serial.printf("菜单按钮： %d  \n", curMenuIdx);
    btn2State_l = btn2State;
    if (btn2State == 1)
    {
      curMenuIdx = 2;
      menu_redraw_required = 1;
    }
  }
  btn3State = digitalRead(BTN_3);
  if (btn3State_l != btn3State)
  {
    digitalWrite(LED_BUILTIN, HIGH);
    Serial.printf("按钮状态： BTN_1: %d, BTN_2: %d, BTN_3: %d, BTN_4: %d \n", btn1State, btn2State, btn3State, btn4State);
    Serial.printf("菜单按钮： %d  \n", curMenuIdx);
    btn3State_l = btn3State;
    if (btn3State == 1)
    {
      curMenuIdx = 3;
      menu_redraw_required = 1;
    }
  }
  btn4State = digitalRead(BTN_4);
  if (btn4State == 1)
  {
    digitalWrite(LED_BUILTIN, LOW);
  }

  if (btn4State != btn4State_l) {
    Serial.printf("按钮状态： btn4State_l: %d, btn4State: %d \n", btn4State_l, btn4State);
    if (btn4State_l == 1 && btn4State == 0)
    {
      digitalWrite(LED_BUILTIN, HIGH);
      btn4_clicked();
    }
    btn4State_l = btn4State;
  }
  
}
