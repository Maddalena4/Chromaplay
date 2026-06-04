#pragma once
#include <array>
#include <algorithm>
#include "StaircaseAlgorithm.h"
#include "ColorMath.h"

struct TestSession {

    std::array<double, 3> thresholds = { 0.0, 0.0, 0.0 };

    std::array<int, 3> percents = { 0, 0, 0 };

    int dominantType = -1;  
    int dominantPercent = 0;

    int completedAxes = 0;

    /**
      * Registra il risultato finale di uno staircase completato.
      *
      * PARAM:
      *  - cvdType
      *      0 = Protan
      *      1 = Deutan
      *      2 = Tritan
      *
      *  - threshold
      *      soglia finale stimata dallo staircase
      *
      * RETURN:
      *  void
      *
      * Salva la soglia nell'array thresholds e incrementa
      * il numero di assi completati.
      */
    void registerAxisResult(int cvdType, double threshold) {
        thresholds[cvdType] = threshold;
        completedAxes++;
    }

    /**
     * Verifica se tutti gli assi del test sono stati completati.
     *
     * RETURN:
     *  true  -> tutti i 3 assi terminati
     *  false -> almeno un asse ancora in esecuzione
     */
    bool isComplete() const { return completedAxes >= 3; }

    /**
     * Calcola le percentuali di deficit cromatico
     * partendo dalle soglie ottenute dagli staircase.
     *
     * RETURN:
     *  void
     *
     * Utilizza una trasformazione logaritmica coerente
     * con la legge di Weber-Fechner:
     *
     *  soglia bassa  -> deficit basso
     *  soglia alta   -> deficit elevato
     *
     * Al termine individua anche il tipo dominante.
     */
    void computeEstimates() {
        for (int i = 0; i < 3; ++i)
            percents[i] = thresholdToPercent(thresholds[i]);

        auto it = std::max_element(percents.begin(), percents.end());
        dominantType = (int)(it - percents.begin());
        dominantPercent = *it;
    }

    /**
     * Converte una soglia staircase in una percentuale
     * di deficit cromatico compresa tra 0 e 100.
     *
     * PARAM:
     *  - thr
     *      soglia finale dello staircase
     *
     * RETURN:
     *  percentuale stimata [0..100]
     *
     * La conversione è effettuata in scala logaritmica:
     *
     *  soglia piccola -> deficit basso
     *  soglia grande  -> deficit elevato
     */
    static int thresholdToPercent(double thr) {
        constexpr double tMin = StaircaseAlgorithm::DELTA_MIN;
        constexpr double tMax = StaircaseAlgorithm::DELTA_MAX;
        double logT = std::log(std::max(thr, tMin));
        double logMin = std::log(tMin);
        double logMax = std::log(tMax);
        double pct = (logT - logMin) / (logMax - logMin) * 100.0;
        return (int)std::max(0.0, std::min(100.0, pct));
    }

    /**
     * Restituisce una descrizione testuale
     * della severità del deficit.
     *
     * PARAM:
     *  - pct
     *      percentuale di deficit
     *
     * RETURN:
     *  "Normale"
     *  "Lieve"
     *  "Moderato"
     *  "Grave"
     */
    static const char* severityLabel(int pct) {
        if (pct < 5) return "Normale";
        if (pct < 30) return "Lieve";
        if (pct < 65) return "Moderato";
        return               "Grave";
    }

    /**
     * Ripristina completamente lo stato della sessione.
     *
     * RETURN:
     *  void
     *
     * Azzera:
     *  - soglie
     *  - percentuali
     *  - tipo dominante
     *  - percentuale dominante
     *  - numero di assi completati
     */
    void reset() {
        thresholds.fill(0.0);
        percents.fill(0);
        dominantType = -1;
        dominantPercent = 0;
        completedAxes = 0;
    }
};