#include <bitset>

using namespace std;

short led = 13;
char message[] = {'S', 'O', 'S'};
std::bitset<10> code[26];
short DAH = 1000;
short DIH = 200;

std::bitset<10> get_code(std::bitset<10> code[26], char c)
{
  if (((int) c > 64) && ((int) c < 91)) // uppercase letter
  {
    return code[((int) c - 65)];
  }
  
}
void blink_a_char(std::bitset<10> set)
{
  short i = 4;
  while(i>=0 && set.test(i))
  {
    if (set.test(i+5))
    {
      digitalWrite(led, HIGH);
      delay(DAH);
    }
    else
    {
      digitalWrite(led, HIGH);
      delay(DIH);      
    }
    digitalWrite(led, LOW);
    delay(1000);
    i--;
  }
}





void setup() {
  // put your setup code here, to run once:
  pinMode(led, OUTPUT);
  code[0] 	= std::bitset<10>(std::string("0100011000"));	// A
  code[1] 	= std::bitset<10>(std::string("1000011110"));	// B
  code[2] 	= std::bitset<10>(std::string("1010011110"));	// C
  code[3] 	= std::bitset<10>(std::string("1000011100"));	// D
  code[4] 	= std::bitset<10>(std::string("0000010000"));	// E
  code[5] 	= std::bitset<10>(std::string("0010011110"));	// F
  code[6] 	= std::bitset<10>(std::string("1100011100"));	// G
  code[7] 	= std::bitset<10>(std::string("0000011110"));	// H
  code[8] 	= std::bitset<10>(std::string("0000011000"));	// I
  code[9] 	= std::bitset<10>(std::string("0111011110"));	// J
  code[10] 	= std::bitset<10>(std::string("1010011100"));	// K
  code[11] 	= std::bitset<10>(std::string("0100011110"));	// L
  code[12] 	= std::bitset<10>(std::string("1100011000"));	// M
  code[13] 	= std::bitset<10>(std::string("1000011000"));	// N
  code[14] 	= std::bitset<10>(std::string("1110011100"));	// O
  code[15] 	= std::bitset<10>(std::string("0110011110"));	// P
  code[16] 	= std::bitset<10>(std::string("1101011110"));	// Q
  code[17] 	= std::bitset<10>(std::string("0100011100"));	// R
  code[18] 	= std::bitset<10>(std::string("0000011100"));	// S
  code[19] 	= std::bitset<10>(std::string("1000010000"));	// T
  code[20] 	= std::bitset<10>(std::string("0010011100"));	// U
  code[21] 	= std::bitset<10>(std::string("0001011110"));	// V
  code[22] 	= std::bitset<10>(std::string("0110011100"));	// W
  code[23] 	= std::bitset<10>(std::string("1001011110"));	// X
  code[24] 	= std::bitset<10>(std::string("1011011110"));	// Y
  code[25] 	= std::bitset<10>(std::string("1100011110"));	// Z
}

void loop() {
  short i;
  
  for (i = 0; i < (sizeof(message)/sizeof(*message)); i++)
  {
    blink_a_char(get_code(code, message[i]));
  }
  digitalWrite(led, LOW);
  delay(2000);
}
