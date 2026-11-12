/*Controle de ciclos para lavadora de barril com Arduino
Abandonado por não ter disponíveis válvulas elétricas que permitam bloqueio de fluxo nos dois sentidos (as solenóides disponíveis só bloqueavam em um sentido)
Última atualização: 2015.*/ 
#include <LiquidCrystal.h>

unsigned long timer;

#define btnRIGHT  0
#define btnUP     1
#define btnDOWN   2
#define btnLEFT   3
#define btnSELECT 4
#define btnNONE   5

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
// int lcd_key = btnNONE;
int last_key = btnNONE;
int hor_pos = 0, ver_pos = 0;
unsigned int segundos_ciclo = 30; // Default


#define CICLO_NORMAL	0
#define CICLO_EXTENDIDO	1
#define EXPURGAR		2
#define ENXAGUAR		3
#define SODA			4
#define PERACETICO		5
#define PRESSURIZAR		6

int menu_lvl[2] = {CICLO_NORMAL, 0};

/* char menu_str[7][2][] = {
	{"Ciclo Normal    ", "Iniciar"},
	{"Ciclo Extendido ", "Iniciar"},
	{"Expurgar        ", "Tempo: "},
	{"Enxaguar        ", "Tempo: "},
	{"Circ. Soda      ", "Tempo: "},
	{"Circ. Peracetico", "Tempo: "},
	{"Pressurizar     ", "Iniciar"}
};
 */

// Lê os botões do módulo LCD
// Retorna valor do botão apenas no evento de pressionar um botão.
// Não retorna valor de botão para pressionamento contínuo.
// Argumentos: NENHUM
// Retorna: (int) valor da tecla
// Variáveis globais usadas: (int) last_key
int read_LCD_buttons() {
  int adc_key_in  = 0;
  int key_val = btnNONE;
  adc_key_in = analogRead(0);       // read the value from the sensor
  if      (adc_key_in < 50)   key_val =  btnRIGHT;
  else if (adc_key_in < 195)  key_val =  btnUP;
  else if (adc_key_in < 380)  key_val =  btnDOWN;
  else if (adc_key_in < 555)  key_val =  btnLEFT;
  else if (adc_key_in < 790)  key_val =  btnSELECT;
  else key_val = btnNONE;                // when all others fail, return this.

  if (last_key == btnNONE)
  {
    last_key = key_val;
    return key_val;
  }
  else
  {
    last_key = key_val;
    return btnNONE;
  }
}




// Imprime um número int com um número especifico de caracteres, justificando à direita.
// Argumentos: Valor a imprimir; número de caracteres.
void print_int(int value, unsigned char len)
{
  unsigned char cnt = 0;
  int value_cnt = value;
  do
  {
    cnt++;
    value_cnt = value_cnt / 10;
  } while (value_cnt != 0);
  if (value < 0) cnt++;
  cnt = len - cnt;
  for (; cnt > 0; cnt--) lcd.print(" ");
  lcd.print(value);
}

// Imprime um número int com um número especifico de caracteres, inserindo zeros à esquerda.
// Argumentos: Valor a imprimir; número de caracteres.
void print_int_lz(int value, unsigned char len)
{
  unsigned char cnt = 0;
  int value_cnt = value;
  int value_unsigned = value;
  do
  {
    cnt++;
    value_cnt = value_cnt / 10;
  } while (value_cnt != 0);
  if (value < 0)
  {
    cnt++;
    lcd.print("-");
    value_unsigned = -value;
  }
  cnt = len - cnt;
  for (; cnt > 0; cnt--) lcd.print("0");
  lcd.print(value_unsigned);
}

//Imprime minutos e segundos (mm:ss) a partir de um valor em milisegundos.
void print_time(unsigned int sec)
{
  unsigned int seconds = 0;
  unsigned int minutes = 0;

  if (sec >= 3600)
  {
    lcd.print("Overflow");
  }
  else
  {
    minutes = sec / 60;
    seconds = sec - minutes * 60;
    //  lcd.setCursor(0,0);             // set the LCD cursor   position
    //  lcd.print(minutes);
    //  lcd.setCursor(0,1);             // set the LCD cursor   position
    //  lcd.print(seconds);
    print_int_lz(int(minutes), 2);
    lcd.print(":");
    print_int_lz(int(seconds), 2);
  }
}

// Flag de timer - emula o efeito de uma interrupção de tempo dentro do ciclo de programa.
bool timer_flag()
{
  if (timer <= millis()) return true;
  else return false;
}

// Checa o tempo restante do timer
float check_timer()
{
  float timer_read = 0;
  if (timer <= millis()) timer_read = 0;
  else timer_read = float(timer - millis()) / 1000;
  return timer_read;
}

// Seta o timer para x segundos além do tempo atual.
void set_timer (float time_set)
{
  timer = long(time_set * 1000) + millis();
}

bool menu()
{
	int button = btnNONE;
	// int menu_lvl[2] = {CICLO_NORMAL, 0};
	char menu_str[7][2][17] = {
		{"Ciclo Normal    ", "Iniciar"},
		{"Ciclo Extendido ", "Iniciar"},
		{"Expurgar        ", "Tempo: "},
		{"Enxaguar        ", "Tempo: "},
		{"Circ. Soda      ", "Tempo: "},
		{"Circ. Peracetico", "Tempo: "},
		{"Pressurizar     ", "Iniciar"}
	};

	button = read_LCD_buttons();

	if(menu_lvl[1] == 0){
		switch(button) {
		 case btnRIGHT:
			menu_lvl[1] = 1;
			break;
		 // case btnLEFT:
			// break;
		 case btnDOWN:
			if (menu_lvl[0] == PRESSURIZAR) menu_lvl[0] = 0;
			else menu_lvl[0]++;
			break;
		 case btnUP:
			if (menu_lvl[0] == 0) menu_lvl[0] = PRESSURIZAR;
			else menu_lvl[0]--;
			break;
		 // case btnSELECT:
			// break;
		 default:
			break;
		}
	}
	else {
		switch(button) {
		 case btnRIGHT:
			//Aqui vai uma instrução de início de operação
			return true;
			break;
		 case btnLEFT:
			menu_lvl[1] = 0;
			break;
		 case btnUP:
			if (menu_lvl[0] == EXPURGAR	|| menu_lvl[0] == ENXAGUAR || menu_lvl[0] == SODA || menu_lvl[0] == PERACETICO || segundos_ciclo < 600) segundos_ciclo++;
			break;
		 case btnDOWN:
			if (menu_lvl[0] == EXPURGAR	|| menu_lvl[0] == ENXAGUAR || menu_lvl[0] == SODA || menu_lvl[0] == PERACETICO || segundos_ciclo > 0) segundos_ciclo--;
			break;
		 // case btnSELECT:
			// break;
		 default:
			break;
	    }
	}
	
	if (button != btnNONE) {
		lcd.setCursor(0, 0);
		lcd.print(menu_str[menu_lvl[0]][0]);
		lcd.setCursor(0, 1);
		if (menu_lvl[1] == 1){
			lcd.print(menu_str[menu_lvl[0]][1]);
			if (menu_lvl[0] == EXPURGAR	|| menu_lvl[0] == ENXAGUAR || menu_lvl[0] == SODA || menu_lvl[0] == PERACETICO) print_int(segundos_ciclo, 3);
		}
		else lcd.print("                ");
	}
		
}


void setup()
{
  lcd.begin(16, 2);               // start the library
  lcd.setCursor(0, 0);            // set the LCD cursor   position
  lcd.print("Lavadora    V.00");
  lcd.setCursor(0, 1);            // set the LCD cursor   position
  lcd.print("H. Frohlich 2015");
  delay(5000);
  lcd.clear();
  lcd.print("Ciclo Normal");
}


void loop() {
	#define MENU		0
	#define WARNING		1
	#define OPERATION	2
	#define HALT		3
	int state = MENU;
	bool beep;
	menu();


	
/*  	switch (state){
		case MENU:
			if (menu()) {
				state = WARNING;
				set_timer(3);
			}
			// adicionar uma função que reinicie 'operation()'
		case WARNING:
			if (read_LCD_buttons() == btnSELECT) {
				state = MENU;
				beep = false;
			}
			else if (operation()) state == MENU;
		case OPERATION:
			if (read_LCD_buttons() == btnSELECT) state = HALT;
			else if (operation()) state = MENU;
		case HALT:
			
 	}*/
  //time = 3600000 - millis();
  //prints time since program started
  // if (check_timer() == 0) set_timer(61);
  // lcd.setCursor(7, 0);            // set the LCD cursor   position
  // print_time(int(check_timer()));
  //  lcd.setCursor(0,1);             // set the LCD cursor   position
  //  lcd.print(time);
  // wait a second so as not to send massive amounts of data
  // delay(1000);

/*    lcd_key = read_LCD_buttons();
  
   switch(lcd_key) {
     case btnRIGHT:
       hor_pos++;
       lcd.setCursor(12,0);
       print_int_lz(hor_pos, 4);
       break;
     case btnLEFT:
       hor_pos--;
       lcd.setCursor(12,0);
       print_int_lz(hor_pos, 4);
       break;
     case btnUP:
       ver_pos++;
       lcd.setCursor(12,1);
       print_int_lz(ver_pos, 4);
       break;
     case btnDOWN:
       ver_pos--;
       lcd.setCursor(12,1);
       print_int_lz(ver_pos, 4);
       break;
     default:
       break;
   }
 */

}
