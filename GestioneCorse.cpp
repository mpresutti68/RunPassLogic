#include "GestioneCorse.h"
#include <algorithm>
#include <sstream>

// COSTRUTTORE
GestioneCorse::GestioneCorse(int corsainiz, int corsalim, const std::unordered_map<int, std::string>& paraltrecorse)
    : corsainiziale(corsainiz), corsalimite(corsalim), altrecorse(paraltrecorse), inviatastampa(false)
{
    corsaattuale = corsainiz;
}

// Implementazione del nuovo metodo
void GestioneCorse::AggiungiAltraCorsa(int chiave, const std::string& valore) {
    altrecorse[chiave] = valore;
}
// GESTIONE EVENTI (CALLBACKS)
void GestioneCorse::SetAggiornaDatiComandaCallback(AggiornaDatiComandaCallback callback) {
    onAggiornaDatiComanda = callback;
}
void GestioneCorse::SetAggiungiMarciatoCallback(AggiungiMarciatoCallback callback) {
    onAggiungiMarciato = callback;
}
void GestioneCorse::OnAggiornaDatiComanda() {
    if (onAggiornaDatiComanda) onAggiornaDatiComanda();
}
void GestioneCorse::OnAggiungiMarciato(int corsa) {
    if (onAggiungiMarciato) onAggiungiMarciato(corsa);
}

// METODI PRIVATI
GestioneCorse::TipoCorsaEnum GestioneCorse::TipoCorsa(int corsa) const {
    if (corsa == 0) return TipoCorsaEnum::NoCourse;
    else if (corsa >= corsainiziale && corsa <= corsalimite) return TipoCorsaEnum::Base;
    else if ((corsa > 0 && corsa < corsainiziale) || (corsa >= corsalimite && corsa < 20)) return TipoCorsaEnum::AltraCorsa;
    return TipoCorsaEnum::NonGestita;
}

bool GestioneCorse::SePrimaCorsa(int corsa) const {
    return (corseattuali.empty());
}

int GestioneCorse::UltimaCorsaUsata() const {
    if (corseattuali.empty()) return 0;

    int max_val = 0;
    bool found = false;
    for (const auto& pair : corseattuali) {
        if (SeCorsaIterabile(pair.first)) {
            if (!found || pair.first > max_val) {
                max_val = pair.first;
                found = true;
            }
        }
    }
    return found ? max_val : 0;
}

bool GestioneCorse::SeTutteMarciate() const {
    // IsSubsetOf: Tutte le corse attuali sono presenti in corsemarciate?
    for (const auto& pair : corseattuali) {
        if (corsemarciate.find(pair.first) == corsemarciate.end()) {
            return false;
        }
    }
    return true;
}

void GestioneCorse::ImpostaCorsa() {
    int ult = UltimaCorsaMarciata();
    if (ult > 0)
        corsaattuale = ult;
    else
        corsaattuale = corsainiziale;

    OnAggiornaDatiComanda();
}

std::string GestioneCorse::NomeAltraCorsa(int numeroCorsa) const {
    auto it = altrecorse.find(numeroCorsa);
    if (it != altrecorse.end())
        return it->second;
    return "";
}

std::string GestioneCorse::TestoMessaggio(const std::string& input, int lunghezzaTotale) const {
    int lunginput = static_cast<int>(input.length());
    int lunglati = (lunghezzaTotale - lunginput) / 2;
    if (lunglati < 1) lunglati = 1; // Previene crash per lunghezze negative

    std::string spaziSX(lunglati - 1, ' ');
    std::string spaziDX(lunglati - 1, ' ');

    return "-" + spaziSX + input + spaziDX + "-";
}


// METODI PUBBLICI LOGICI
bool GestioneCorse::SeCorsaMarciabile(int corsa) const {
    return TipoCorsa(corsa) == TipoCorsaEnum::Base || TipoCorsa(corsa) == TipoCorsaEnum::AltraCorsa;
}
bool GestioneCorse::SeCorsaIterabile(int corsa) const {
    return TipoCorsa(corsa) == TipoCorsaEnum::Base;
}
bool GestioneCorse::SeCorsaUsabile(int corsa) const {
    return TipoCorsa(corsa) == TipoCorsaEnum::Base || TipoCorsa(corsa) == TipoCorsaEnum::AltraCorsa;
}
bool GestioneCorse::SeCorsaNonGestita(int corsa) const {
    return TipoCorsa(corsa) == TipoCorsaEnum::NonGestita;
}

int GestioneCorse::UltimaCorsaMarciataGlobale() const {
    int max_val = 0;
    for (int c : corsemarciate) {
        if (SeCorsaUsabile(c) && c > max_val) max_val = c;
    }
    return max_val;
}

int GestioneCorse::UltimaCorsaMarciata() const {
    int max_val = 0;
    for (int c : corsemarciate) {
        if (SeCorsaIterabile(c) && c > max_val) max_val = c;
    }
    return max_val;
}

bool GestioneCorse::SeCorsaUsata(int corsa) const {
    return corseattuali.find(corsa) != corseattuali.end();
}
bool GestioneCorse::SeCorsaMarciata(int corsa) const {
    return corsemarciate.find(corsa) != corsemarciate.end();
}

bool GestioneCorse::SeCorsaBloccata(int corsa) const {
    int ultcorsamarciata = UltimaCorsaMarciata();

    for (const auto& pair : corseattuali) {
        int c = pair.first;
        if (c == corsa) {
            // Se non è iterabile, viene rimossa in C#, quindi per noi è "bloccata" (= true)
            if (!SeCorsaIterabile(c)) return true;
            // Se è marciata ed è diversa dall'ultima corsa marciata, viene rimossa (= true)
            if (SeCorsaMarciata(c) && (c != ultcorsamarciata)) return true;
            // Altrimenti non è bloccata
            return false;
        }
    }
    return true; // Se la corsa non è tra le corse usate, è da considerarsi "bloccata" (rimossa dalla logica)
}

bool GestioneCorse::SeImmediata(int corsa) const {
    return (SeCorsaMarciata(corsa) || SePrimaCorsa(corsa)) || SeCorsaNonGestita(corsa);
}

bool GestioneCorse::SeCorsaAggiuntaAComandaFinita(int corsa) const {
    return (SeTutteMarciate() && SeCorsaMarciabile(corsa) && !SeCorsaUsata(corsa)) && !SePrimaCorsa(corsa);
}

int GestioneCorse::NextMarcia() const {
    if (corseattuali.empty()) return 0;

    int min_val = -1;
    int ult = UltimaCorsaMarciataGlobale();

    for (const auto& pair : corseattuali) {
        int c = pair.first;
        if (c > ult) {
            if (min_val == -1 || c < min_val) {
                min_val = c;
            }
        }
    }
    return (min_val == -1) ? 0 : min_val;
}


// METODI PUBBLICI - TESTI E STRINGHE
std::string GestioneCorse::OttieniStringaMarciate() const {
    if (corsemarciate.empty()) {
        return "Nessuna Marciata";
    }

    if (SeTutteMarciate() && !SeCorsaUsata(corsaattuale)) {
        return "Tutte Marciate";
    }

    std::string result = "";
    bool first = true;

    // In C# i Set non sono ordinati, per mantenere un output pulito in C++ ordiniamo la visualizzazione
    std::vector<int> marciate_ordinate(corsemarciate.begin(), corsemarciate.end());
    std::sort(marciate_ordinate.begin(), marciate_ordinate.end());

    for (int v : marciate_ordinate) {
        if (!first) result += ", ";

        if (TipoCorsa(v) == TipoCorsaEnum::AltraCorsa) {
            result += NomeAltraCorsa(v);
        }
        else {
            result += std::to_string(v);
        }
        first = false;
    }

    return "Marciate: " + result;
}

std::vector<std::pair<std::string, std::string>> GestioneCorse::Intestazione() const {
    std::vector<std::pair<std::string, std::string>> intestazione;

    intestazione.push_back({ "", TestoMessaggio("Corsa " + std::to_string(corsaattuale), 40) });
    intestazione.push_back({ "", TestoMessaggio(OttieniStringaMarciate(), 40) });

    bool immediata = SeImmediata(corsaattuale);
    std::string tipo = immediata ? "3" : "2";
    std::string testo = immediata ? "Immediata" : "A Seguire";

    intestazione.push_back({ tipo, TestoMessaggio(testo, 40) });

    return intestazione;
}

std::string GestioneCorse::DatiIntestazione() const {
    std::string strMarciate = "";
    bool first = true;
    for (int c : corsemarciate) {
        if (!first) strMarciate += ",";
        strMarciate += std::to_string(c);
        first = false;
    }
    return strMarciate;
}

// METODI PUBBLICI - AGGIORNAMENTO DATI
void GestioneCorse::PulisciCorseAttuali() {
    corseattuali.clear();
}

void GestioneCorse::AggiungiCorsaAttuale(int corsa, int link) {
    corseattuali[corsa].push_back(link);
}

void GestioneCorse::FineAggiornamentoCorse() {
    if (corseattuali.size() == 1) {
        int primaCorsa = corseattuali.begin()->first;
        if (SeCorsaMarciabile(primaCorsa) && !SeCorsaMarciata(primaCorsa)) {
            AddMarciata(primaCorsa);
            return; // AddMarciata lancia già OnAggiornaDatiComanda
        }
    }
    OnAggiornaDatiComanda();
}

void GestioneCorse::SetCorseMarciate(const std::string& stringaDati) {
    corsemarciate.clear();

    if (!stringaDati.empty()) {
        std::stringstream ss(stringaDati);
        std::string item;
        while (std::getline(ss, item, ',')) {
            try {
                corsemarciate.insert(std::stoi(item));
            }
            catch (...) { /* Ignora se la conversione fallisce */ }
        }
    }

    // RemoveWhere(c => !SeCorsaMarciabile(c))
    for (auto it = corsemarciate.begin(); it != corsemarciate.end(); ) {
        if (!SeCorsaMarciabile(*it)) {
            it = corsemarciate.erase(it);
        }
        else {
            ++it;
        }
    }

    ImpostaCorsa();
}

void GestioneCorse::SetCorsa(int corsa) {
    if (SeCorsaIterabile(corsa)) {
        corsaattuale = corsa;
        OnAggiornaDatiComanda();
    }
}

void GestioneCorse::SetNextCorsa() {
    std::vector<int> listaCorse;

    // Ricrea il filtraggio C# (rimuovi non iterabili, rimuovi bloccate)
    for (const auto& pair : corseattuali) {
        int c = pair.first;
        if (SeCorsaIterabile(c) && !SeCorsaBloccata(c)) {
            listaCorse.push_back(c);
        }
    }

    if (listaCorse.empty()) return;

    // Poiché C# ottiene la lista dai Keys del Dictionary (ordinamento non garantito matematicamente ma solitamente crescente),
    // la ordiniamo per avere un comportamento coerente con "Next"
    std::sort(listaCorse.begin(), listaCorse.end());

    auto it = std::find(listaCorse.begin(), listaCorse.end(), corsaattuale);
    if (it != listaCorse.end()) {
        int index = std::distance(listaCorse.begin(), it);
        if (index == listaCorse.size() - 1) {
            // È l'ultima, passa alla successiva calcolata matematicamente
            corsaattuale = listaCorse[index] + 1;
            if (corsaattuale > corsalimite) {
                corsaattuale = listaCorse[0];
            }
        }
        else {
            // Prendi la successiva nell'elenco
            corsaattuale = listaCorse[index + 1];
        }
    }

    OnAggiornaDatiComanda();
}

void GestioneCorse::AddMarciata(int corsa) {
    if (SeCorsaMarciabile(corsa)) {
        corsemarciate.insert(corsa);
        OnAggiungiMarciato(corsa);
        OnAggiornaDatiComanda();
    }
}

void GestioneCorse::RemoveMarciata(int corsa) {
    corsemarciate.erase(corsa);
}

void GestioneCorse::Stampata() {
    inviatastampa = true;
}

// --- ESPORTAZIONE PER C# (API "PIATTA") ---

extern "C" {
    // 1. Crea l'oggetto in memoria e restituisce il puntatore (IntPtr in C#)
    __declspec(dllexport) void* GC_Crea(int corsaIniz, int corsaLim) {
        // Creiamo la classe con una mappa vuota iniziale
        std::unordered_map<int, std::string> mapVuota;
        return new GestioneCorse(corsaIniz, corsaLim, mapVuota);
    }

    // 2. Distrugge l'oggetto per liberare la memoria RAM
    __declspec(dllexport) void GC_Distruggi(void* istanza) {
        if (istanza != nullptr) {
            delete static_cast<GestioneCorse*>(istanza);
        }
    }

    // 3. Funzione per permettere a C# di passare il Dictionary "AltreCorse"
    // NOTA: Devi aggiungere un metodo pubblico "void AggiungiAltraCorsa(int chiave, const std::string& valore)" nel tuo GestioneCorse.h!
    /*
    // Aggiungi questo in GestioneCorse.h nei Metodi Pubblici:
    // void AggiungiAltraCorsa(int chiave, const std::string& valore) { altrecorse[chiave] = valore; }
    */
    __declspec(dllexport) void GC_AggiungiAltraCorsa(void* istanza, int chiave, const char* valore) {
        GestioneCorse* gc = static_cast<GestioneCorse*>(istanza);
        gc->AggiungiAltraCorsa(chiave, std::string(valore));
    }

    // 4. Esempio per chiamare un metodo normale
    __declspec(dllexport) int GC_OttieniCorsaAttuale(void* istanza) {
        GestioneCorse* gc = static_cast<GestioneCorse*>(istanza);
        return gc->CorsaAttuale();
    }
}