#define _CRT_SECURE_NO_WARNINGS 

#include "vozilo.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void unos_teksta(char* odrediste, int max_velicina) {
    fgets(odrediste, max_velicina, stdin);
    odrediste[strcspn(odrediste, "\n")] = 0;
}

void demonstracija_datotecnih_funkcija() {
    FILE* izvor = NULL;
    FILE* kopija = NULL;
    char buffer;
    size_t bajtovi;



    izvor = fopen(DATOTEKA_BAZA, "rb");
    if (izvor != NULL) {
        kopija = fopen("skladiste_backup.bin", "wb");
        if (kopija != NULL) {
            while ((bajtovi = fread(&buffer, 1, 1, izvor)) > 0) {
                fwrite(&buffer, 1, bajtovi, kopija);
            }
            fclose(kopija);
            printf(" -> Korak 1: Izradjena kopija datoteke pod nazivom 'skladiste_backup.bin'\n");
        }
        fclose(izvor);
    }
    else {
        printf(" -> Napomena: Datoteka baze je prazna. Nemam sto kopirati.\n");
        return;
    }

    if (remove(DATOTEKA_BAZA) == 0) {
        printf(" -> Korak 2: Originalna datoteka uspjesno obrisana pomocu remove().\n");
    }

    if (rename("skladiste_backup.bin", DATOTEKA_BAZA) == 0) {
        printf(" -> Korak 3: Sigurnosna kopija uspjesno preimenovana natrag pomocu rename().\n");
    }
}

int main() {
    OklopnoVozilo* skladiste = NULL;
    int broj_vozila = 0;
    int izbor = 0;
    int i = 0;

    OklopnoVozilo novo;
    OklopnoVozilo izmjenjeno;
    const OklopnoVozilo* pronadjeno = NULL;
    int unos_id = 0;
    int oklop_izbor = 0;
    int rez = 0;
    OpcijaIzbornika odabrana_opcija;

    if (ucitaj_bazu(&skladiste, &broj_vozila) == 0) {
        sortiraj_vozila(skladiste, broj_vozila);
        printf("Sustav podignut. Ucitano vozila iz datoteke: %d\n", broj_vozila);
    }

    do {
        printf("\n=========================================\n");
        printf("   SUCELJE: SKLADISTE OKLOPNIH VOZILA   \n");
        printf("=========================================\n");
        printf("1. Unesi novo oklopno vozilo (Create/Insert)\n");
        printf("2. Ispisi sva vozila iz skladista (Read)\n");
        printf("3. Azuriraj podatke o vozilu (Update)\n");
        printf("4. Ukloni vozilo iz skladista (Delete)\n");
        printf("5. Brza pretraga vozila po ID-u (bsearch)\n");
        printf("6. Pokreni datotecni podsustav (remove/rename)\n");
        printf("0. Izlaz iz aplikacije\n");
        printf("-----------------------------------------\n");
        printf("Vas odabir: ");

        if (scanf("%d", &izbor) != 1) {
            printf("Greska: Unos mora biti broj. Pokusajte ponovo.\n");
            while (getchar() != '\n');
            continue;
        }
        while (getchar() != '\n');

        odabrana_opcija = (OpcijaIzbornika)izbor;

        switch (odabrana_opcija) {
        case CREATE_INSERT: {
            printf("Unesite ID novog vozila (cijeli broj): ");
            scanf("%d", &novo.id);
            while (getchar() != '\n');

            printf("Unesite tocan naziv vozila: ");
            unos_teksta(novo.naziv, MAX_NAZIV);

            printf("Unesite tezinu vozila (u tonama): ");
            scanf("%d", &novo.tezina_tona);

            printf("Unesite broj clanova posade: ");
            scanf("%d", &novo.broj_posade);

            printf("Odaberite kategoriju oklopa (0 - Lako, 1 - Srednje, 2 - Tesko): ");
            scanf("%d", &oklop_izbor);
            novo.tip = (TipOklopa)oklop_izbor;

            if (dodaj_vozilo(&skladiste, &broj_vozila, &novo) == 0) {
                sortiraj_vozila(skladiste, broj_vozila);
                printf("Uspjeh: Vozilo spremljeno u RAM i upisano u binarnu datoteku!\n");
            }
            break;
        }
        case READ_ALL: {
            if (broj_vozila == 0) {
                printf("Skladiste je trenutno potpuno prazno.\n");
            }
            else {
                printf("\n--- PREGLED STANJA SKLADISTA ---\n");
                for (i = 0; i < broj_vozila; i++) {
                    if (INLINE_VALIDAN_ID(i, broj_vozila)) {
                        printf("ID: %d | Naziv: %s | Tezina: %d t | Posada: %d clana | Oklop: %d\n",
                            skladiste[i].id, skladiste[i].naziv, skladiste[i].tezina_tona,
                            skladiste[i].broj_posade, skladiste[i].tip);
                    }
                }
            }
            break;
        }
        case UPDATE: {
            printf("Unesite jedinstveni ID vozila koje zelite azurirati: ");
            scanf("%d", &unos_id);
            while (getchar() != '\n');

            printf("Unesite NOVI naziv vozila: ");
            unos_teksta(izmjenjeno.naziv, MAX_NAZIV);

            printf("Unesite NOVU tezinu (t): ");
            scanf("%d", &izmjenjeno.tezina_tona);

            printf("Unesite NOVI broj posade: ");
            scanf("%d", &izmjenjeno.broj_posade);

            printf("Odaberite NOVI tip oklopa (0 - Lako, 1 - Srednje, 2 - Tesko): ");
            scanf("%d", &oklop_izbor);
            izmjenjeno.tip = (TipOklopa)oklop_izbor;

            rez = azuriraj_vozilo(skladiste, broj_vozila, unos_id, &izmjenjeno);
            if (rez == 0) printf("Uspjeh: Podaci o vozilu azurirani u bazi i datoteci!\n");
            else printf("Greska: Vozilo s unesenim ID-om %d ne postoji u sustavu.\n", unos_id);
            break;
        }
        case DELETE: {
            printf("Unesite ID vozila koje zelite trajno ukloniti: ");
            scanf("%d", &unos_id);

            rez = obrisi_vozilo(&skladiste, &broj_vozila, unos_id);
            if (rez == 0) printf("Uspjeh: Vozilo obrisano, a memorijski prostor suzen!\n");
            else printf("Greska: Nemoguce obrisati, ID ne postoji u bazi podataka.\n");
            break;
        }
        case PRETRAZI: {
            printf("Unesite ID oklopnog vozila koje pretrazujete: ");
            scanf("%d", &unos_id);

            pronadjeno = pronadji_vozilo(skladiste, broj_vozila, unos_id);
            if (pronadjeno != NULL) {
                printf("\n[REZULTAT PRETRAGE - bsearch]:\n");
                printf(" -> Pronadjeno vozilo: %s\n -> Tezina: %d tona\n -> Broj posade: %d\n",
                    pronadjeno->naziv, pronadjeno->tezina_tona, pronadjeno->broj_posade);
            }
            else {
                printf("Obavijest: Vozilo s trazenim ID-om %d nije pronadjeno.\n", unos_id);
            }
            break;
        }
        case OPERACIJE_DATOTEKA: {
            demonstracija_datotecnih_funkcija();
            break;
        }
        case IZLAZ:
            printf("Zatvaranje aplikacije Skladiste... Oslobadjanje resursa...\n");
            break;
        default:
            printf("Greska: Odabrali ste nepostojecu opciju u izborniku.\n");
            break;
        }
    } while (izbor != 0);

    oslobodi_memoriju(&skladiste, &broj_vozila);
    return 0;
}