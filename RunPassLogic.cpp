#include "RunPassLogic.h"
#include <algorithm>
#include <cstring>
#include <cstdlib>

// =========================================================================
// IMPLEMENTAZIONE DEI METODI PRIVATI
// =========================================================================

long long RunPassLogic::calcolaInversoModulare(long long a, long long m) {
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

long long RunPassLogic::moltiplicazioneModulare(long long a, long long b, long long m) {
    long long res = 0;
    a = a % m;
    while (b > 0) {
        if (b % 2 == 1) res = (res + a) % m;
        a = (a * 2) % m;
        b /= 2;
    }
    return res;
}

// =========================================================================
// COSTRUTTORE E VALIDAZIONE
// =========================================================================

RunPassLogic::RunPassLogic(const char* codice, long long molt, long long add, long long mod, int idApp, long idverifica)
    : chiave_molt(molt), chiave_add(add), modulo(mod), id_applicazione(idApp),
    licenza_valida(false), cliente_id(0), mese(0), anno(0) // <-- Corretta la parentesi extra qui
{
    validaLicenza(codice, idverifica);
}

bool RunPassLogic::validaLicenza(const char* codice, long idverifica) {
    if (codice == nullptr) {
        licenza_valida = false;
        return false;
    }

    std::string codicePulito(codice);
    codicePulito.erase(std::remove_if(codicePulito.begin(), codicePulito.end(),
        [](char c) { return c == '-' || c == '.'; }), codicePulito.end());

    if (codicePulito.length() != 12) {
        licenza_valida = false;
        return false;
    }

    long long cifrato_int = 0;
    try {
        cifrato_int = std::stoll(codicePulito);
    }
    catch (...) {
        // Se ci sono lettere o caratteri strani sfuggiti, fallisce in modo sicuro
        licenza_valida = false;
        return false;
    }
    long long inverso_moltiplicativo = calcolaInversoModulare(chiave_molt, modulo);
    long long temp_base = cifrato_int - chiave_add;

    temp_base = temp_base % modulo;
    if (temp_base < 0) {
        temp_base += modulo;
    }

    temp_base = moltiplicazioneModulare(temp_base, inverso_moltiplicativo, modulo);
    long long numero_base = temp_base;

    int app_estratta = (int)(numero_base % 10);
    int mese_estratto = (int)((numero_base / 10) % 100);
    int anno_estratto = (int)((numero_base / 1000) % 100); // Corretto da /100 a /1000 in base al tuo codice originale
    int id_estratto = (int)(numero_base / 10000000);

    if (app_estratta != id_applicazione) {
        licenza_valida = false;
        return false;
    }

    std::string str_estratto = std::to_string(id_estratto);
    std::string str_verifica = std::to_string(idverifica);
   
    if (str_estratto.find(str_verifica) != 0) {
        licenza_valida = false;
        return false;
    }

    // Licenza valida, salviamo i dati
    cliente_id = id_estratto;
    mese = mese_estratto;
    anno = anno_estratto;
    licenza_valida = true;

    return true;
}

bool RunPassLogic::IsLicenzaValida() const {
    return licenza_valida;
}

int RunPassLogic::GetMese() const {
    return mese;
}

int RunPassLogic::GetAnno() const {
    return anno;
}

// =========================================================================
// LOGICA APPLICATIVA (CCS e TPNM)
// =========================================================================

int RunPassLogic::EseguiCCS(int ci, int cl, int ca, const std::vector<int>& cm, const std::vector<int>& cu) {
    if (!licenza_valida) return -999; // Blocco esecuzione se licenza non valida

    int mm = -1;
    if (!cm.empty()) {
        mm = *std::max_element(cm.begin(), cm.end());
    }

    int mu = -1;
    if (!cu.empty()) {
        mu = *std::max_element(cu.begin(), cu.end());
    }

    int p = std::max(ci, ca + 1);
    int ls = std::min(cl, mu + 1);
    for (int cc = p; cc <= ls; ++cc) {
        bool im = (std::find(cm.begin(), cm.end(), cc) != cm.end());
        if (im) continue;
        return cc;
    }

    if (mm != -1) return mm;
    return ci;
}

int RunPassLogic::EseguiTPNM(const std::vector<int>& cm, const std::vector<int>& cu) {
    if (!licenza_valida) return -999; // Blocco esecuzione se licenza non valida

    int pct = -1;
    for (int c_u : cu) {
        bool i_m = (std::find(cm.begin(), cm.end(), c_u) != cm.end());
        if (!i_m) {
            if (pct == -1 || c_u < pct) {
                pct = c_u;
            }
        }
    }
    return pct;
}

// =========================================================================
// INTERFACCIA C ESPORTATA (Wrapper per C#)
// =========================================================================

extern "C" {
    // Definizione delle chiavi segrete
    const long long SECRET_MOLT = 4398213;
    const long long SECRET_ADD = 8549320184;
    const long long SECRET_MOD = 1000000000000LL;
    const int SECRET_APP_ID = 1;

    // 1. Crea l'istanza passando il codice licenza e l'id da verificare
    __declspec(dllexport) RunPassLogic* CreateRunPassLogic(const char* codiceLicenza, long idverifica) {
        return new RunPassLogic(codiceLicenza, SECRET_MOLT, SECRET_ADD, SECRET_MOD, SECRET_APP_ID, idverifica);
    }

    // 2. Distrugge l'istanza liberando la memoria
    __declspec(dllexport) void DestroyRunPassLogic(RunPassLogic* instance) {
        if (instance != nullptr) {
            delete instance;
        }
    }

    // 3. Controlla se la licenza è valida (utile per verificare lato C# se Create ha avuto successo)
    __declspec(dllexport) bool LM_IsValida(RunPassLogic* instance) {
        if (instance == nullptr) return false;
        if (instance == nullptr || !instance->IsLicenzaValida()) return false;
        return true;
    }

    __declspec(dllexport) int LM_GetMese(RunPassLogic* instance) {
        if (instance == nullptr || !instance->IsLicenzaValida()) return -1;
        return instance->GetMese();
    }

    __declspec(dllexport) int LM_GetAnno(RunPassLogic* instance) {
        if (instance == nullptr || !instance->IsLicenzaValida()) return -1;
        return instance->GetAnno();
    }

    // 4. Wrapper per CCS
    __declspec(dllexport) int LM_CCSI(
        RunPassLogic* instance,
        int ci, int cl, int ca,
        const int* cmArr, int cmSize,
        const int* cuArr, int cuSize)
    {
        if (instance == nullptr || !instance->IsLicenzaValida()) return -999;

        std::vector<int> m;
        if (cmArr != nullptr && cmSize > 0) m.assign(cmArr, cmArr + cmSize);

        std::vector<int> u;
        if (cuArr != nullptr && cuSize > 0) u.assign(cuArr, cuArr + cuSize);

        return instance->EseguiCCS(ci, cl, ca, m, u);
    }

    // 5. Wrapper per TPNM
    __declspec(dllexport) int LM_TPNMI(
        RunPassLogic* instance,
        const int* cmArr, int cmSize,
        const int* cuArr, int cuSize)
    {
        if (instance == nullptr || !instance->IsLicenzaValida()) return -999;

        std::vector<int> m;
        if (cmArr != nullptr && cmSize > 0) m.assign(cmArr, cmArr + cmSize);

        std::vector<int> u;
        if (cuArr != nullptr && cuSize > 0) u.assign(cuArr, cuArr + cuSize);

        return instance->EseguiTPNM(m, u);
    }
}