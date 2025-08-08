#include <iostream>
#include <stdlib.h>
#include <cstdlib> //  rand(), srand()
#include <ctime> // time()
#include <cctype>
#include <string>
#include <cstring>
#include <fstream>
#include <sstream> // za isstringstream
#include <algorithm> // za swap


using namespace std;

struct Koordinata {
    int x;
    int y;
};

struct Igrac {
    int score;
    string ime;
};

int pomRand = 0; // Pomocna globalna promenljiva za nasumicno generisanje
int brPBKomp = 0; // Koliko je komp (delova)brodova pogodio igracu
int brPBIgr = 0; // Koliko je igrac (delova)brodova pogodio kompu
int BrodoviKomp[4] = { 1,2,3,4 }; // Koliko je kompu preostalo brodova
int BrodoviIgrac[4] = { 1,2,3,4 }; // Koliko je igracu preostalo brodova


int** KreiranjeMatrice();
void BrisanjeMatrice(int* matr[]);
void IspisMatrica(int* matr1[], int* matr2[]);
void AutoBrod(int* matr[]);
bool PostavljanjeBroda(int* matr[], Koordinata glavna, int velicina_broda, int hor_ver);
bool Provera(int* matr[], Koordinata glavna, int velicina_broda, int hor_ver);
int SetupIgrac(int* matrI[]);
void KompletnoRucnoPostavljanje(int* matr[]);
bool ProveraUnosa(string unos, int* brDesetki);
void PomocnoRucnoPostavljanje(int* matr[], int velicina);
void IspisMatrice(int* matr[]);
Koordinata PucanjeIgrac(int* matrK[], int* matrPK[]);
Koordinata PucanjeKomp(int* matr[]);
bool ProveraBrodova(int* matr[], int* matrZG[], Koordinata gadjan, int k, int tezinaKomp);
void ScoreBoard(string ime, int HighScore);

int main() {

    //Pravljenje igracove table
    int** tablaIgrac = KreiranjeMatrice();
    int tezina;
    tezina = SetupIgrac(tablaIgrac);

    // Pravljenje table za kompjuter
    int** tablaKomp = KreiranjeMatrice();
    AutoBrod(tablaKomp);

    //Pravljenje table za pucanje na komp
    int** tablaPomocnaKomp = KreiranjeMatrice();
    IspisMatrica(tablaIgrac, tablaPomocnaKomp);

    //Kopiranje igracove table
    int** kopijatableIgrac = KreiranjeMatrice();
    for (int i = 0; i < 10; i++)
        for (int j = 0; j < 10; j++)
            kopijatableIgrac[i][j] = tablaIgrac[i][j];

    // Medjusobno gadjanje brodova dok ne bude pobednik
    int HighScore = 0;
    Koordinata gadjan;
    string ceo;
    while (brPBIgr != 20 && brPBKomp != 20)
    {
        int pamtiK, pamtiI;
    IgracOpet:
        pamtiI = brPBIgr;
        gadjan = PucanjeIgrac(tablaKomp, tablaPomocnaKomp);
        if (pamtiI < brPBIgr) // U slucaju pogodjenog broda igrac igra opet
        {
            if (ProveraBrodova(tablaKomp, tablaPomocnaKomp, gadjan, 1, tezina)) // Da li je ceo brod pogodjen ili ne
                ceo = "je";
            else
                ceo = "nije";
            IspisMatrica(tablaIgrac, tablaPomocnaKomp);
            cout << endl << "Brod " << ceo << " ceo potopljen";
            HighScore++;
            if (brPBIgr == 20)
                break;
            goto IgracOpet;
        }
    KompOpet: // U slucaju pogodjenog brod komp igra opet
        pamtiK = brPBKomp;
        gadjan = PucanjeKomp(tablaIgrac);
        if (pamtiK < brPBKomp)
        {
            ProveraBrodova(kopijatableIgrac, tablaIgrac, gadjan, 3, tezina); // U slucaju da brod nije ceo pogodjen            
            cout << endl << "Protivnik je gadjao: " << char(gadjan.x + 65) << gadjan.y + 1 << endl;
            IspisMatrica(tablaIgrac, tablaPomocnaKomp);
            if (brPBKomp == 20)
                break;
            goto KompOpet;
        }
        cout << endl << "Protivnik je gadjao: " << char(gadjan.x + 65) << gadjan.y + 1 << endl;
        IspisMatrica(tablaIgrac, tablaPomocnaKomp);
        HighScore++; // Pracenje skora igraca
    }

    // Provera pobednika
    if (brPBIgr == 20)
    {
        string ime;
        cout << endl << "Pobedio si!!!" << endl;
        cout << "Score(broj odigranih poteza): " << HighScore << endl;
        cout << endl << "Unesite ime: ";
        getline(cin, ime);
        cout << "Top 5 igraca: " << endl;
        ScoreBoard(ime, HighScore);
    }
    else
    {
        cout << endl << "Izgubio si :/" << endl;
    }



    // Brisanje tabli
    BrisanjeMatrice(tablaKomp);
    BrisanjeMatrice(tablaIgrac);
    BrisanjeMatrice(tablaPomocnaKomp);
    BrisanjeMatrice(kopijatableIgrac);
}

// Za upis, sortiranje i citanje prethodnih skorova
void ScoreBoard(string ime, int HighScore)
{
    // Otvaranje fajla
    fstream fajl("Scoreboard.txt", ios::in | ios::out | ios::app);
    if (!fajl) {
        cerr << "Greska pri otvaranju fajla" << endl;
        exit(1);
    }

    // niz igraca
    Igrac* pobednici = new Igrac[100];
    int i = 0;

    // Provera da li je prazan, tj. tek napravljen
    fajl.seekg(0, ios::end);
    if (fajl.tellg() == 0) {

        fajl.clear();
        fajl << HighScore << " " << ime << endl;
        fajl.close();
        cout << "1. " << ime << ": " << HighScore << endl;
        return;
    }

    // Prepisivanje skorova u niz
    fajl.clear();
    fajl.seekg(0);
    string red;
    bool ubacen = false;
    while (getline(fajl, red)) {
        istringstream iss(red);
        iss >> pobednici[i].score;
        getline(iss >> ws, pobednici[i].ime);
        if (!ubacen && HighScore < pobednici[i].score) // ubacivanje novog skora na sortirano mesto
        {
            i++;
            pobednici[i].score = HighScore;
            pobednici[i].ime = ime;

            swap(pobednici[i], pobednici[i - 1]);
            ubacen = true;
        }

        i++;
    }

    // Ako je najveci skor(tj. najgori) zaelpi ga na kraj
    if (!ubacen) {
        pobednici[i].score = HighScore;
        pobednici[i].ime = ime;
        i++;
    }

    fajl.close();
    // Overvrajtovanje fajla sa novim sortiranim nizom
    ofstream fout("Scoreboard.txt", ios::trunc);
    for (int j = 0; j < i; j++) {
        fout << pobednici[j].score << " " << pobednici[j].ime << endl;
        // Ispis prvih 5 igraca
        if (j < 5)
        {
            cout << j + 1 << ". " << pobednici[j].ime << ": " << pobednici[j].score << endl;
        }
    }
    fout.close();
    delete[]pobednici;
}


// Provera da li je brod ceo pogodjen ili ne
bool ProveraBrodova(int* matr[], int* matrZG[], Koordinata gadjan, int k, int tezinaKomp)
{
    Koordinata pocetna = gadjan;
    int velicina = 1;
    bool x = false, y = false;
    // Pomera koordinate na pocetak broda
dole:
    if (gadjan.y > 0 && matr[gadjan.y - 1][gadjan.x] == 1)
    {
        gadjan.y--;
        goto dole;
    }
levo:
    if (gadjan.x > 0 && matr[gadjan.y][gadjan.x - 1] == 1)
    {
        gadjan.x--;
        goto levo;
    }

    //Broji velicinu broda
    while (gadjan.x < 9 && matr[gadjan.y][gadjan.x + 1] == 1)
    {
        x = true;
        gadjan.x++;
        velicina++;
    }
    while (gadjan.y < 9 && matr[gadjan.y + 1][gadjan.x] == 1)
    {
        y = true;
        gadjan.y++;
        velicina++;
    }
    if (k == 3 && tezinaKomp > 1)
    {
        //Postavlja sigurne granice oko svakog dela broda u slucaju da racunar gadja
        if (pocetna.x > 0 && pocetna.y > 0)
            matrZG[pocetna.y - 1][pocetna.x - 1] = 3;
        if (pocetna.x < 9 && pocetna.y > 0)
            matrZG[pocetna.y - 1][pocetna.x + 1] = 3;
        if (pocetna.x < 9 && pocetna.y < 9)
            matrZG[pocetna.y + 1][pocetna.x + 1] = 3;
        if (pocetna.x > 0 && pocetna.y < 9)
            matrZG[pocetna.y + 1][pocetna.x - 1] = 3;
        // U slucaju da je izabrano 'tesko' komp postavlja granice "hakovajuci", tj. proveracajuci gde nije brod igracu
        if (tezinaKomp == 3)
        {
            if (pocetna.y > 0)
                if (matr[pocetna.y - 1][pocetna.x] != 1)
                    matrZG[pocetna.y - 1][pocetna.x] = 3;
            if (pocetna.x > 0)
                if (matr[pocetna.y][pocetna.x - 1] != 1)
                    matrZG[pocetna.y][pocetna.x - 1] = 3;
            if (pocetna.y < 9)
                if (matr[pocetna.y + 1][pocetna.x] != 1)
                    matrZG[pocetna.y + 1][pocetna.x] = 3;
            if (pocetna.x < 9)
                if (matr[pocetna.y][pocetna.x + 1] != 1)
                    matrZG[pocetna.y][pocetna.x + 1] = 3;
        }
    }
    //Proverava da li je brod ceo pogodjen
    for (int i = 0; i < velicina; i++)
    {
        if (y)
        {
            if (matrZG[gadjan.y - i][gadjan.x] != k)
            {
                return false;
            }
        }
        if (x)
        {
            if (matrZG[gadjan.y][gadjan.x - i] != k)
            {
                return false;
            }
        }
    }
    if (k == 1) {
        //Postavlja brod sa svojim okrugom
        for (int i = 0; i < velicina; i++)
        {
            if (y)
                matrZG[gadjan.y - i][gadjan.x] = 0;
            if (x)
                matrZG[gadjan.y][gadjan.x - i] = 0;
        }
        if (velicina == 1) // U slucaju broda velicnie 1
            matrZG[gadjan.y][gadjan.x] = 0;
        PostavljanjeBroda(matrZG, gadjan, velicina, y);
        BrodoviKomp[4 - velicina]--; // Smanjuje broj brodova racunara odredjene velicine za potopljeni brod
    }
    else
    {
        // U slucaju tezine vece od 'lako' postavlja okvir oko pogodjenog broda
        if (tezinaKomp > 1)
        {
            if (gadjan.y > 0)
                matrZG[gadjan.y - 1][gadjan.x] = 3;
            if (gadjan.x > 0)
                matrZG[gadjan.y][gadjan.x - 1] = 3;
            if (gadjan.y < 9)
                matrZG[gadjan.y + 1][gadjan.x] = 3;
            if (gadjan.x < 9)
                matrZG[gadjan.y][gadjan.x + 1] = 3;
        }
        BrodoviIgrac[4 - velicina]--; // Smanjuje broj brodova igraca odredjene velicine za potopljeni brod
    }
    return true;
}

// Funkcija u kojoj komp gadja igracove brodove
Koordinata PucanjeKomp(int* matr[]) {
    Koordinata glavna;
pocetak:
    srand(time(0) + pomRand); // Da bi dobili razliciti broj svaki put, tj. zadajemo seed trenutnog vremena u sekundama
    pomRand++;
    glavna.x = rand() % 10;
    glavna.y = rand() % 10;
    if (matr[glavna.y][glavna.x] == 3)
        goto pocetak;

    if (matr[glavna.y][glavna.x] == 1)
        brPBKomp++; // Povecava ukupan broj pogodjenih brodova igraca
    if (brPBIgr != 20) // Da ne odigra potez ako je igrac vec pobedio
        matr[glavna.y][glavna.x] = 3;
    return glavna;
}

// Igracevo gadjanje kompovih brodova
Koordinata PucanjeIgrac(int* matrK[], int* matrPK[]) {
    string meta;
    Koordinata pucanje;


pocetak:
    int brDesetki = 0;
    cout << endl << "Unesi metu: ";
    getline(cin, meta);
    // Provera unosa
    if (meta.length() != 2)
    {
        if (meta.length() == 3 && meta.at(1) == '1' && meta.at(2) == '0')
        {
            meta.at(1) = ':';
            meta.erase(2);
            brDesetki = 1;
        }
        else
        {
            cout << "Pogresna velicina unosa";
            goto pocetak;
        }
    }
    meta.at(0) = toupper(meta.at(0));
    if (meta.at(0) > 'J' || meta.at(0) < 'A')
    {
        cout << "Pogresna unesena x koordinata";
        goto pocetak;
    }
    if (meta.at(1) > '9' || meta.at(1) < '0')
    {
        if (meta.at(1) != ':' || brDesetki != 1)
        {
            cout << "Pogresna unesena y koordinata";
            goto pocetak;
        }
    }
    pucanje.y = meta.at(1) - '1';
    pucanje.x = meta.at(0) - 'A';
    if (matrPK[pucanje.y][pucanje.x] != 0)
    {
        if (matrPK[pucanje.y][pucanje.x] == 2)
            cout << "Ne moze biti brod";
        else
            cout << "Vec gadjano mesto";

        goto pocetak;
    }
    // Proverava da li je broj pogodjen ili ne
    if (matrK[pucanje.y][pucanje.x] == 1)
    {
        matrPK[pucanje.y][pucanje.x] = 1;
        brPBIgr++;  // Povecava ukupan broj pogodjenih delova broda Kompa
    }
    else
    {
        matrPK[pucanje.y][pucanje.x] = 3;
    }
    return pucanje;
}



// Objasnjava igru i pomaze igracu pri postavljanju brodova
int SetupIgrac(int* matrI[]) {
    cout << "Igra potatanje brodova pravila:" << endl
        << "- Svaki igrac postavlja jedan brod velicine 4, dva broda velicine 3, tri broda velicine 2 i cetiri broda velicine 1(1x4, 2x3, 3x2, 4x1 brodova)." << endl
        << "- Brodovi mogu biti postavljeni iskljucivo u pravoj liniji, horizontalno ili vertikalno." << endl
        << "- Brodovi se ne smeju preklapati i ne smeju se naci u tudjem okrugu(1 celija oko broda)." << endl
        << "- Nakon postavljanja brodova igraci se menjaju nagadjajuci gde se tudji brodovi nalaze. " << endl
        << "- Ukoliko igrac pogodi tudji brod, ima pravo da pogadja opet." << endl
        << "- Kada igrac pogodi ceo brod izacice mu poruka: \"Brod je potopljen\"." << endl
        << "- Igra se zavrsava kada jedan igrac potopi sve tudje brodove.\nSRECNO!!!" << endl << endl;

    // Odredjivanje tezine protivnika(racunara)
    int tezina = 0;
    string pom;
    cout << "Tezina:" << endl << "1-lako" << endl << "2-normalno" << endl << "3-tesko" << endl;
    do {
        cout << "Vas izbor: ";
        getline(cin, pom); // da ne bi string zabagovao program
        if (pom.length() == 1)
            tezina = pom.at(0) - '0';
        if (tezina < 1 || tezina > 3)
            cout << "Pogresan unos. Unesite broj 1, 2 ili 3 za odredjivanje tezine" << endl;
    } while (tezina < 1 || tezina > 3);

    int izbor = 3;

    // Odredjivanje nacina postavljanja brodova
    cout << endl << "Postavljanje brodova: " << endl << "1 - Automatsko Postavljanje" << endl << "2 - Rucno postavljanje" << endl;
    // Trazi nacin unosa od igraca
    do {
        cout << "Vas izbor: ";
        getline(cin, pom); // da ne bi string zabagovao program
        if (pom.length() == 1)
            izbor = pom.at(0) - '0';

        switch (izbor)
        {
        case 1: // Automatski generise tablu sa nasumicno i ispravno postavljenim brodovima
            cout << endl;
            AutoBrod(matrI);
            break;
        case 2:
            cout << endl;
            // Ispisavanje prazne matrice radi lakseg postavljanja brodova
            cout << "Legenda:\n~ - Voda(prazno mesto)\nX - Brod\no - okrug broda" << endl << endl;
            cout << "   A B C D E F G H I J" << endl;
            for (int i = 0; i < 10; i++)
            {
                if (i < 9)
                    cout << " " << i + 1 << " ";
                else
                    cout << i + 1 << " ";
                cout << "~ ~ ~ ~ ~ ~ ~ ~ ~ ~" << endl;
            }
            cout << "       Vasa tabla" << endl << endl;
            KompletnoRucnoPostavljanje(matrI);
            break;
        default: // Obavestava o gresci unosa
            cout << "Pogresan unos. Unesite broj 1 za autmatsko, a broj 2 za rucno postavljanje" << endl;
            break;
        }
    } while (izbor != 1 && izbor != 2);

    return tezina;
}



//Postavljanje i provera postave brodova od strane igraca
void KompletnoRucnoPostavljanje(int* matr[])
{

    cout << "Unos koordinata za postavljanje brodova: " << endl
        << "- Unos se vrsi od najvecih ka najmanjim brodovima" << endl
        << "- Unosi se prvo pocetna koordinata, zatim \"-\" i onda krajnja koordinata broda(format: xy-xy)" << endl
        << "- Za brodove koji zauzimaju samo jednu celiju, unosi se samo jedna koordinata(format: xy)" << endl
        << "+ Primer unosa za brod velicine 4:A4-D4" << endl
        << "+ Primer unosa za brod velicine 1: J10" << endl;

    for (int i = 4; i > 0; i--)
        PomocnoRucnoPostavljanje(matr, i);
}

// Postavlja odredjeni broj brodova na osnovu njihove velicine
void PomocnoRucnoPostavljanje(int* matr[], int velicina)
{

    string pomocni;
    Koordinata brod;
    bool postoji;
    bool format = true;
    int brDesetki = 0;
    // Broj brodova na osnovu njihove velicine
    int brojBrodova;
    if (velicina == 1)
        brojBrodova = 4;
    else if (velicina == 2)
        brojBrodova = 3;
    else if (velicina == 3)
        brojBrodova = 2;
    else
        brojBrodova = 1;

    // U slucaju da je brod velicine 1, tj. da se unosi samo jedna koordinata
    if (velicina == 1)
    {
        for (int i = 0; i < brojBrodova; i++) {
            // Unos i provera unosa
            do {
            pocetak1:
                brDesetki = 0;
                format = true;
                cout << endl << "Vas unos:" << endl << i + 1 << ". brod velicine " << velicina << ": ";
            pocetakPeek1:
                char peek = cin.peek();
                if (peek == '\n' || peek == ' ')
                {
                    cin.get();
                    goto pocetakPeek1;
                }
                getline(cin, pomocni);

                if (pomocni.length() != 2 && pomocni.length() != 3) // Proverava duzinu stringa
                {
                    cout << "Pogresna velicina unosa" << endl;
                    goto pocetak1;
                }
                pomocni.at(0) = toupper(pomocni.at(0)); // Caseinsensitive
                if (pomocni.length() == 3)
                    if (pomocni.at(1) == '1' && pomocni.at(2) == '0') //U slucaju da je y koordinata 10
                    {
                        pomocni = pomocni.at(0);
                        pomocni.append(":"); // Ascii ekvivalent broju 10
                        brDesetki = 1;
                    }
                if (pomocni.length() != 2) // Ponovna provera duzine
                {
                    cout << "Pogresna velicina unosa" << endl;
                    goto pocetak1;
                }
                if (pomocni.at(0) < 'A' || pomocni.at(0) > 'J') // Provera x koordinate
                {
                    cout << "Pogresno unesena 1. x koordinata" << endl;
                    format = false;
                }
                if (pomocni.at(1) < '1' || pomocni.at(1) > '9') // Provera y koordinate
                {
                    if (pomocni.at(1) != ':' || brDesetki != 1)
                    {
                        cout << "Pogresno unesena 1. y koordinata" << endl;
                        format = false;
                    }
                }
            } while (!format);
            // Postavljanje i provera mesta
            brod.x = pomocni.at(0) - 'A';
            brod.y = pomocni.at(1) - '1';
            postoji = PostavljanjeBroda(matr, brod, 1, 1);

            if (postoji)
            {
                cout << "Invalidno mesto" << endl;
                goto pocetak1; // U slucaju da je mesto zauzeto, vraca na pocetak
            }

            IspisMatrice(matr);
        }
        return;
    }
    // Za sve brodove osim za velicinu 1
    for (int i = 0; i < brojBrodova; i++)
    {
        do {
            // Unos i provera unosa
        pocetak:
            brDesetki = 0;
            cout << endl << "Vas unos:" << endl << i + 1 << ". brod velicine " << velicina << ": ";
        pocetakPeek:
            char peek = cin.peek();
            if (peek == '\n' || peek == ' ')
            {
                cin.get();
                goto  pocetakPeek;
            }

            getline(cin, pomocni);
            if (pomocni.length() < pomocni.find('-') + 2) // Za slucaj da se string zavrsi na -
            {
                cout << "Pogresno formatiran unos (Format: yx-yx)" << endl;
                goto pocetak;
            }
            pomocni.at(0) = toupper(pomocni.at(0)); // Prva x koordinata
            pomocni.at(pomocni.find('-') + 1) = toupper(pomocni.at(pomocni.find('-') + 1)); // Druga x koordinata(odmah posle -)
            format = ProveraUnosa(pomocni, &brDesetki);
        } while (!format);

        //Postavlja brod ili vraca na ponovni ulaz u slucaju prevelikih ili premalih dimenzija
        if (brDesetki == 1 || brDesetki == 3)
        {
            pomocni.at(1) = ':'; // Ascii mesto ekvivalentno broju deset
            pomocni.erase(2, 1); // Brisanje suvisne 0
        }
        if (brDesetki > 1)
        {
            pomocni.at(4) = ':';// Ascii mesto ekvivalentno broju deset
            pomocni.erase(5);// Brisanje suvisne 0
        }
        // Vertikalno postavljanje
        if (pomocni.at(0) == pomocni.at(3) && (pomocni.at(1) == pomocni.at(4) - (velicina - 1) || pomocni.at(1) == pomocni.at(4) + (velicina - 1)))
        {
            if (pomocni.at(1) > pomocni.at(4))
                brod.y = pomocni.at(1) - '1';
            else
                brod.y = pomocni.at(4) - '1';
            brod.x = pomocni.at(0) - 'A';
            postoji = PostavljanjeBroda(matr, brod, velicina, 1);
            if (postoji)
            {
                cout << "Invalidno mesto" << endl;
                goto pocetak; // U slucaju da je mesto zauzeto, vraca na pocetak
            }

        }
        // Horizontalno postavljanje
        else if (pomocni.at(1) == pomocni.at(4) && (pomocni.at(0) == pomocni.at(3) - (velicina - 1) || pomocni.at(0) == pomocni.at(3) + (velicina - 1)))
        {
            if (pomocni.at(0) > pomocni.at(3))
                brod.x = pomocni.at(0) - 'A';
            else
                brod.x = pomocni.at(3) - 'A';
            brod.y = pomocni.at(1) - '1';
            if (pomocni.at(1) == ':')
                brod.y = 9;
            postoji = PostavljanjeBroda(matr, brod, velicina, 0);
            if (postoji)
            {
                cout << "Invalidno mesto" << endl;
                goto pocetak; // U slucaju da je mesto zauzeto, vraca na pocetak
            }
        }
        else {
            cout << "Pogresan unos(dimenzije)" << endl;
            goto pocetak;
        }
        IspisMatrice(matr);
    }

}

//Proverava tacnost fromata unosa za brodove vece od 1
bool ProveraUnosa(string unos, int* brDesetki)
{
    if (unos.length() < 5) // Da bi proverili da li string ima minimalnu trazenu duzinu
    {
        cout << "Pogresno formatiran unos (Format: yx-yx)" << endl;
        return false;
    }
    *brDesetki = 0; // Pamti da li su desetke na 1., 2. ili na oba mesta
    // Pretvaranje 10 u 0 radi provere
    if (unos.at(1) == '1' && unos.at(2) == '0')
    {
        string pom = unos.substr(3, unos.length() - 3);
        unos = unos.at(0);
        unos.append("0");
        unos.append(pom);
        *brDesetki = 1; // Da li je 10 prva y koordinata
    }
    if (unos.at(unos.length() - 2) == '1' && unos.at(unos.length() - 1) == '0')
    {
        unos = unos.substr(0, unos.length() - 1);
        unos.at(unos.length() - 1) = '0';
        *brDesetki += 2; // Da li je 10 druga y koordinata
    }

    if (unos.length() != 5) //Proverava duzinu
    {
        cout << "Pogresno formatiran unos (Format: yx-yx)" << endl;
        return false;
    }
    if (unos.at(0) < 'A' || unos.at(0) > 'J') // 1. x koordinata
    {
        cout << "Pogresno unesena 1. x koordinata" << endl;
        return false;
    }
    if (unos.at(1) < '1' || unos.at(1) > '9') //1. y koordinata
    {
        if (unos.at(1) != '0')
        {
            cout << "Pogresno unesena 1. y koordinata" << endl;
            return false;
        }
        if (*brDesetki != 1 && *brDesetki != 3)
        {
            cout << "Pogresno unesena 1. y koordinata" << endl;
            return false;
        }
    }
    if (unos.at(2) != '-')
    {
        cout << "Pogresno formatiran unos (Format: yx-yx)" << endl;
        return false;
    }
    if (unos.at(3) < 'A' || unos.at(3) > 'J') // 2. x koordinata
    {
        cout << "Pogresno unesena 2. x koordinata" << endl;
        return false;
    }
    if (unos.at(4) < '1' || unos.at(4) > '9') // 2. y koordinata
    {
        if (unos.at(4) != '0')
        {
            cout << "Pogresno unesena 2. y koordinata" << endl;
            return false;
        }
        if (*brDesetki != 2 && *brDesetki != 3)
        {
            cout << "Pogresno unesena 2. y koordinata" << endl;
            return false;
        }
    }

    return true;
}

// Proverava da li je mesto validno za postavljanje broda
bool Provera(int* matr[], Koordinata glavna, int velicina_broda, int hor_ver)
{
    if (hor_ver == 0) // Horizontalna provera
        for (int i = 0; i < velicina_broda; i++)
        {

            if (matr[glavna.y][glavna.x - i] != 0) // Da li je mesto slobodno, tj. da li je 0
                return true;
        }
    else // Vertikalna provera
        for (int i = 0; i < velicina_broda; i++)
        {

            if (matr[glavna.y - i][glavna.x] != 0) // Da li je mesto slobodno, tj. da li je 0
                return true;

        }
    return false;

}

// Funkcija za postavljanje broda i njegove okoline
bool PostavljanjeBroda(int* matr[], Koordinata glavna, int velicina_broda, int hor_ver)
{
    bool pom = Provera(matr, glavna, velicina_broda, hor_ver);
    if (pom)
        return true;
    if (hor_ver == 0) // Horizontalno postavljanje broda
    {
        // Postavljanje broda(broja 1)
        for (int i = 0; i < velicina_broda; i++)
        {
            matr[glavna.y][glavna.x - i] = 1;
        }
        // Postavljanje granica(broja 2) oko broda
        for (int i = -1; i <= velicina_broda; i++) // Prolazak kroz brod i okolne kolone
        {
            if (glavna.x - i >= 0 && glavna.x - i < 10) // Provera da li je kolona unutar granica matrice
            {
                if (glavna.y > 0) // Provera gornjeg reda
                    if (matr[glavna.y - 1][glavna.x - i] != 1)
                        matr[glavna.y - 1][glavna.x - i] = 2;

                if (glavna.y < 9) // Provera donjeg reda
                    if (matr[glavna.y + 1][glavna.x - i] != 1)
                        matr[glavna.y + 1][glavna.x - i] = 2;
            }
        }
        if (glavna.x + 1 < 10) // Desna granica
            if (matr[glavna.y][glavna.x + 1] != 1)
                matr[glavna.y][glavna.x + 1] = 2;

        if (glavna.x - velicina_broda >= 0) // Leva grancia
            if (matr[glavna.y][glavna.x - velicina_broda] != 1)
                matr[glavna.y][glavna.x - velicina_broda] = 2;
    }
    else // Vertikalno postavljanje broda
    {
        for (int i = 0; i < velicina_broda; i++)
        {
            matr[glavna.y - i][glavna.x] = 1;
        }
        // Postavljanje granica(broja 2) oko broda
        for (int i = -1; i <= velicina_broda; i++) // Ukljucujemo jednu dodatnu celiju pre i posle broda
        {
            if (glavna.y - i >= 0 && glavna.y - i < 10) // Provera redova matrice
            {
                if (glavna.x > 0) // Leva strana oko broda
                    if (matr[glavna.y - i][glavna.x - 1] != 1)
                        matr[glavna.y - i][glavna.x - 1] = 2;

                if (glavna.x < 9) // Desna strana oko broda
                    if (matr[glavna.y - i][glavna.x + 1] != 1)
                        matr[glavna.y - i][glavna.x + 1] = 2;
            }
        }
        if (glavna.y + 1 < 10) // Donja celija
            if (matr[glavna.y + 1][glavna.x] != 1)
                matr[glavna.y + 1][glavna.x] = 2;

        if (glavna.y - velicina_broda >= 0) // Gornja celija
            if (matr[glavna.y - velicina_broda][glavna.x] != 1)
                matr[glavna.y - velicina_broda][glavna.x] = 2;
    }

    return false;
}

// Funkcija za automatsko postavljanje brodova
void AutoBrod(int* matr[]) {
    Koordinata glavna;
    bool postoji;
    int pom;

    srand(time(0) + pomRand); // Da bi dobili razliciti broj svaki put, tj. zadajemo seed trenutnog vremena u sekundama
    pomRand++;// Povecavamo da bi iako pozovemo funkciju vise puta u istoj sekundi, uvek dobijemo drugu vrednost

    // Generise koordinate za brod velicine 4
    glavna.x = rand() % 7 + 3;
    glavna.y = rand() % 7 + 3;
    pom = rand() % 2; // Da li ce brod biti veritkalno(0) ili horizontalno(1) postavljen
    PostavljanjeBroda(matr, glavna, 4, pom); // Postavlja brod velicine 4 i njegovu okolinu

    // Postavljanje 2 broda velicine 3
    for (int i = 0; i < 2; i++)
    {
        postoji = true;
        while (postoji) // Dokle god brod nije postavljen na dobro mesto
        {
            postoji = false;
            // Generise koordinate za brod velicine 3
            glavna.x = rand() % 8 + 2;
            glavna.y = rand() % 8 + 2;
            pom = rand() % 2; // Da li ce brod biti veritkalno ili horizontalno postavljen
            postoji = PostavljanjeBroda(matr, glavna, 3, pom); // Provera da li je slobodno mesto, i ako jeste postavlja brod
        }
    }

    // Postavljanje 3 broda velicine 2
    for (int i = 0; i < 3; i++)
    {
        postoji = true;
        while (postoji) // Dokle god brod nije postavljen na dobro mesto
        {
            postoji = false;
            // Generise koordinate za brod velicine 2
            glavna.x = rand() % 9 + 1;
            glavna.y = rand() % 9 + 1;
            pom = rand() % 2; // Da li ce brod biti veritkalno ili horizontalno postavljen
            postoji = PostavljanjeBroda(matr, glavna, 2, pom); // Provera da li je slobodno mesto, i ako jeste postavlja brod
        }
    }

    // Postavljanje 4 broda velicine 1
    for (int i = 0; i < 4; i++)
    {
        postoji = true;
        while (postoji) // Dokle god brod nije postavljen na dobro mesto
        {
            postoji = false;
            // Generise koordinate za brod velicine 1
            glavna.x = rand() % 10;
            glavna.y = rand() % 10;
            pom = rand() % 2; // Da li ce brod biti veritkalno ili horizontalno postavljen
            postoji = PostavljanjeBroda(matr, glavna, 1, pom); // Provera da li je slobodno mesto, i ako jeste postavlja brod
        }
    }

}

// Funkcija za ispis matrice
void IspisMatrice(int* matr[]) {
    cout << endl;
    cout << "   A B C D E F G H I J" << endl; // Ispisivanje naziva kolona
    for (int i = 0; i < 10; i++) {
        //Prva matrica
        // Indexiranje redova
        if (i < 9)
            cout << " " << i + 1 << " ";
        else
            cout << i + 1 << " ";

        for (int j = 0; j < 10; j++) {
            if (matr[i][j] == 0) // Proverava da li je mesto gadjano(0 znaci da nije)
                cout << "~ "; // Znak za vodu
            else if (matr[i][j] == 1)
                cout << "X "; // Znak za pogodjen broj
            else if (matr[i][j] == 2)
                cout << "o "; // Znak za mesto gde drugi brodovi ne mogu biti
            else
                cout << "/ "; // Znak za pogodjeno mesto igracu
        }
        cout << endl;
    }
}

// Funkcija za ispisivanje dve matrice (jedna pored druge)
void IspisMatrica(int* matr1[], int* matr2[]) {
    int brojac = 0;
    cout << endl;
    cout << "   A B C D E F G H I J        A B C D E F G H I J"; // Ispisivanje naziva kolona
    cout << "\tPreostali brodovi" << endl;
    for (int i = 0; i < 10; i++) {
        //Prva matrica
        // Indexiranje redova
        if (i < 9)
            cout << " " << i + 1 << " ";
        else
            cout << i + 1 << " ";

        for (int j = 0; j < 10; j++) {
            if (matr1[i][j] == 0) // Proverava da li je mesto gadjano(0 znaci da nije)
                cout << "~ "; // Znak za vodu
            else if (matr1[i][j] == 1)
                cout << "X "; // Znak za pogodjen broj
            else if (matr1[i][j] == 2)
                cout << "o "; // Znak za mesto gde drugi brodovi ne mogu biti
            else
                cout << "/ "; // Znak za pogodjeno mesto igracu
        }
        //Druga Matrica
        // Indexiranje redova
        if (i < 9)
            cout << "     " << i + 1 << " ";
        else
            cout << "    " << i + 1 << " ";
        for (int j = 0; j < 10; j++) {
            if (matr2[i][j] == 0) // Proverava da li je mesto gadjano(0 znaci da nije)
                cout << "~ "; // Znak za vodu
            else if (matr2[i][j] == 1)
                cout << "X "; // Znak za pogodjen broj
            else
                cout << "o "; // Znak za mesto gde drugi brodovi ne mogu biti
        }
        if (i == 0)
            cout << "\t\t    Komp    Igrac"; // Ispis preostalih brodova
        if (i % 2 == 0 && i > 0)
        {
            cout << "\t" << "Velicine " << 4 - brojac << ":   " << BrodoviKomp[brojac] << "       " << BrodoviIgrac[brojac];
            brojac++;
        }
        cout << endl;
    }
    cout << "      Vasa tabla              Protivnicka tabla" << endl;
}

// Funkcija za pravljenje table(matrice) za potatanje brodova 10x10
int** KreiranjeMatrice() {
    // Alociranje 10 pokazivaca za redove
    int** tabla = new int* [10];
    if (tabla == NULL) {
        cout << "Greska pri alokaciji memorije matrice" << endl;
        exit(1);
    }

    // Alociranje memorije za svaki red
    for (int i = 0; i < 10; i++) {
        tabla[i] = new int[10](); // Postavljanje elemenata na 0
        if (tabla[i] == NULL) {
            cout << "Greska pri alokaciji memorije reda" << endl;
            exit(1);
        }
    }

    return tabla;
}

//Funkcija za brisanje matrice 10x10
void BrisanjeMatrice(int* matr[]) {
    // Sigurnosna provera da li postoji matrica
    if (matr == NULL) return;

    for (int i = 0; i < 10; i++) {
        if (matr[i] != NULL) {
            delete[] matr[i];
        }
    }
    delete[] matr;
}
