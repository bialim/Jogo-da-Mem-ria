/*
 * Jogo de Memória Musical Multiplayer
 * 
 * Funcionalidades:
 * - 1 ou 2 jogadores
 * - Registro de nomes dos jogadores
 * - Cronometragem de tempo para cada jogador
 * - Ranking ao final
 * - Feedback sonoro
 */

#define NOTE_B0  31
#define NOTE_C1  33
#define NOTE_CS1 35
#define NOTE_D1  37
#define NOTE_DS1 39
#define NOTE_E1  41
#define NOTE_F1  44
#define NOTE_FS1 46
#define NOTE_G1  49
#define NOTE_GS1 52
#define NOTE_A1  55
#define NOTE_AS1 58
#define NOTE_B1  62
#define NOTE_C2  65
#define NOTE_CS2 69
#define NOTE_D2  73
#define NOTE_DS2 78
#define NOTE_E2  82
#define NOTE_F2  87
#define NOTE_FS2 93
#define NOTE_G2  98
#define NOTE_GS2 104
#define NOTE_A2  110
#define NOTE_AS2 117
#define NOTE_B2  123
#define NOTE_C3  131
#define NOTE_CS3 139
#define NOTE_D3  147
#define NOTE_DS3 156
#define NOTE_E3  165
#define NOTE_F3  175
#define NOTE_FS3 185
#define NOTE_G3  196
#define NOTE_GS3 208
#define NOTE_A3  220
#define NOTE_AS3 233
#define NOTE_B3  247
#define NOTE_C4  262
#define NOTE_CS4 277
#define NOTE_D4  294
#define NOTE_DS4 311
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_FS4 370
#define NOTE_G4  392
#define NOTE_GS4 415
#define NOTE_A4  440
#define NOTE_AS4 466
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_CS5 554
#define NOTE_D5  587
#define NOTE_DS5 622
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_FS5 740
#define NOTE_G5  784
#define NOTE_GS5 831
#define NOTE_A5  880
#define NOTE_AS5 932
#define NOTE_B5  988
#define NOTE_C6  1047
#define NOTE_CS6 1109
#define NOTE_D6  1175
#define NOTE_DS6 1245
#define NOTE_E6  1319
#define NOTE_F6  1397
#define NOTE_FS6 1480
#define NOTE_G6  1568
#define NOTE_GS6 1661
#define NOTE_A6  1760
#define NOTE_AS6 1865
#define NOTE_B6  1976
#define NOTE_C7  2093
#define NOTE_CS7 2217
#define NOTE_D7  2349
#define NOTE_DS7 2489
#define NOTE_E7  2637
#define NOTE_F7  2794
#define NOTE_FS7 2960
#define NOTE_G7  3136
#define NOTE_GS7 3322
#define NOTE_A7  3520
#define NOTE_AS7 3729
#define NOTE_B7  3951
#define NOTE_C8  4186
#define NOTE_CS8 4435
#define NOTE_D8  4699
#define NOTE_DS8 4978
#define REST      0

// Configurações do jogo
const int bPin = 12;
int melodyOg[] = { NOTE_A2, NOTE_A3, NOTE_A4, NOTE_A5, NOTE_A6 };
int noteDurationOg = 1000/8;
int ledPins[5] = {2, 3, 4, 5, 6};
int buttonPins[5] = {7, 8, 9, 10, 11};
int sequence[5] = {-1, -1, -1, -1, -1};
int play[5] = {-1, -1, -1, -1, -1};

// Variáveis do jogo
int count = 0;
int numJogadores = 1;
int jogadorAtual = 0;
unsigned long tempoInicio = 0;
unsigned long tempoJogada[2] = {0, 0};
String nomesJogadores[2] = {"", ""};
bool aguardandoInput = true;
bool gameStarted = false;
int Tempo = (60000/128)*4;

// Variáveis para música de vitória
volatile int currentNote = 0;
volatile bool playingVictory = false;

// Melodia de vitória
const int victoryMelody[] = {
  NOTE_C5,16, NOTE_B4,16, NOTE_C5,4, NOTE_E5,4, 
  NOTE_FS5,4, NOTE_A5,16, NOTE_G5,16, NOTE_A5,8, NOTE_G5,8, NOTE_FS5,8, NOTE_A5,8,
  NOTE_G5,4, NOTE_B5,16, NOTE_A5,16, NOTE_B5,4, NOTE_E5,4,
  NOTE_C5,16, NOTE_B4,16, NOTE_C5,8, NOTE_B4,8, NOTE_A4,4,
  NOTE_FS5,4, NOTE_A5,16, NOTE_G5,16, NOTE_A5,8, NOTE_G5,8, NOTE_FS5,8, NOTE_A5,8,
  NOTE_G5,3, NOTE_FS5,16, NOTE_G5,16, NOTE_A5,3, NOTE_G5,16, NOTE_A5,16,
  NOTE_B5,8, NOTE_A5,8, NOTE_G5,8, NOTE_FS5,8, NOTE_E5,4, NOTE_B4,4, NOTE_C5,1
};

const int victoryDurations[] = {
  16,16,4,4,4,16,16,8,8,8,8,
  4,16,16,4,4,16,16,8,8,4,
  4,16,16,8,8,8,8,
  3,16,16,3,16,16,
  8,8,8,8,4,4,1
};

void setup() {
  Serial.begin(9600);
  
  // Configura pinos
  for(int i = 0; i < 5; i++){
    pinMode(ledPins[i], OUTPUT);
    pinMode(buttonPins[i], INPUT_PULLUP);
  }
  
  // Mensagem inicial
  Serial.println("Bem-vindo ao Jogo de Memoria Musical!");
  Serial.println("Quantos jogadores? (Digite 1 ou 2)");
}

void loop() {
  if (aguardandoInput) {
    if (!gameStarted) {
      tratarSelecaoJogadores();
    } else {
      tratarNomesJogadores();
    }
  } else {
    if (jogadorAtual < numJogadores) {
      jogarPartida();
    } else {
      mostrarRanking();
      // Reinicia o jogo
      aguardandoInput = true;
      gameStarted = false;
      jogadorAtual = 0;
      Serial.println("\n\nNovo jogo!");
      Serial.println("Quantos jogadores? (Digite 1 ou 2)");
    }
  }
}

void tratarSelecaoJogadores() {
  if (Serial.available() > 0) {
    char c = Serial.read();
    if (c == '1' || c == '2') {
      numJogadores = c - '0';
      gameStarted = true;
      aguardandoInput = true;
      Serial.print("\nJogadores selecionados: ");
      Serial.println(numJogadores);
      Serial.println("Digite o nome do Jogador 1 e pressione ENTER:");
    } else {
      Serial.println("Entrada invalida! Digite 1 ou 2");
    }
  }
}

void tratarNomesJogadores() {
  if (Serial.available() > 0) {
    String nome = Serial.readStringUntil('\n');
    nome.trim();
    
    if (nome.length() > 0) {
      nomesJogadores[jogadorAtual] = nome;
      jogadorAtual++;
      
      if (jogadorAtual < numJogadores) {
        Serial.print("Digite o nome do Jogador ");
        Serial.print(jogadorAtual + 1);
        Serial.println(" e pressione ENTER:");
      } else {
        aguardandoInput = false;
        jogadorAtual = 0;
        randomSeed(analogRead(A1));
        Serial.println("\nJogo iniciado!");
      }
    }
  }
}

void jogarPartida() {
  Serial.print("\nVez de ");
  Serial.println(nomesJogadores[jogadorAtual]);
  Serial.println("Pressione qualquer botao para comecar...");
  
  // Espera qualquer botão ser pressionado para começar
  while (true) {
    for (int i = 0; i < 5; i++) {
      if (digitalRead(buttonPins[i]) == LOW) {
        delay(300); // Debounce
        break;
      }
    }
    break;
  }
  
  tempoInicio = millis();
  playSequence();
  
  while (count < 5) {
    for (int i = 0; i < 5; i++) {
      if (digitalRead(buttonPins[i]) == LOW) {
        delay(250);
        tone(bPin, melodyOg[i], noteDurationOg);
        digitalWrite(ledPins[i], HIGH);
        play[count] = i;
        count++;
        delay(200);
        digitalWrite(ledPins[i], LOW);
        validatePlay();
      }
    }
  }
  
  // Jogador acertou
  tempoJogada[jogadorAtual] = millis() - tempoInicio;
  Serial.print("Parabens, ");
  Serial.print(nomesJogadores[jogadorAtual]);
  Serial.print("! Tempo: ");
  Serial.print(tempoJogada[jogadorAtual] / 1000);
  Serial.println(" segundos");
  
  startVictoryMusic();
  delay(3000);
  
  // Prepara próximo jogador
  count = 0;
  jogadorAtual++;
  if (jogadorAtual < numJogadores) {
    resetGameForNextPlayer();
  }
}

void resetGameForNextPlayer() {
  for (int i = 0; i < 5; i++) {
    play[i] = -1;
    digitalWrite(ledPins[i], LOW);
  }
  randomSeed(analogRead(A1));
}

void mostrarRanking() {
  Serial.println("\n=== RESULTADO FINAL ===");
  
  if (numJogadores == 1) {
    Serial.print(nomesJogadores[0]);
    Serial.print(" completou em ");
    Serial.print(tempoJogada[0] / 1000);
    Serial.println(" segundos");
  } else {
    // Determina o vencedor
    int vencedor = tempoJogada[0] < tempoJogada[1] ? 0 : 1;
    
    Serial.print("Vencedor: ");
    Serial.print(nomesJogadores[vencedor]);
    Serial.print(" (");
    Serial.print(tempoJogada[vencedor] / 1000);
    Serial.println(" segundos)");
    
    Serial.print(nomesJogadores[1 - vencedor]);
    Serial.print(": ");
    Serial.print(tempoJogada[1 - vencedor] / 1000);
    Serial.println(" segundos");
  }
}

void playSequence() {
  for (int i = 0; i < 5; i++) {
    int randomNumber = random(5);
    sequence[i] = randomNumber;
  }
  
  for (int i = 0; i < 5; i++) {
    delay(200);
    tone(bPin, melodyOg[sequence[i]], noteDurationOg);
    digitalWrite(ledPins[sequence[i]], HIGH);
    delay(500);
    digitalWrite(ledPins[sequence[i]], LOW);
  }
}

void validatePlay() {
  for (int i = 0; i < 5; i++) {
    if (play[i] != -1 && play[i] != sequence[i]) {
      // Jogador errou
      for (int j = 0; j < 5; j++) {
        digitalWrite(ledPins[j], HIGH);
        delay(500);
      }
      
      gameOverMusic();
      
      // Reinicia a tentativa
      for (int j = 0; j < 5; j++) {
        play[j] = -1;
        digitalWrite(ledPins[j], LOW);
      }
      
      count = 0;
      Serial.println("Sequencia errada. Tente novamente...");
      delay(2000);
      playSequence();
      return;
    }
  }
}

void startVictoryMusic() {
  playingVictory = true;
  currentNote = 0;
  setupTimer1();
}

void setupTimer1() {
  // Configura TIMER1 para controle da música
  TCCR1A = 0;                // Modo normal
  TCCR1B = 0;                // Limpa registrador
  TCNT1 = 0;                 // Zera temporizador
  OCR1A = 31250;             // Valor para interrupção
  TCCR1B |= (1 << WGM12);    // Modo CTC
  TCCR1B |= (1 << CS12) | (1 << CS10); // Prescaler 1024
  TIMSK1 |= (1 << OCIE1A);   // Habilita interrupção
}

ISR(TIMER1_COMPA_vect) {
  if (playingVictory) {
    if (currentNote < sizeof(victoryMelody)/sizeof(victoryMelody[0])) {
      int note = victoryMelody[currentNote];
      int duration = victoryDurations[currentNote];
      
      if (note > 0) {
        tone(bPin, note, Tempo/duration);
      }
      
      currentNote++;
      OCR1A = (Tempo/duration) * 15625 / 1000;
    } else {
      playingVictory = false;
      noTone(bPin);
      TIMSK1 &= ~(1 << OCIE1A); // Desabilita interrupção
    }
  }
}

void gameOverMusic() {
  int tempo = 200;
  int melody[] = {
    NOTE_C5,-4, NOTE_G4,-4, NOTE_E4,4,
    NOTE_A4,-8, NOTE_B4,-8, NOTE_A4,-8, NOTE_GS4,-8, NOTE_AS4,-8, NOTE_GS4,-8,
    NOTE_G4,8, NOTE_D4,8, NOTE_E4,-1,
  };

  int notes = sizeof(melody) / sizeof(melody[0]) / 2;
  int wholenote = (60000 * 4) / tempo;
  int divider = 0, noteDuration = 0;
  
  for (int thisNote = 0; thisNote < notes * 2; thisNote = thisNote + 2) {
    divider = melody[thisNote + 1];
    if (divider > 0) {
      noteDuration = (wholenote) / divider;
    } else if (divider < 0) {
      noteDuration = (wholenote) / abs(divider);
      noteDuration *= 1.5; 
    }
    tone(bPin, melody[thisNote], noteDuration * 0.9);
    delay(noteDuration);
    noTone(bPin);
  }
}