#pragma once
#include <unordered_set>
#include <unordered_map>
#include <vector>
#include <string>
#include <functional>
#include <utility>


class GestioneCorse
{
public:
    // Tipi per gli Eventi (Callbacks)
    using AggiornaDatiComandaCallback = std::function<void()>;
    using AggiungiMarciatoCallback = std::function<void(int)>;

    // Costruttore
    GestioneCorse(int corsainiz, int corsalim, const std::unordered_map<int, std::string>& paraltrecorse);

    // Getters per le proprietà pubbliche
    int GetCorsaIniziale() const { return corsainiziale; }
    int GetCorsaLimite() const { return corsalimite; }
    int CorsaAttuale() const { return corsaattuale; }
    bool GetInviataStampa() const { return inviatastampa; }

    // Registrazione Eventi
    void SetAggiornaDatiComandaCallback(AggiornaDatiComandaCallback callback);
    void SetAggiungiMarciatoCallback(AggiungiMarciatoCallback callback);

    // Metodi Pubblici
    bool SeCorsaMarciabile(int corsa) const;
    bool SeCorsaIterabile(int corsa) const;
    bool SeCorsaUsabile(int corsa) const;
    bool SeCorsaNonGestita(int corsa) const;

    int UltimaCorsaMarciataGlobale() const;
    int UltimaCorsaMarciata() const;

    bool SeCorsaUsata(int corsa) const;
    bool SeCorsaMarciata(int corsa) const;
    bool SeCorsaBloccata(int corsa) const;

    bool SeImmediata(int corsa) const;
    bool SeCorsaAggiuntaAComandaFinita(int corsa) const;

    int NextMarcia() const;
    std::string OttieniStringaMarciate() const;
    std::string DatiIntestazione() const;
    std::vector<std::pair<std::string, std::string>> Intestazione() const;

    // Metodi in sostituzione di AggiornaCorse (per svincolarsi da Micros POS)
    void PulisciCorseAttuali();
    void AggiungiCorsaAttuale(int corsa, int link);
    void FineAggiornamentoCorse();

    void SetCorseMarciate(const std::string& stringaDati);
    void SetCorsa(int corsa);
    void SetNextCorsa();
    void AddMarciata(int corsa);
    void RemoveMarciata(int corsa);
    void Stampata();

    void AggiungiAltraCorsa(int chiave, const std::string& valore);

private:
    int corsainiziale;
    int corsalimite;
    int corsaattuale;
    std::unordered_set<int> corsemarciate;
    std::unordered_map<int, std::vector<int>> corseattuali;
    std::unordered_map<int, std::string> altrecorse;
    bool inviatastampa;

    // Callbacks degli eventi
    AggiornaDatiComandaCallback onAggiornaDatiComanda;
    AggiungiMarciatoCallback onAggiungiMarciato;

    enum class TipoCorsaEnum
    {
        NoCourse = 0,
        Base = 1,
        AltraCorsa = 2,
        NonGestita = 3
    };

    // Metodi Privati
    TipoCorsaEnum TipoCorsa(int corsa) const;
    bool SePrimaCorsa(int corsa) const;
    int UltimaCorsaUsata() const;
    bool SeTutteMarciate() const;
    void ImpostaCorsa();
    std::string NomeAltraCorsa(int numeroCorsa) const;
    std::string TestoMessaggio(const std::string& input, int lunghezzaTotale) const;

    // Wrapper per lanciare gli eventi
    void OnAggiornaDatiComanda();
    void OnAggiungiMarciato(int corsa);
};