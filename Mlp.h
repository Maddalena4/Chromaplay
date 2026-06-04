/**
 * @class MLP
 *
 * Implementazione di una rete neurale Multi-Layer Perceptron
 * utilizzata per classificare il deficit cromatico a partire
 * dalle soglie percettive ottenute tramite staircase.
 *
 * Input:
 * - soglia Protan;
 * - soglia Deutan;
 * - soglia Tritan;
 * - differenze tra le soglie.
 *
 * Output:
 * - tipologia di deficit cromatico;
 * - percentuale stimata di severità.
 *
 * Architettura:
 * 6 input ? 32 neuroni ? 16 neuroni ? output.
 */
#pragma once
#include <vector>
#include <array>
#include <cmath>
#include <random>
#include <algorithm>
#include <fstream>
#include <functional>
#include <string>
#include "DatasetGenerator.h"

static inline double relu(double x) { return x > 0.0 ? x : 0.0; }
static inline double reluGrad(double x) { return x > 0.0 ? 1.0 : 0.0; }
static inline double sigmoid(double x) { return 1.0 / (1.0 + std::exp(-x)); }
static inline double sigmoidGrad(double x) { double s = sigmoid(x); return s * (1.0 - s); }

static inline void softmax(std::vector<double>& v) {
    double mx = *std::max_element(v.begin(), v.end());
    double sum = 0.0;
    for (auto& x : v) { x = std::exp(x - mx); sum += x; }
    for (auto& x : v)   x /= sum;
}

struct Layer {
    int inSize, outSize;
    std::vector<std::vector<double>> W;
    std::vector<double> b, z, a, dz;

    Layer() {}
    Layer(int in, int out, std::mt19937& rng) : inSize(in), outSize(out) {
        double stddev = std::sqrt(2.0 / in);
        std::normal_distribution<double> dist(0.0, stddev);
        W.assign(out, std::vector<double>(in));
        b.assign(out, 0.0);
        z.assign(out, 0.0); a.assign(out, 0.0); dz.assign(out, 0.0);
        for (auto& row : W) for (auto& w : row) w = dist(rng);
    }

    void forward(const std::vector<double>& input, bool useReLU) {
        for (int o = 0; o < outSize; ++o) {
            z[o] = b[o];
            for (int i = 0; i < inSize; ++i) z[o] += W[o][i] * input[i];
            a[o] = useReLU ? relu(z[o]) : z[o];
        }
    }
};

struct MLPResult {
    int   cvdType;    
    int   percent;    
    float confidence; 
};

class MLP {
public:
    
    static constexpr double LR_INIT = 0.005;
    static constexpr double LR_DECAY = 0.992;
    static constexpr double LR_MIN = 0.0003;
    static constexpr int    EPOCHS = 200;
    static constexpr int    BATCH_SIZE = 64;

    static constexpr double FEAT_MIN = StaircaseAlgorithm::DELTA_MIN;
    static constexpr double FEAT_MAX = StaircaseAlgorithm::DELTA_MAX;
    static constexpr double FEAT_RANGE = FEAT_MAX - FEAT_MIN;
    static constexpr int    N_INPUTS = 6;

    MLP() : trained_(false) {}

    /**
     * Inizializza la rete neurale.
     *
     * Crea i layer e inizializza pesi e bias tramite
     * distribuzione casuale controllata.
     *
     * @param seed Seme del generatore pseudo-casuale.
     *
     * @return void
     */
    void init(unsigned int seed = 42) {
        std::mt19937 rng(seed);
        l1_ = Layer(N_INPUTS, 32, rng);
        l2_ = Layer(32, 16, rng);
        lType_ = Layer(16, 4, rng);  
        lPct_ = Layer(16, 1, rng);
        trained_ = false;
    }
    
    /**
     * Costruisce il vettore delle feature utilizzate dal modello.
     *
     * A partire dalle tre soglie staircase vengono generate:
     * - le tre soglie normalizzate;
     * - le tre differenze tra le soglie.
     *
     * @param thr Array contenente le tre soglie.
     *
     * @return Vettore di 6 feature normalizzate.
     */
    static std::vector<double> buildFeatures(const double thr[3]) {
        double P = thr[0], D = thr[1], T = thr[2];
        auto n = [](double x) {
            return (x - FEAT_MIN) / FEAT_RANGE;   
            };
        auto d = [](double x) {
            return std::max(-1.0, std::min(1.0, x / FEAT_RANGE));
            };
        return { n(P), n(D), n(T), d(P - D), d(P - T), d(D - T) };
    }

    /**
     * Esegue la propagazione in avanti della rete neurale.
     *
     * Calcola le attivazioni di tutti i layer e produce
     * le probabilità di classificazione e la stima
     * della severità.
     *
     * @param input Array contenente le tre soglie percettive.
     *
     * @return void
     */
    void forward(const double input[3]) {
        auto feat = buildFeatures(input);
        l1_.forward(feat, true);
        l2_.forward(l1_.a, true);
        lType_.forward(l2_.a, false);
        lPct_.forward(l2_.a, false);
        softmax(lType_.a);
        lPct_.a[0] = sigmoid(lPct_.z[0]);
    }

    /**
     * Effettua la classificazione finale del soggetto.
     *
     * Determina:
     * - la classe più probabile;
     * - la severità stimata;
     * - il livello di confidenza della previsione.
     *
     * @param thr Array contenente le tre soglie percettive.
     *
     * @return Struttura MLPResult contenente il risultato.
     */
    MLPResult predict(const double thr[3]) {
        forward(thr);

        int best = 0;
        for (int i = 1; i < 4; ++i)
            if (lType_.a[i] > lType_.a[best]) best = i;

        MLPResult r;
        r.confidence = (float)lType_.a[best];

        if (best == 0) {
            r.cvdType = 0;
            r.percent = 0;
        }
        else {
            r.cvdType = best - 1;   
            r.percent = std::max(1, std::min(100,
                (int)std::round(lPct_.a[0] * 100.0)));
        }
        return r;
    }

    /**
     * Addestra la rete neurale utilizzando il dataset sintetico.
     *
     * L'addestramento utilizza mini-batch gradient descent,
     * funzione di perdita combinata e backpropagation.
     *
     * @param dataset Dataset utilizzato per il training.
     * @param onEpoch Callback opzionale invocata al termine
     *                di ogni epoca.
     *
     * @return void
     */
    void train(const std::vector<DataSample>& dataset,
        std::function<void(int, double)> onEpoch = nullptr) {
        init();
        std::mt19937 rng(123);
        double lr = LR_INIT;

        std::vector<DataSample> trainSet = dataset;

        for (int epoch = 0; epoch < EPOCHS; ++epoch) {
            std::shuffle(trainSet.begin(), trainSet.end(), rng);
            double epochLoss = 0.0;
            int    batches = 0;

            for (int b = 0; b < (int)trainSet.size(); b += BATCH_SIZE) {
                int end = std::min(b + BATCH_SIZE, (int)trainSet.size());
                clearGradients();
                double batchLoss = 0.0;
                for (int s = b; s < end; ++s) {
                    forward(trainSet[s].thresholds);
                    batchLoss += computeLossAndBackprop(trainSet[s]);
                }
                updateWeights(lr, end - b);
                epochLoss += batchLoss / (end - b);
                batches++;
            }
            lr = std::max(LR_MIN, lr * LR_DECAY);
            if (onEpoch) onEpoch(epoch, epochLoss / batches);
        }
        trained_ = true;
    }

    bool isTrained() const { return trained_; }

    /**
     * Salva la rete neurale su file binario.
     *
     * Vengono memorizzati pesi e bias di tutti i layer.
     *
     * @param path Percorso del file di destinazione.
     *
     * @return true se il salvataggio è riuscito.
     * @return false in caso di errore.
     */
    bool save(const std::string& path) const {
        std::ofstream f(path, std::ios::binary);
        if (!f) return false;
        writeLayer(f, l1_); writeLayer(f, l2_);
        writeLayer(f, lType_); writeLayer(f, lPct_);
        return true;
    }

    /**
     * Carica una rete neurale precedentemente salvata.
     *
     * Ripristina pesi, bias e struttura dei layer.
     *
     * @param path Percorso del file da leggere.
     *
     * @return true se il caricamento è riuscito.
     * @return false in caso di errore.
     */
    bool load(const std::string& path) {
        std::ifstream f(path, std::ios::binary);
        if (!f) return false;
        init();
        readLayer(f, l1_); readLayer(f, l2_);
        readLayer(f, lType_); readLayer(f, lPct_);
        trained_ = true;
        return true;
    }

private:
    Layer l1_, l2_, lType_, lPct_;
    bool  trained_;

    std::vector<std::vector<std::vector<double>>> dW_;

    /**
     * Determina la classe target associata a un campione del dataset.
     *
     * La rete utilizza quattro classi:
     * - 0 = visione normale;
     * - 1 = protanomalia;
     * - 2 = deuteranomalia;
     * - 3 = tritanomalia.
     *
     * Per i soggetti normali (percentuale = 0) viene sempre
     * restituita la classe 0; per gli altri casi la classe
     * corrisponde al tipo di deficit cromatico.
     *
     * @param s Campione del dataset.
     *
     * @return Indice della classe target [0..3].
     */
    static int sampleClass(const DataSample& s) {
        return (s.percent == 0) ? 0 : s.cvdType + 1;
    }

    /**
     * Calcola la funzione di perdita ed esegue la fase di
     * backpropagation della rete neurale.
     *
     * La loss totale è composta da:
     * - perdita di classificazione (cross-entropy);
     * - perdita di regressione della severità (MSE).
     *
     * Durante il calcolo vengono inoltre determinati i gradienti
     * necessari per aggiornare i pesi della rete.
     *
     * @param s Campione utilizzato per l'addestramento.
     *
     * @return Valore totale della loss per il campione corrente.
     */
    double computeLossAndBackprop(const DataSample& s) {
        int cls = sampleClass(s);

        double lossType = -std::log(lType_.a[cls] + 1e-10);
        for (int i = 0; i < 4; ++i)
            lType_.dz[i] = lType_.a[i] - (i == cls ? 1.0 : 0.0);

        double lossPct = 0.0;
        if (s.percent > 0) {
            double target = s.percent / 100.0;
            double pred = lPct_.a[0];
            lossPct = (pred - target) * (pred - target);
            lPct_.dz[0] = 2.0 * (pred - target) * sigmoidGrad(lPct_.z[0]);
        }
        else {
            lPct_.dz[0] = 0.0;   
        }

        std::vector<double> dL2(16, 0.0);
        for (int j = 0; j < 16; ++j) {
            for (int i = 0; i < 4; ++i)  dL2[j] += lType_.W[i][j] * lType_.dz[i];
            dL2[j] += lPct_.W[0][j] * lPct_.dz[0];
            l2_.dz[j] = dL2[j] * reluGrad(l2_.z[j]);
        }

        std::vector<double> dL1(32, 0.0);
        for (int j = 0; j < 32; ++j) {
            for (int i = 0; i < 16; ++i) dL1[j] += l2_.W[i][j] * l2_.dz[i];
            l1_.dz[j] = dL1[j] * reluGrad(l1_.z[j]);
        }

        accumulateGradients(s.thresholds);
        return lossType + lossPct;
    }
    /**
     * Accumula i gradienti dei pesi e dei bias calcolati
     * durante la backpropagation.
     *
     * I gradienti vengono sommati all'interno delle strutture
     * temporanee dW_ e successivamente utilizzati per
     * aggiornare i parametri della rete al termine del batch.
     *
     * @param raw Array contenente le tre soglie percettive
     *            utilizzate come input della rete.
     *
     * @return void
     */
    void accumulateGradients(const double raw[3]) {
        auto feat = buildFeatures(raw);
        for (int o = 0; o < 32; ++o) {
            dW_[0][o][0] += l1_.dz[o];
            for (int i = 0; i < N_INPUTS; ++i)
                dW_[0][o][i + 1] += l1_.dz[o] * feat[i];
        }
        for (int o = 0; o < 16; ++o) {
            dW_[1][o][0] += l2_.dz[o];
            for (int i = 0; i < 32; ++i)
                dW_[1][o][i + 1] += l2_.dz[o] * l1_.a[i];
        }
        for (int o = 0; o < 4; ++o) {
            dW_[2][o][0] += lType_.dz[o];
            for (int i = 0; i < 16; ++i)
                dW_[2][o][i + 1] += lType_.dz[o] * l2_.a[i];
        }
        dW_[3][0][0] += lPct_.dz[0];
        for (int i = 0; i < 16; ++i)
            dW_[3][0][i + 1] += lPct_.dz[0] * l2_.a[i];
    }

    /**
     * Azzera tutte le strutture contenenti i gradienti accumulati.
     *
     * La funzione viene invocata all'inizio di ogni batch di
     * addestramento per evitare che i gradienti di iterazioni
     * precedenti influenzino l'aggiornamento corrente.
     *
     * @param Nessuno.
     *
     * @return void
     */
    void clearGradients() {
        dW_.assign(4, {});
        dW_[0].assign(32, std::vector<double>(N_INPUTS + 1, 0.0));
        dW_[1].assign(16, std::vector<double>(33, 0.0));
        dW_[2].assign(4, std::vector<double>(17, 0.0));
        dW_[3].assign(1, std::vector<double>(17, 0.0));
    }

    /**
     * Aggiorna i pesi e i bias della rete neurale utilizzando
     * i gradienti accumulati nel batch corrente.
     *
     * L'aggiornamento viene effettuato mediante Gradient Descent,
     * scalando i gradienti tramite il learning rate e la
     * dimensione del batch.
     *
     * @param lr Learning rate utilizzato durante l'ottimizzazione.
     * @param batchSize Numero di campioni contenuti nel batch.
     *
     * @return void
     */
    void updateWeights(double lr, int batchSize) {
        double sc = lr / batchSize;
        for (int o = 0; o < 32; ++o) {
            l1_.b[o] -= sc * dW_[0][o][0];
            for (int i = 0; i < N_INPUTS; ++i) l1_.W[o][i] -= sc * dW_[0][o][i + 1];
        }
        for (int o = 0; o < 16; ++o) {
            l2_.b[o] -= sc * dW_[1][o][0];
            for (int i = 0; i < 32; ++i) l2_.W[o][i] -= sc * dW_[1][o][i + 1];
        }
        for (int o = 0; o < 4; ++o) {
            lType_.b[o] -= sc * dW_[2][o][0];
            for (int i = 0; i < 16; ++i) lType_.W[o][i] -= sc * dW_[2][o][i + 1];
        }
        lPct_.b[0] -= sc * dW_[3][0][0];
        for (int i = 0; i < 16; ++i) lPct_.W[0][i] -= sc * dW_[3][0][i + 1];
    }

    /**
     * Salva su file binario i parametri di un layer della rete.
     *
     * Vengono memorizzati:
     * - numero di neuroni di ingresso;
     * - numero di neuroni di uscita;
     * - matrice dei pesi;
     * - vettore dei bias.
     *
     * Utilizzata durante il salvataggio del modello addestrato.
     *
     * @param f Stream binario di output.
     * @param l Layer da serializzare.
     *
     * @return void
     */
    static void writeLayer(std::ofstream& f, const Layer& l) {
        f.write((char*)&l.inSize, sizeof(int));
        f.write((char*)&l.outSize, sizeof(int));
        for (auto& row : l.W) f.write((char*)row.data(), row.size() * sizeof(double));
        f.write((char*)l.b.data(), l.b.size() * sizeof(double));
    }

    /**
     * Carica da file binario i parametri di un layer della rete.
     *
     * La funzione ricostruisce completamente la struttura
     * del layer, allocando memoria per pesi, bias,
     * attivazioni e gradienti.
     *
     * Utilizzata durante il caricamento di un modello
     * precedentemente salvato.
     *
     * @param f Stream binario di input.
     * @param l Layer da deserializzare.
     *
     * @return void
     */
    static void readLayer(std::ifstream& f, Layer& l) {
        f.read((char*)&l.inSize, sizeof(int));
        f.read((char*)&l.outSize, sizeof(int));
        l.W.assign(l.outSize, std::vector<double>(l.inSize));
        l.b.assign(l.outSize, 0.0); l.z.assign(l.outSize, 0.0);
        l.a.assign(l.outSize, 0.0); l.dz.assign(l.outSize, 0.0);
        for (auto& row : l.W) f.read((char*)row.data(), row.size() * sizeof(double));
        f.read((char*)l.b.data(), l.b.size() * sizeof(double));
    }
};