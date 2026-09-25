#pragma once

#include <vector>
#include <string>

class RunPassLogic {
private:
    long long chiave_molt;
    long long chiave_add;
    long long modulo;
    int id_applicazione;
    bool licenza_valida;
    int cliente_id;
    int mese;
    int anno;

    // Funzioni di supporto matematico interno
    long long calcolaInversoModulare(long long a, long long m);
    long long moltiplicazioneModulare(long long a, long long b, long long m);

public:
    // Costruttore: riceve le chiavi e il codice di licenza da validare subito
    RunPassLogic(const char* codice, long long molt, long long add, long long mod, int idApp, long idverifica);

    // Metodi di validazione e controllo stato
    bool validaLicenza(const char* codice, const long idverifica);
    bool IsLicenzaValida() const;
    int GetMese() const;
    int GetAnno() const;

    // Logica CCS
    int EseguiCCS(int ci, int cl, int ca, const std::vector<int>& cm, const std::vector<int>& cu);

    // Logica TPNM
    int EseguiTPNM(const std::vector<int>& cm, const std::vector<int>& cu);
};