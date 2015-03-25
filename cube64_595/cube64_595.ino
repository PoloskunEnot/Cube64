#define MAX_ANODE 16
#define MAX_CATHODE 4

byte cathode[MAX_CATHODE] = {9, 10, 11, 12}; // пины на катоды (слои)
byte state[MAX_CATHODE][MAX_ANODE] ;

//Пин подключен к DS входу 74HC595 
int dataPin = 5;
//Пин подключен к ST_CP входу 74HC595
int latchPin = 6;
//Пин подключен к SH_CP входу 74HC595
int clockPin = 7;

byte col, row; 
byte buf_count;
bool flag;
byte buff[256]; // буфер для приема сообщения от компа

// последовательный проход по всем светодиодам (col, row)
void add_count(){
  col ++;
  if (col >= MAX_ANODE) {
    col = 0;
    row ++;
    if (row >=MAX_CATHODE) {
      col = MAX_ANODE - 1; 
      row = MAX_CATHODE-1;
    }
  }
}

// динамическая индикация (послойная - в один момент времени горит один слой)
void showDisplay(){
  for (byte i = 0; i < MAX_CATHODE; i++){
    int data = 0; // 2х8=16 бит для записи состояния светодиодов на слое
    for (byte j=0; j< MAX_ANODE;   j++)  {
      // записали в data содержимое массива state в слое i
      bitWrite(data, j, state[i][j]); 
    }
     //Отключаем вывод на регистре
    digitalWrite(latchPin, LOW);
    // отправляем data в регистры
    shiftOut(dataPin, clockPin, MSBFIRST, (data>>8));
    shiftOut(dataPin, clockPin, MSBFIRST, data);
    // "защелкиваем" регистр, чтобы байт появился на его выходах
    digitalWrite(latchPin, HIGH); 
    // зажигаем соответствующий слой
    digitalWrite(cathode[i], HIGH);
    delayMicroseconds(1000);
    digitalWrite(cathode[i], LOW);
  }
}

void setup() {
  Serial.begin(9600);

  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);

  for (byte i=0; i< MAX_CATHODE; i++) {
    pinMode(cathode[i], OUTPUT);
    digitalWrite(cathode[i], LOW);
  }
  
  buf_count = 0;
  flag = false;
}

void loop() {

  while (Serial.available()) {     //если есть принятый символ,
    byte val = Serial.read();      //  то читаем его и сохраняем в val
    if (val == 'e') {  // сообщение заканчивается символом 'e'
      flag = true;
      break;
    }
    else buff[buf_count] = val; // побайтово сохраняем сообщение в буфере
    buf_count++;
  }
  
  if (flag) { // буфер содержит законченное сообщение
    row = 0; col = 0;
    // разворачиваем сообщение в массив state
    for (byte i = 0; i <buf_count; i++) {
      if (buff[i] == '0') {
        state[row][col] = LOW;
        add_count();
      } 
      if (buff[i] == '1') {               
        state[row][col] = HIGH;
        add_count();
      }
    }
    flag = false;
    buf_count = 0;
  }
  showDisplay();
}
