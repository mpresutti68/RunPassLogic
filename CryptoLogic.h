#pragma once
// extern "C" disabilita il "name mangling" del C++, mantenendo il nome della funzione pulito e visibile al C#
extern "C" {
    // Esportiamo la funzione passando i risultati tramite puntatori
    __declspec(dllexport) bool DL(const char* codice, int* outClienteId, int* outMese, int* outAnno);
}