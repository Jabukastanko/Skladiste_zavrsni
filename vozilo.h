
#ifndef VOZILO_H
#define VOZILO_H

#include <stdio.h>

#define MAX_NAZIV 50
#define ERROR_LOG(poruka) perror("GRESKA [" poruka "]")
#define INLINE_VALIDAN_ID(id, max) ((id) >= 0 && (id) < (max))

typedef enum {
    LAKO_OKLOPLJENO,
    SREDNJE_OKLOPLJENO,
    TESKO_OKLOPLJENO
} TipOklopa;

// OVAJ ENUM MORA IMATI OVAKVE NAZIVE DA BI IH MAIN.C PREPOZNAO:
typedef enum {
    IZLAZ = 0,
    CREATE_INSERT,
    READ_ALL,
    UPDATE,
    DELETE,
    PRETRAZI,
    OPERACIJE_DATOTEKA  // Rijec koja je javljala gresku!
} OpcijaIzbornika;

typedef struct {
    int id;
    char naziv[MAX_NAZIV];
    int tezina_tona;
    int broj_posade;
    TipOklopa tip;
} OklopnoVozilo;

extern const char* DATOTEKA_BAZA;

int ucitaj_bazu(OklopnoVozilo** polje, int* broj_vozila);
int zapisi_bazu(const OklopnoVozilo* polje, int broj_vozila);
int dodaj_vozilo(OklopnoVozilo** polje, int* broj_vozila, const OklopnoVozilo* novo_vozilo);
int azuriraj_vozilo(OklopnoVozilo* polje, int broj_vozila, int id, const OklopnoVozilo* izmjenjeno);
int obrisi_vozilo(OklopnoVozilo** polje, int* broj_vozila, int id);
void sortiraj_vozila(OklopnoVozilo* polje, int broj_vozila);
const OklopnoVozilo* pronadji_vozilo(const OklopnoVozilo* polje, int broj_vozila, int id);
void oslobodi_memoriju(OklopnoVozilo** polje, int* broj_vozila);

#endif // VOZILO_H#pragma once
