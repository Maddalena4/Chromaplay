#pragma once
#include "screenshot.h"
#include <sstream>
#include <iomanip>
#include <vector>
#include <string>
#include <direct.h>

extern int   cvdMode;
extern float cvdSeverity;
extern int   width, height;

void renderizza(float currentTime);

struct CVDEntry {
    int         mode;
    std::string nome;
    bool        sweep;   
};

static const std::vector<CVDEntry> CVD_CONFIG = {
    {  0, "normale",         false },
    {  1, "protanopia",      false },
    {  3, "deuteranopia",    false },
    {  5, "tritanopia",      false },
    {  7, "protanomalia",    true  },
    {  9, "deuteranomalia",  true  },
    { 11, "tritanomalia",    true  },
};

/**
 * Genera automaticamente l'intero dataset di immagini per
 * l'addestramento e la validazione del sistema.
 *
 * La funzione esegue il rendering della scena per tutte le
 * configurazioni di visione normale, dicromatismi completi
 * e anomalie cromatiche con severità comprese tra 1% e 100%.
 *
 * Le immagini vengono salvate nella cartella "dataset".
 *
 * @param currentTime Tempo corrente utilizzato dalla funzione
 *                    di rendering.
 *
 * @return void
 */
inline void cattura_dataset(float currentTime)
{
    _mkdir("dataset");

    const int   savedMode = cvdMode;
    const float savedSeverity = cvdSeverity;

    int totale = 0;

    for (const auto& cfg : CVD_CONFIG)
    {
        cvdMode = cfg.mode;

        if (!cfg.sweep)
        {
            cvdSeverity = 1.0f;
            renderizza(currentTime);

            std::string path = "dataset/" + cfg.nome + ".png";
            salvaScreenshot(path, width, height);
            totale++;
        }
        else
        {
            
            printf("[Dataset] Inizio sweep: %s (100 screenshot)...\n",
                cfg.nome.c_str());

            for (int pct = 1; pct <= 100; pct++)
            {
                cvdSeverity = (float)pct / 100.0f;
                renderizza(currentTime);

                std::ostringstream oss;
                oss << "dataset/"
                    << cfg.nome
                    << "_"
                    << std::setw(3) << std::setfill('0') << pct
                    << ".png";

                salvaScreenshot(oss.str(), width, height);
                totale++;
            }

            printf("[Dataset] %s completata.\n", cfg.nome.c_str());
        }
    }

    cvdMode = savedMode;
    cvdSeverity = savedSeverity;
    renderizza(currentTime);

    printf("\n[Dataset] ===== COMPLETATO: %d immagini in ./dataset/ =====\n",
        totale);
}