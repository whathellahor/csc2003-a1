#ifndef BARCODE_H
#define BARCODE_H

#define BARCODE_PIN 26

char decode(int message[]); 
int colorDetection(float volatage);
void barcode();
void initBarcode();

#endif /* BARCODE_H */