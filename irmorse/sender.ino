#define IR_CLOCK_RATE    38000L
#define ZERO_CLOCK_RATE    0L
#define pwmPin 11   // IR Carrier 
#define DIH 10
#define DAH 30
#define SPACE 70

char* letters_code[90];
char * message = "PIGI MERDAAAAAAAAA ";
int delay_;

void setup() {
  Serial.begin(2400);
  // toggle on compare, clk/1
  TCCR2A = _BV(WGM21) | _BV(COM2A0);
  TCCR2B = _BV(CS20);
  // 38kHz carrier/timer
  OCR2A = (F_CPU/(IR_CLOCK_RATE*2L)-1);
  OCR2B = OCR2A / 2;
  pinMode(pwmPin, OUTPUT);
  
  letters_code[48] = (char *) "-----";
  letters_code[49] = (char *) ".----";
  letters_code[50] = (char *) "..---";
  letters_code[51] = (char *) "...--";
  letters_code[52] = (char *) "....-"; 
  letters_code[53] = (char *) ".....";
  letters_code[54] = (char *) "-....";
  letters_code[55] = (char *) "--..."; 
  letters_code[56] = (char *) "---..";
  letters_code[57] = (char *) "----.";
  letters_code[65] = (char *) ".-"; 
  letters_code[66] = (char *) "-..."; 
  letters_code[67] = (char *) "-.-.";
  letters_code[68] = (char *) "-..";
  letters_code[69] = (char *) ".";
  letters_code[70] = (char *) "..-.";
  letters_code[71] = (char *) "--."; 
  letters_code[72] = (char *) "....";
  letters_code[73] = (char *) "..";
  letters_code[74] = (char *) ".---";
  letters_code[75] = (char *) "-.-";
  letters_code[76] = (char *) ".-..";
  letters_code[77] = (char *) "--";
  letters_code[78] = (char *) "-.";
  letters_code[79] = (char *) "---";
  letters_code[80] = (char *) ".--.";
  letters_code[81] = (char *) "--.-";
  letters_code[82] = (char *) ".-.";
  letters_code[83] = (char *) "...";
  letters_code[84] = (char *) "-";
  letters_code[85] = (char *) "..-";
  letters_code[86] = (char *) "...-";
  letters_code[87] = (char *) ".--";
  letters_code[88] = (char *) "-..-";
  letters_code[89] = (char *) "-.--";
  letters_code[90] = (char *) "--..";
}

void pulse(unsigned long delay_time, int bit_value)
{
  if (bit_value)
    TCCR2A = _BV(WGM21) | _BV(COM2A0);
  else
    TCCR2A = _BV(WGM21);
  delay(delay_time);
}

void loop() {
  for(char* it_mess = message; *it_mess; ++it_mess) 
  {
    
    if(int(*it_mess)==32){
      pulse(SPACE, 1);
      pulse(DIH, 0);
    }
    else
    {
      char * code = letters_code[int(*it_mess)];
      for(char* it_code = code; *it_code; ++it_code)
      {    
        if(*it_code=='.')
        {
          pulse(DIH, 1);
        }
        else
        {
          pulse(DAH, 1);
        }
        pulse(DIH, 0);
      }
    }
    pulse(2 * DIH, 0);
  }
}
