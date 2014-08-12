#include <EEPROM.h>
#define DEPTH 4
byte cube[4][4][4];
long update_counter = 0;
long update_max = 20000;
boolean up = true;
int layer = -1;
byte dir = 0;

struct point {
  byte x;
  byte y;
  byte z; 
};

point p1, p2, p3, p4;

void (*update_method)();

void clear_cube(){
  for (int i = 0; i < DEPTH; i++){
    for (int j = 0; j < DEPTH; j++){
      for (int k = 0; k < DEPTH; k++){
        cube[i][j][k] = 0;
      }
    }
  }
}

void init_move_cube(){
  update_max = 20000;
  update_method = &mode_move_cube;
  p1.x = 0;
  p1.y = 0;
  p1.z = 0;
  p2.x = 3;
  p2.y = 3;
  p2.z = 3;
}


void mode_move_cube(){
 for(int x = p1.x; x <= p2.x; x++){
  cube[p1.y][x][p1.z] = 1;
  cube[p1.y][x][p2.z] = 1;
  cube[p2.y][x][p1.z] = 1;
  cube[p2.y][x][p2.z] = 1;
 } 
 for(int y = p1.y; y <= p2.y; y++){
  cube[y][p1.x][p1.z] = 1;
  cube[y][p1.x][p2.z] = 1;
  cube[y][p2.x][p1.z] = 1;
  cube[y][p2.x][p2.z] = 1;
 } 
 for(int z = p1.z; z <= p2.z; z++){
  cube[p1.y][p1.x][z] = 1;
  cube[p2.y][p1.x][z] = 1;
  cube[p1.y][p2.x][z] = 1;
  cube[p2.y][p2.x][z] = 1;
 } 
}

void init_rand_pixel(){ 
  update_max = 10000;
  update_method = &mode_rand_pixel;
  p1.x = random(4);
  p1.y = random(4);
  p1.z = random(4);
  cube[p1.z][p1.x][p1.y] = 1;
}

void mode_rand_pixel(){
  cube[p1.x][p1.y][p1.z] = 0;
  if (random(100) > 33){
   if (p1.z == 0){ p1.z = 1; }
   else if (p1.z == 3){ p1.z = 2; }
   else { p1.z += random(2) ? -1 : +1; }   
  }
  if (random(100) > 33){
   if (p1.x == 0){ p1.x = 1; }
   else if (p1.x == 3){ p1.x = 2; }
   else { p1.x += random(2) ? -1 : +1; }   
  }
  if (random(100) > 33){
   if (p1.y == 0){ p1.y = 1; }
   else if (p1.y == 3){ p1.y = 2; }
   else { p1.y += random(2) ? -1 : +1; }   
  }
  cube[p1.x][p1.y][p1.z] = 1;
}

void init_rand(){ 
  update_max = 50000;
  update_method = &mode_rand;
}

void mode_rand(){
  for (int i = 0; i < DEPTH; i++){
    for (int j = 0; j < DEPTH; j++){
      for (int k = 0; k < DEPTH; k++){
        cube[i][j][k] = random(100) > 50 ? 1 : 0;
      }
    }
  }  
}

void init_rain(){
  update_max = 150000;
  update_method = &mode_rain;
}

void mode_rain(){
  for (int i = DEPTH-1; i > 0 ; i--){
    for (int j = 0; j < DEPTH; j++){
      for (int k = 0; k < DEPTH; k++){
        if (i == DEPTH-1 && cube[i][j][k] == 1){
          cube[i][j][k] = 0;
          continue;
        }
        if (cube[i-1][j][k] == 1){
          if (i == 1 && random(100) > 15){
            continue;
          }
          cube[i][j][k] = 1;
          cube[i-1][j][k] = 0;
         }
      }
    }
  }  
  if (random(100) > 13){
   cube[0][random(4)][random(4)] = 1;
  }
}

void init_row(){
  update_max = 20000;
  update_method = &mode_row;
}

void mode_row(){    
  if (up){
    for (int i = 0; i < DEPTH; i++){
      for (int j = 0; j < DEPTH; j++){
        for (int k = 0; k < DEPTH; k++){
          if (i == DEPTH-1 && j == DEPTH-1 && k == DEPTH-1){
            up = false;
          }
          if (cube[i][j][k] == 0){
            cube[i][j][k] = 1;
            return;
          }
        }
      }
    }    
  }else{
    for (int i = DEPTH-1; i >= 0; i--){
      for (int j = DEPTH-1; j >= 0; j--){
        for (int k = DEPTH-1; k >= 0; k--){
          if (i == 0 && j == 0 && k == 0){
            up = true;
          }
          if (cube[i][j][k] == 1){
            cube[i][j][k] = 0;
            return;
          }
        }
      }
    }  
  }
}

void update(){
  update_counter += 1;
  if (update_counter > update_max){
    update_counter = 0;
     update_method(); 
  }
}

void render(){
  layer = layer + 1 >= DEPTH ? 0 : layer +1;  
  PORTC = ((PORTC)&(B11000000))|(1 << layer+2)|(cube[layer][3][3] << 1)|(cube[layer][3][2]);
  PORTB = ((PORTB)&(B11000000))|(cube[layer][3][1] << 5)|(cube[layer][3][0] << 4)|(cube[layer][2][3] << 3)|(cube[layer][2][2] << 2)|(cube[layer][2][1] << 1)|(cube[layer][2][0]);
  PORTD = (B00000000)|(cube[layer][0][3] << 7)|(cube[layer][0][2] << 6)|(cube[layer][0][1] << 5)|(cube[layer][0][0] << 4)|(cube[layer][1][3] << 3)|(cube[layer][1][2] << 2)|(cube[layer][1][1] << 1)|(cube[layer][1][0]);  
}

void setup() {  
    
  for (int i=0; i < A5;i++){
    pinMode(i, OUTPUT); 
    digitalWrite(i, LOW);
  }
  
  noInterrupts();
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1  = 0;
  
  OCR1A = 250;//250;
  TCCR1B |= (1 << WGM12);
  TCCR1B &= ~((1 << CS10)|(1 << CS11));
  TCCR1B |= (1 << CS12);
  TIMSK1 |= (1 << OCIE1A);
  interrupts(); 
  
  clear_cube();
  
  byte last_method = EEPROM.read(0);
  
  switch (last_method) {
   case 0:
      init_rain();
      break;
   case 1:
      init_rand();
      break;
   case 2:
      init_rand_pixel();
      break;
   case 3:
      init_row();
      break;
   case 4:
      init_move_cube();
      break;
   default:
      init_rain();
      last_method = 0; 
  }
  
  last_method++;
  EEPROM.write(0,last_method);
}

ISR(TIMER1_COMPA_vect){
  render();
}

void loop() {
  
  update();

}
