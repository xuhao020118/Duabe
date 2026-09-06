#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SPI.h>
#include <SD.h>
#include <vector>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const int SD_CS = 10;

// Trạng thái sinh học
enum State {
  STATE_SLEEPING,
  STATE_HUNGRY,
  STATE_AWAKE
};

State currentState = STATE_AWAKE;
int batteryLevel = 90;

// Bộ nhớ từ vựng hoàn toàn trống lúc mới sinh, chỉ có chuỗi bập bẹ nguyên thủy
std::vector<String> learnedWords = {};
String rawBabble = "ậthaugwkwywiwyhe";

void drawFace(String expression) {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(15, 25);
  display.println(expression);
  display.display();
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("Lỗi OLED!");
    while(1);
  }

  drawFace("[ o _ o ]"); // Mặt ngơ ngác mới sinh

  // Khởi động thẻ nhớ và nạp lại những gì người thầy từng dạy
  if (SD.begin(SD_CS)) {
    File file = SD.open("/teacher_lessons.txt", FILE_READ);
    if (file) {
      while (file.available()) {
        String word = file.readStringUntil('\n');
        word.trim();
        if (word.length() > 0) {
          learnedWords.push_back(word);
        }
      }
      file.close();
      Serial.println("Đã khôi phục tri thức do thầy dạy từ thẻ nhớ.");
    } else {
      Serial.println("Bộ não hoàn toàn trắng tinh, chờ người thầy xuất hiện...");
    }
  }
}

void loop() {
  // Giả lập tụt pin theo thời gian
  batteryLevel -= 1;
  if (batteryLevel <= 0) batteryLevel = 100;

  if (batteryLevel < 10) {
    currentState = STATE_SLEEPING;
  } else if (batteryLevel < 25) {
    currentState = STATE_HUNGRY;
  } else {
    currentState = STATE_AWAKE;
  }

  // Hiển thị hành vi và khuôn mặt trên OLED
  if (currentState == STATE_SLEEPING) {
    drawFace("( - _ - )");
    Serial.println("AI: (Đang ngủ sâu)...");
    delay(4000);
    return;
  } 
  
  if (currentState == STATE_HUNGRY) {
    drawFace("(> _ <)");
    // Nếu đói mà chưa được dạy từ chỉ sự đói, nó chỉ biết ú ớ
    if (learnedWords.empty()) {
      Serial.println("AI (đói): ậ... ha...");
    } else {
      // Dùng từ đã học để gọi (hoặc kêu bập bẹ)
      String w = learnedWords[random(0, learnedWords.size())];
      Serial.print("AI (đói, gọi thầy): ");
      Serial.println(w);
    }
    delay(3000);
    return;
  }

  // Trạng thái thức: Nếu chưa có từ nào, nó chỉ nói linh tinh ngẫu nhiên
  if (learnedWords.empty()) {
    drawFace("[ o _ o ]");
    int idx = random(0, rawBabble.length());
    String randomSound = String(rawBabble[idx]) + String(rawBabble[random(0, rawBabble.length())]);
    Serial.print("AI (bập bẹ vô nghĩa): ");
    Serial.println(randomSound);
  } else {
    // Đã được thầy dạy, nó sẽ nói lại các từ do thầy truyền đạt
    drawFace("[ ^ _ ^ ]");
    String taughtWord = learnedWords[random(0, learnedWords.size())];
    Serial.print("AI (tập nói theo thầy): ");
    Serial.println(taughtWord);
  }

  // Vai trò của người thầy: Dạy nó từng từ một qua Serial (hoặc qua Mic sau này)
  if (Serial.available() > 0) {
    String lessonFromTeacher = Serial.readStringUntil('\n');
    lessonFromTeacher.trim();

    if (lessonFromTeacher.length() > 0) {
      drawFace("[ * _ * ]");
      Serial.print("--> Người thầy dạy từ mới: ");
      Serial.println(lessonFromTeacher);

      // Thêm vào bộ nhớ RAM
      learnedWords.push_back(lessonFromTeacher);

      // Lưu vĩnh viễn vào thẻ nhớ SD
      File file = SD.open("/teacher_lessons.txt", FILE_WRITE);
      if (file) {
        file.println(lessonFromTeacher);
        file.close();
        Serial.println("--> Đã khắc ghi bài học vào thẻ nhớ!");
      }
      
      delay(2000);
    }
  }

  delay(3000);
}
