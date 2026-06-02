#define _CRT_SECURE_NO_WARNINGS
#include "vozilo.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
// Definiranje extern varijable i staticke varijable za brojanje operacija (Koncept 6, 8)
const char* DATOTEKA_BAZA = "skladiste.bin";
static int brojac_operacija = 0;

// Pomocna funkcija za usporedbu (qsort/bsearch - Koncept 23, 24, 26)
static int usporedi_id(const void* a, const void* b) {
	const OklopnoVozilo* v1 = (const OklopnoVozilo*)a;
	const OklopnoVozilo* v2 = (const OklopnoVozilo*)b;
	return (v1->id - v2->id);
}

// Rekurzivno pretrazivanje (Koncept 25)
static int rekurzivni_binarni_search(const OklopnoVozilo* polje, int lijevo, int desno, int id) {
	if (desno >= lijevo) {
		int sredina = lijevo + (desno - lijevo) / 2;
		if (polje[sredina].id == id) return sredina;
		if (polje[sredina].id > id) return rekurzivni_binarni_search(polje, lijevo, sredina - 1, id);
		return rekurzivni_binarni_search(polje, sredina + 1, desno, id);
	}
	return -1;
}

// Ucitavanje iz binarne datoteke (Koncept 16, 17, 19, 20, 22)
int ucitaj_bazu(OklopnoVozilo** polje, int* broj_vozila) {
	if (polje == NULL || broj_vozila == NULL) return -1; // Zastita parametara (Koncept 14)

	FILE* fp = fopen(DATOTEKA_BAZA, "rb");
	if (fp == NULL) {
		if (errno == ENOENT) {
			*polje = NULL;
			*broj_vozila = 0;
			return 0;
		}
		ERROR_LOG("Neuspjesno otvaranje datoteke");
		return -1;
	}

	// Odredivanje velicine datoteke pomocu fseek i ftell (Koncept 20)
	fseek(fp, 0, SEEK_END);
	long velicina_datoteke = ftell(fp);
	rewind(fp);

	*broj_vozila = velicina_datoteke / sizeof(OklopnoVozilo);
	if (*broj_vozila == 0) {
		*polje = NULL;
		fclose(fp);
		return 0;
	}

	// dinamicko zauzimanje memorije + malloc(Koncept 16, 17)
	*polje = (OklopnoVozilo*)malloc((*broj_vozila) * sizeof(OklopnoVozilo));
	if (*polje == NULL) {
		ERROR_LOG("Alokacija memorije neuspjesna");
		fclose(fp);
		return -1;
	}

	size_t procitano = fread(*polje, sizeof(OklopnoVozilo), *broj_vozila, fp);
	if (procitano != (size_t)*broj_vozila) {
		if (ferror(fp)) ERROR_LOG("Greska pri citanju");
		free(*polje);
		*polje = NULL;
		fclose(fp);
		return -1;
	}

	fclose(fp);
	return 0;
}


// Zapisivanje u datoteku (Koncept 19)
int zapisi_bazu(const OklopnoVozilo* polje, int broj_vozila) {
	if (broj_vozila > 0 && polje == NULL) return -1;

	FILE* fp = fopen(DATOTEKA_BAZA, "wb");
	if (fp == NULL) {
		ERROR_LOG("Greska pri otvaranju za pisanje");
		return -1;
	}

	if (broj_vozila > 0) {
		size_t zapisano = fwrite(polje, sizeof(OklopnoVozilo), broj_vozila, fp);
		if (zapisano != (size_t)broj_vozila) {
			ERROR_LOG("Greska pri zapisivanju");
			fclose(fp);
			return -1;
		}
	}

	fclose(fp);
	brojac_operacija++;
	return 0;
}


int dodaj_vozilo(OklopnoVozilo** polje, int* broj_vozila, const OklopnoVozilo* novo_vozilo) {
	if (polje == NULL || broj_vozila == NULL || novo_vozilo == NULL) return -1;

	// Realokacija prostora za novo vozilo (Koncept 17)
	OklopnoVozilo* privremeno = (OklopnoVozilo*)realloc(*polje, (*broj_vozila + 1) * sizeof(OklopnoVozilo));
	if (privremeno == NULL) {
		ERROR_LOG("Realokacija memorije neuspjesna");
		return -1;
	}
	*polje = privremeno;
	(*polje)[*broj_vozila] = *novo_vozilo;
	(*broj_vozila)++;

	return zapisi_bazu(*polje, *broj_vozila);
}

// UPDATE (Koncept 1)
int azuriraj_vozilo(OklopnoVozilo* polje, int broj_vozila, int id, const OklopnoVozilo* izmjenjeno) {
	if (polje == NULL || izmjenjeno == NULL) return -1;

	int indeks = rekurzivni_binarni_search(polje, 0, broj_vozila - 1, id);
	if (indeks == -1) return -2;

	polje[indeks] = *izmjenjeno;
	polje[indeks].id = id;

	return zapisi_bazu(polje, broj_vozila);
}

// DELETE (Koncept 1, 17)
int obrisi_vozilo(OklopnoVozilo** polje, int* broj_vozila, int id) {
	if (polje == NULL || broj_vozila == NULL || *polje == NULL) return -1;

	int indeks = rekurzivni_binarni_search(*polje, 0, *broj_vozila - 1, id);
	if (indeks == -1) return -2;


	for (int i = indeks; i < *broj_vozila - 1; i++) {
		(*polje)[i] = (*polje)[i + 1];
	}

	(*broj_vozila)--;

	if (*broj_vozila == 0) {
		free(*polje);
		*polje = NULL;
	}
	else {
		OklopnoVozilo* privremeno = (OklopnoVozilo*)realloc(*polje, (*broj_vozila) * sizeof(OklopnoVozilo));
		if (privremeno != NULL) {
			*polje = privremeno;
		}
	}

	return zapisi_bazu(*polje, *broj_vozila);
}

// Sortiranje + qsort(pokazivaci) (Koncept 23, 26)
void sortiraj_vozila(OklopnoVozilo* polje, int broj_vozila) {
	if (polje == NULL || broj_vozila <= 1) return;
	qsort(polje, broj_vozila, sizeof(OklopnoVozilo), usporedi_id);
}

// Pretrazivanje,pokazivaci,READ(Koncept 1, 24, 26)
const OklopnoVozilo* pronadji_vozilo(const OklopnoVozilo* polje, int broj_vozila, int id) {
	if (polje == NULL || broj_vozila <= 0) return NULL;

	OklopnoVozilo kljuc;
	kljuc.id = id;

	return (const OklopnoVozilo*)bsearch(&kljuc, polje, broj_vozila, sizeof(OklopnoVozilo), usporedi_id);
}

// Sigurno brisanje memorije i  anuliranje pokazivaca(Koncept 18)
void oslobodi_memoriju(OklopnoVozilo** polje, int* broj_vozila) {
	if (polje != NULL && *polje != NULL) {
		free(*polje);
		*polje = NULL;
	}
	if (broj_vozila != NULL) {
		*broj_vozila = 0;
	}
}