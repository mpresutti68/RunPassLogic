#include "CryptoLogic.h"
#include <stdlib.h>
#include <string.h>

// 1. INSERISCI QUI LE TUE CHIAVI SEGRETE!
// Essendo scritte qui, finiranno compilate in assembly e invisibili da .NET.
const long long CHIAVE_MOLT = 4398213;      // Sostituisci con il valore reale
const long long CHIAVE_ADD = 8549320184;       // Sostituisci con il valore reale
const long long MODULO = 1000000000000LL;   // Sostituisci con il valore reale (nota LL finale per long long)
const int IDAPPLICAZIONE = 1;               // Sostituisci con il tuo ID applicazione

// 2. Trasposizione esatta della tua funzione C# in C++
long long CalcolaInversoModulare(long long a, long long m) {
    long long m0 = m;
    long long y = 0, x = 1;

    if (m == 1) return 0;

    while (a > 1) {
        long long q = a / m;
        long long t = m;

        m = a % m;
        a = t;
        t = y;

        y = x - q * y;
        x = t;
    }

    if (x < 0) x += m0;
    return x;
}

// 3. SOSTITUTO DI BigInteger: Moltiplicazione sicura (a * b) % m per evitare overflow a 64 bit
long long MoltiplicazioneModulare(long long a, long long b, long long m) {
    long long res = 0;
    a = a % m;
    while (b > 0) {
        if (b % 2 == 1) res = (res + a) % m;
        a = (a * 2) % m;
        b /= 2;
    }
    return res;
}

extern "C" {
    __declspec(dllexport) bool DL(const char* codice, int* outClienteId, int* outMese, int* outAnno) {
        // Controlli di base (anche se già validati in C#)
        if (codice == nullptr || strlen(codice) != 12) return false;

        // Convertiamo la stringa char* in long long (Int64)
        long long cifrato_int = atoll(codice);

        // Algoritmo di decodifica
        long long inverso_moltiplicativo = CalcolaInversoModulare(CHIAVE_MOLT, MODULO);

        long long temp_base = cifrato_int - CHIAVE_ADD;

        // In C++ il resto di un numero negativo può essere negativo, quindi:
        temp_base = temp_base % MODULO;
        if (temp_base < 0) {
            temp_base += MODULO;
        }

        // Qui evitiamo l'uso di BigInteger usando la nostra moltiplicazione sicura
        temp_base = MoltiplicazioneModulare(temp_base, inverso_moltiplicativo, MODULO);

        long long numero_base = temp_base;

        // Estrazione
        int app_estratta = (int)(numero_base % 10);
        int mese_estratto = (int)((numero_base / 10) % 100);
        int anno_estratto = (int)((numero_base / 1000) % 100);
        int id_estratto = (int)(numero_base / 100000);

        //if (app_estratta != IDAPPLICAZIONE) {
        //    return false; // ID errato
        //}

        // Assegniamo i valori estratti ai puntatori per restituirli al C#
        if (outClienteId) *outClienteId = id_estratto;
        if (outMese) *outMese = mese_estratto;
        if (outAnno) *outAnno = anno_estratto;

        return true;
    }
}
