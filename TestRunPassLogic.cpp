#include <iostream>
#include <vector>

// Forward declaration opaca per il puntatore della classe 
// (non abbiamo bisogno di includere RunPassLogic.h qui, testiamo solo l'API esposta)
class RunPassLogic;

// Dichiariamo le firme delle funzioni esportate dalla DLL
extern "C" {
    __declspec(dllimport) RunPassLogic* CreateRunPassLogic(const char* codiceLicenza, long idverifica);
    __declspec(dllimport) void DestroyRunPassLogic(RunPassLogic* instance);
    __declspec(dllimport) bool LM_IsValida(RunPassLogic* instance);
    __declspec(dllimport) int LM_CCSI(
        RunPassLogic* instance, int ci, int cl, int ca,
        const int* cmArr, int cmSize, const int* cuArr, int cuSize);
    __declspec(dllimport) int LM_TPNMI(
        RunPassLogic* instance, const int* cmArr, int cmSize, const int* cuArr, int cuSize);

    __declspec(dllimport) int LM_GetMese(RunPassLogic* instance);

    __declspec(dllimport) int LM_GetAnno(RunPassLogic* instance);
}

int main() {
    std::cout << "=== TEST RUNPASSLOGIC DLL IN C++ ===" << std::endl;

    // Parametri di test
    const char* codiceLicenza = "6263-0839-0957";
    long idDaVerificare = 51665;

    std::cout << "Verifica codice: " << codiceLicenza << " per ID: " << idDaVerificare << "...\n";

    // 1. Creazione dell'istanza tramite la DLL
    RunPassLogic* logica = CreateRunPassLogic(codiceLicenza, idDaVerificare);

    // 2. Lettura della validità
    if (LM_IsValida(logica)) {
        std::cout << "[SUCCESSO] Licenza valida!\n";

        // Preparazione dati per il test TPNM
        std::vector<int> cm = { 2, 4, 6 };
        std::vector<int> cu = { 1, 2, 3, 4 };

        // 3. Esecuzione del test passando i puntatori (data) e le dimensioni (size) dei vector
        int risultato = LM_TPNMI(logica, cm.data(), (int)cm.size(), cu.data(), (int)cu.size());

        std::cout << "Risultato elaborazione TPNM: " << risultato << "\n";

        // --- AGGIUNTA: Chiamata alla funzione GetMese ---
        int mese = LM_GetMese(logica);
        std::cout << "Valore Mese ottenuto: " << mese << "\n";

        // --- AGGIUNTA: Chiamata alla funzione GetAnno ---
        int anno = LM_GetAnno(logica);
        std::cout << "Valore Anno ottenuto: " << anno << "\n";
    }
    else {
        std::cout << "[FALLITO] La licenza non e' valida. (Normale se il codice non matcha le chiavi)\n";
    }

    // 4. Pulizia della memoria
    if (logica != nullptr) {
        DestroyRunPassLogic(logica);
        logica = nullptr;
    }

    std::cout << "\nPremi Invio per uscire...";
    std::cin.get();
    return 0;
}