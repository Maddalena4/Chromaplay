#pragma once
#include "TestSession.h"
#include "StaircaseAlgorithm.h"
#include "ColorMath.h"
#include "DatasetGenerator.h"
#include "MLP.h"

#include <array>
#include <thread>
#include <atomic>
#include <mutex>
#include <string>

enum class AppScreen { WELCOME, TRAINING, TESTING, RESULTS };
enum class FeedbackState { NONE, CORRECT, WRONG };

struct TrialColors {
    ColorMath::RGBf32 c[3];
    int oddIdx = 0;
};

struct TrainingState {
    std::atomic<bool> running{ false };
    std::atomic<bool> done{ false };
    std::atomic<int>  epoch{ 0 };
    std::atomic<double> loss{ 0.0 };
    std::atomic<int>  progress{ 0 };
    bool success = false;
    std::string message;
    std::mutex msgMutex;

    void setMessage(const std::string& m) {
        std::lock_guard<std::mutex> lk(msgMutex);
        message = m;
    }
    std::string getMessage() {
        std::lock_guard<std::mutex> lk(msgMutex);
        return message;
    }
};

struct AppState {
    AppScreen    screen = AppScreen::WELCOME;
    MLP          mlp;
    TrainingState training;
    bool         mlpReady = false;

    TestSession        session;
    StaircaseAlgorithm staircase;

    int currentAxis = 0;
    int anchorIdx = 0;

    TrialColors   trial;
    bool          awaitingClick = false;
    FeedbackState feedback = FeedbackState::NONE;
    int           clickedIdx = -1;
    double        feedbackTimer = 0.0;

    static constexpr double FEEDBACK_DURATION = 0.55;

    MLPResult mlpResult = { 0, 0, 0.0f };

    /**
      * Avvia l'addestramento del modello MLP in un thread separato.
      *
      * La funzione tenta inizialmente di caricare un modello già salvato
      * dal file "model.bin". Se il file non esiste, genera il dataset di
      * addestramento, esegue il training della rete neurale e salva il
      * modello addestrato su disco.
      *
      * Durante l'esecuzione aggiorna lo stato di avanzamento tramite
      * la struttura TrainingState.
      *
      * @param Nessuno.
      *
      * @return void
     */
    void startTraining() {
        screen = AppScreen::TRAINING;
        training.running = true;
        training.done = false;
        training.success = false;
        training.progress = 0;
        training.epoch = 0;
        training.loss = 0.0;

        std::thread([this]() {
            training.setMessage("Cerco model.bin...");

            if (mlp.load("model.bin")) {
                mlpReady = true;
                training.progress = 100;
                training.success = true;
                training.done = true;
                training.running = false;
                training.setMessage("Modello caricato!");
                return;
            }

            training.setMessage("Generazione dataset...");
            training.progress = 10;

            DatasetGenerator gen;
            int total = 0;
            auto dataset = gen.generate(&total);

            training.progress = 20;
            training.setMessage("Training MLP classificatore...");

            mlp.train(dataset, [this](int epoch, double loss) {
                training.epoch = epoch;
                training.loss = loss;
                training.progress = 20 + (int)((epoch / (double)MLP::EPOCHS) * 75.0);
                });

            training.progress = 97;
            training.setMessage("Salvataggio modello...");
            mlp.save("model.bin");

            mlpReady = true;
            training.progress = 100;
            training.success = true;
            training.done = true;
            training.running = false;
            training.setMessage("Pronto!");
            }).detach();
    }

    /**
     * Inizializza una nuova sessione di test.
     *
     * Resetta i risultati precedenti, imposta il primo asse cromatico
     * da analizzare e avvia la sequenza dei test percettivi.
     *
     * @param Nessuno.
     *
     * @return void
    */
    void startTest() {
        session.reset();
        currentAxis = 0;
        screen = AppScreen::TESTING;
        startAxis(0);
    }

    /**
     * Avvia il test relativo ad uno specifico asse cromatico.
     *
     * Reinizializza lo staircase adattivo e prepara il primo trial
     * dell'asse selezionato.
     *
     * @param axis Indice dell'asse da testare:
     *             0 = Protan,
     *             1 = Deutan,
     *             2 = Tritan.
     *
     * @return void
     */
    void startAxis(int axis) {
        currentAxis = axis;
        staircase = StaircaseAlgorithm();
        anchorIdx = 0;
        setupNextTrial();
    }

    /**
     * Genera un nuovo trial percettivo.
     *
     * Seleziona il colore di riferimento (anchor), calcola il colore
     * alterato mediante lo spostamento lungo la confusion line e
     * distribuisce casualmente il colore differente tra i tre stimoli.
     *
     * Inoltre aggiorna lo stato interno dell'interfaccia utente per
     * attendere la risposta dell'utente.
     *
     * @param Nessuno.
     *
     * @return void
     */
    void setupNextTrial() {
        anchorIdx = (staircase.trialCount() / 4) % 2;
        auto anc = ColorMath::getAnchor(currentAxis, anchorIdx);
        auto base = ColorMath::xyToImGui(anc.x, anc.y);

        double delta = staircase.currentDelta();
        auto shifted = ColorMath::getShiftedColor(anc.x, anc.y, currentAxis, delta);
        auto odd = ColorMath::xyToImGui(shifted.x, shifted.y);

        trial.oddIdx = staircase.trialCount() % 3;
        if ((staircase.trialCount() / 3) % 2 == 1)
            trial.oddIdx = (trial.oddIdx + 1) % 3;

        for (int i = 0; i < 3; ++i)
            trial.c[i] = (i == trial.oddIdx) ? odd : base;

        awaitingClick = true;
        feedback = FeedbackState::NONE;
        clickedIdx = -1;
    }

    /**
     * Registra la risposta fornita dall'utente.
     *
     * Verifica se la scelta effettuata corrisponde allo stimolo
     * differente e aggiorna lo staircase adattivo con il risultato.
     *
     * @param idx Indice del pulsante selezionato dall'utente
     *            (0, 1 oppure 2).
     *
     * @return void
     */
    void registerClick(int idx) {
        if (!awaitingClick || feedback != FeedbackState::NONE) return;

        clickedIdx = idx;
        bool correct = (idx == trial.oddIdx);
        feedback = correct ? FeedbackState::CORRECT : FeedbackState::WRONG;
        feedbackTimer = FEEDBACK_DURATION;
        awaitingClick = false;

        staircase.registerResponse(correct);
    }

    /**
     * Aggiorna il timer del feedback visivo.
     *
     * Quando il tempo di visualizzazione del feedback termina,
     * prepara automaticamente il trial successivo oppure conclude
     * l'asse corrente se lo staircase è terminato.
     *
     * @param deltaTime Tempo trascorso dall'ultimo aggiornamento
     *                  espresso in secondi.
     *
     * @return true se il feedback è terminato.
     * @return false se il feedback è ancora attivo.
     */
    bool updateFeedback(double deltaTime) {
        if (feedback == FeedbackState::NONE) return false;
        feedbackTimer -= deltaTime;
        if (feedbackTimer > 0.0) return false;

        feedback = FeedbackState::NONE;
        clickedIdx = -1;

        if (staircase.state() == StaircaseAlgorithm::State::FINISHED)
            onAxisFinished();
        else
            setupNextTrial();

        return true;
    }

    /**
     * Riporta l'applicazione alla schermata iniziale.
     *
     * @param Nessuno.
     *
     * @return void
     */
    void goToWelcome() { screen = AppScreen::WELCOME; }

    /**
     * Restituisce la percentuale di avanzamento del test corrente.
     *
     * @param Nessuno.
     *
     * @return Percentuale di completamento dello staircase.
     */
    int  axisProgress()  const { return staircase.progressPercent(); }
    
    /**
     * Restituisce il numero di assi cromatici completati.
     *
     * @param Nessuno.
     *
     * @return Numero di assi completati.
     */
    int  completedAxes() const { return session.completedAxes; }

    /**
     * Verifica se il sistema è in attesa della risposta dell'utente.
     *
     * @param Nessuno.
     *
     * @return true se è possibile selezionare uno stimolo.
     * @return false altrimenti.
     */
    bool isWaitingForClick() const {
        return screen == AppScreen::TESTING &&
            awaitingClick &&
            feedback == FeedbackState::NONE;
    }

private:

    /**
     * Gestisce la conclusione del test relativo ad un asse cromatico.
     *
     * Salva la soglia percettiva stimata dallo staircase e, se sono
     * presenti altri assi da valutare, avvia automaticamente il test
     * successivo.
     *
     * Al termine dei tre assi esegue la classificazione tramite il
     * modello MLP ottenendo:
     * - tipo di deficit cromatico;
     * - percentuale stimata di severità.
     *
     * @param Nessuno.
     *
     * @return void
     */
    void onAxisFinished() {
        double thr = staircase.threshold();
        session.registerAxisResult(currentAxis, thr);
        currentAxis++;

        if (currentAxis < 3) {
            startAxis(currentAxis);
            return;
        }

        session.computeEstimates();

        double thresholds[3] = {
            session.thresholds[0],
            session.thresholds[1],
            session.thresholds[2]
        };

        mlpResult = mlp.predict(thresholds);
        screen = AppScreen::RESULTS;
    }
};