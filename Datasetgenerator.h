#pragma once
#include <vector>
#include <random>
#include <cmath>
#include <algorithm>
#include <fstream>
#include <string>
#include "StaircaseAlgorithm.h"
#include "ColorMath.h"

struct DataSample {
    double thresholds[3];
    int    cvdType;        
    int    percent;         
};

class DatasetGenerator {
public:

    static const int N_VARIANTS = 20;
    static constexpr double NOISE_SIGMA = 0.004;

    /**
     * Genera il dataset sintetico utilizzato per l'addestramento
     * della rete neurale MLP.
     *
     * Il dataset contiene esempi di visione normale e di deficit
     * cromatici Protan, Deutan e Tritan con severità comprese tra
     * 1% e 100%.
     *
     * @param totalSamples Puntatore opzionale in cui memorizzare
     *                     il numero totale di campioni generati.
     *
     * @return Vettore contenente tutti i campioni generati.
     */
    std::vector<DataSample> generate(int* totalSamples = nullptr) {
        std::vector<DataSample> dataset;
        std::mt19937 rng(42);
        std::normal_distribution<double> noise(0.0, NOISE_SIGMA);

        
        const int N_NORMAL = 1200;
        for (int v = 0; v < N_NORMAL; ++v) {
            DataSample s;
            s.cvdType = 0;   
            s.percent = 0;
            for (int axis = 0; axis < 3; ++axis) {
                double base = sensitivityToThreshold(1.0);
                s.thresholds[axis] = clamp(base + noise(rng));
            }
            dataset.push_back(s);
        }

       
        for (int type = 0; type < 3; ++type) {
            for (int pct = 1; pct <= 100; ++pct) {
                double severity = pct / 100.0;
                double sensMain = computeSensitivity(severity);

                for (int v = 0; v < N_VARIANTS; ++v) {
                    DataSample s;
                    s.cvdType = type;
                    s.percent = pct;

                    for (int axis = 0; axis < 3; ++axis) {
                        double sens = (axis == type) ? sensMain : 1.0;
                        double base = sensitivityToThreshold(sens);
                        s.thresholds[axis] = clamp(base + noise(rng));
                    }
                    dataset.push_back(s);
                }
            }
        }

        std::shuffle(dataset.begin(), dataset.end(), rng);
        if (totalSamples) *totalSamples = (int)dataset.size();
        return dataset;
    }

    /**
      * Converte un valore di sensibilità percettiva nella soglia
      * attesa misurata tramite algoritmo staircase.
      *
      * La funzione utilizza un'interpolazione lineare tra punti
      * ottenuti da calibrazione sperimentale.
      *
      * @param sens Sensibilità normalizzata [0.05, 1.0].
      *
      * @return Soglia staircase corrispondente.
      */
    static double sensitivityToThreshold(double sens) {
        sens = std::max(0.05, std::min(1.0, sens));
        static const double S[6] = { 0.05, 0.20, 0.40, 0.60, 0.80, 1.00 };
        static const double T[6] = { 0.227, 0.226, 0.225, 0.197, 0.153, 0.119 };
        for (int i = 0; i < 5; ++i) {
            if (sens >= S[i] && sens <= S[i + 1]) {
                double t = (sens - S[i]) / (S[i + 1] - S[i]);
                return T[i] + t * (T[i + 1] - T[i]);
            }
        }
        return T[5];
    }

    /**
     * Salva il dataset in formato CSV.
     *
     * Ogni riga contiene:
     * - soglia Protan
     * - soglia Deutan
     * - soglia Tritan
     * - tipo di deficit
     * - severità
     *
     * @param dataset Dataset da salvare.
     * @param path Percorso del file CSV.
     *
     * @return void
     */
    static void saveCSV(const std::vector<DataSample>& dataset,
        const std::string& path = "dataset.csv") {
        std::ofstream f(path);
        if (!f.is_open()) return;
        f << "thresh_P,thresh_D,thresh_T,cvd_type,percent\n";
        for (const auto& s : dataset)
            f << s.thresholds[0] << "," << s.thresholds[1] << ","
            << s.thresholds[2] << "," << s.cvdType << ","
            << s.percent << "\n";
    }

private:

    /**
     * Calcola la sensibilità residua associata ad una data severità.
     *
     * Modella la perdita di sensibilità cromatica tramite una
     * funzione non lineare con andamento saturante.
     *
     * @param severity Severità del deficit [0,1].
     *
     * @return Sensibilità normalizzata.
     */
    static double computeSensitivity(double severity) {
        double s = 1.0 - severity * (0.90 + 0.05 * severity);
        return std::max(0.05, std::min(1.0, s));
    }

    /**
     * Limita un valore all'intervallo consentito dallo staircase.
     *
     * @param v Valore da limitare.
     *
     * @return Valore compreso tra DELTA_MIN e DELTA_MAX.
     */
    static double clamp(double v) {
        return std::max(StaircaseAlgorithm::DELTA_MIN,
            std::min(StaircaseAlgorithm::DELTA_MAX, v));
    }
};