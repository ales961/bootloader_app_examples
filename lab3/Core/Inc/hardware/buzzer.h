#ifndef BUZZER_H_
#define BUZZER_H_

#include <inttypes.h>

#define MAX_OCTAVE 8;
#define MIN_VOLUME 0;
#define MAX_VOLUME 10;

char* playA();
char* playB();
char* playC();
char* playD();
char* playE();
char* playF();
char* playG();
char* upOctave();
char* downOctave();
char* upDuration();
char* downDuration();
void setPlay(uint8_t noteNumber);
char* playNote(uint8_t noteNumber);
char* playAll();
void disableIsPlaying();

#endif /* BUZZER_H_ */
