#pragma once
#include <vector>
#include <cmath>
#include <algorithm>


class StaircaseAlgorithm {
public:

    static constexpr int    N_REVERSALS = 14;
    static constexpr int    REVERSALS_USED = 8;
    static constexpr double DELTA_INIT = 0.22;
    static constexpr double DELTA_MIN = 0.020;
    static constexpr double DELTA_MAX = 0.28;
    static constexpr double DELTA_NORMAL_CEILING = 0.025;

    enum class State { RUNNING, FINISHED };

    StaircaseAlgorithm()
        : delta_(DELTA_INIT)
        , streak_(0)
        , nReversals_(0)
        , lastDir_(0)
        , state_(State::RUNNING)
        , forcedStop_(false)
        , normalCeilingHit_(false)
    {}

    /**
     * Registra una risposta dell'utente utilizzando il
     * comportamento standard dello staircase.
     *
     * Internamente richiama la versione adattiva utilizzando
     * un fattore di scala pari a 1.0.
     *
     * @param correct true se la risposta è corretta,
     *                false altrimenti.
     *
     * @return void
     */
    void registerResponse(bool correct) {
        registerResponseAdaptive(correct, 1.0);
    }

    /**
     * Aggiorna lo staircase sulla base della risposta fornita.
     *
     * Se l'utente risponde correttamente per tre volte
     * consecutive la difficoltà aumenta riducendo il delta.
     * In caso di errore il delta viene incrementato.
     *
     * Il processo continua fino al raggiungimento del numero
     * previsto di inversioni oppure della soglia di arresto
     * per visione normale.
     *
     * @param correct Esito della risposta.
     * @param deltaScale Fattore moltiplicativo applicato
     *                   alla variazione del delta.
     *
     * @return void
     */
    void registerResponseAdaptive(bool correct, double deltaScale) {
        if (state_ == State::FINISHED) return;

        history_.push_back(correct);

        if (correct) {
            streak_++;
            if (streak_ >= 3) {
                streak_ = 0;
                applyStep(delta_ * (0.70 * deltaScale), -1);
            }
        }
        else {
            streak_ = 0;
            applyStep(delta_ * (1.43 * deltaScale), +1);
        }

        if (state_ == State::RUNNING && delta_ < DELTA_NORMAL_CEILING) {
            normalCeilingHit_ = true;
            state_ = State::FINISHED;
        }
    }

    /**
     * Forza la conclusione immediata dell'asse corrente.
     *
     * Il valore corrente del delta viene salvato come ultima
     * misura disponibile prima della terminazione.
     *
     * @param Nessuno.
     *
     * @return void
     */
    void forceFinish() {
        if (state_ == State::FINISHED) return;
        // Salva il delta corrente come ultima "inversione"
        reversalDeltas_.push_back(delta_);
        forcedStop_ = true;
        state_ = State::FINISHED;
    }

    /**
     * Restituisce il valore corrente del delta utilizzato
     * nello staircase.
     *
     * @param Nessuno.
     *
     * @return Delta corrente.
     */
    double currentDelta()     const { return delta_; }

     /**
     * Restituisce lo stato attuale dell'algoritmo.
     *
     * @param Nessuno.
     *
     * @return Stato RUNNING oppure FINISHED.
     */
    State  state()            const { return state_; }

    /**
     * Restituisce il numero totale di trial eseguiti.
     *
     * @param Nessuno.
     *
     * @return Numero di trial registrati.
     */
    int    trialCount()       const { return (int)history_.size(); }

    /**
     * Restituisce il numero di inversioni rilevate
     * durante lo staircase.
     *
     * @param Nessuno.
     *
     * @return Numero di reversal.
     */
    int    reversalCount()    const { return nReversals_; }
    bool   wasForcedStop()    const { return forcedStop_; }
    bool   wasNormalCeiling() const { return normalCeilingHit_; }

    /**
      * Calcola la percentuale di avanzamento del test.
      *
      * La stima è basata sul numero di inversioni già
      * osservate rispetto al numero totale previsto.
      *
      * @param Nessuno.
      *
      * @return Percentuale di completamento [0-100].
      */
    int progressPercent() const {
        if (forcedStop_ || normalCeilingHit_) return 100;
        return std::min(100, nReversals_ * 100 / N_REVERSALS);
    }

    /**
     * Calcola la soglia percettiva finale dell'asse.
     *
     * La soglia viene ottenuta come media delle ultime
     * inversioni considerate statisticamente stabili.
     *
     * @param Nessuno.
     *
     * @return Valore finale della soglia.
     */
    double threshold() const {
        if (normalCeilingHit_) return delta_;
        if (reversalDeltas_.empty()) return delta_;
        int n = std::min((int)reversalDeltas_.size(), REVERSALS_USED);
        int from = (int)reversalDeltas_.size() - n;
        double sum = 0;
        for (int i = from; i < (int)reversalDeltas_.size(); ++i)
            sum += reversalDeltas_[i];
        return sum / n;
    }

    const std::vector<bool>& history()         const { return history_; }
    const std::vector<double>& reversalDeltas()  const { return reversalDeltas_; }

private:
    double delta_;
    int    streak_;
    int    nReversals_;
    int    lastDir_;
    State  state_;
    bool   forcedStop_;
    bool   normalCeilingHit_;

    std::vector<bool>   history_;
    std::vector<double> reversalDeltas_;

    /**
     * Applica un nuovo valore di delta allo staircase.
     *
     * La funzione aggiorna la direzione del movimento,
     * rileva eventuali inversioni e verifica la condizione
     * di terminazione dell'algoritmo.
     *
     * @param newDelta Nuovo valore di delta.
     * @param newDir Direzione dello step:
     *               -1 = diminuzione,
     *               +1 = aumento.
     *
     * @return void
     */
    void applyStep(double newDelta, int newDir) {
        if (lastDir_ != 0 && newDir != lastDir_) {
            reversalDeltas_.push_back(delta_);
            nReversals_++;
            if (nReversals_ >= N_REVERSALS) {
                state_ = State::FINISHED;
                return;
            }
        }
        lastDir_ = newDir;
        delta_ = std::max(DELTA_MIN, std::min(DELTA_MAX, newDelta));
    }
};