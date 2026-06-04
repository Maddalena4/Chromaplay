#pragma once
#include "imgui/imgui.h"
#include "AppState.h"
#include "ImageLoader.h"
#include "launcher.h"
#include <cmath>
#include <cstring>


namespace UI {

    /*
     * textCentered
     * Disegna un testo centrato orizzontalmente all'interno della regione di contenuto corrente di ImGui.
     *
     * Parametri di input:
     * - text: Puntatore alla stringa di caratteri (C-string) da mostrare a schermo.
     * - col: Oggetto ImVec4 che definisce il colore del testo in formato RGBA (valore predefinito: grigio chiaro/azzurrato).
     *
     */
    static void textCentered(const char* text,
        ImVec4 col = { 0.85f, 0.85f, 0.92f, 1.0f }) {
        float w = ImGui::GetContentRegionAvail().x;
        float tw = ImGui::CalcTextSize(text).x;
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (w - tw) * 0.5f);
        ImGui::TextColored(col, "%s", text);
    }

    /*
     * progressBar
     * Disegna una barra di progresso personalizzata con angoli arrotondati e uno sfondo scuro.
     *
     * Parametri di input:
     * - fraction: Valore float compreso tra 0.0f e 1.0f che indica la percentuale di riempimento della barra.
     * - fillColor: Oggetto ImVec4 che definisce il colore della parte riempita della barra.
     * - height: Valore float che indica l'altezza in pixel della barra (valore predefinito: 8.0f).
     *
     */
    static void progressBar(float fraction, ImVec4 fillColor, float height = 8.0f) {
        ImVec2 pos = ImGui::GetCursorScreenPos();
        float  width = ImGui::GetContentRegionAvail().x;
        ImDrawList* dl = ImGui::GetWindowDrawList();
        dl->AddRectFilled(pos, { pos.x + width, pos.y + height },
            IM_COL32(30, 30, 55, 255), height * 0.5f);
        if (fraction > 0.0f) {
            float fw = width * std::min(fraction, 1.0f);
            dl->AddRectFilled(pos, { pos.x + fw, pos.y + height },
                ImGui::ColorConvertFloat4ToU32(fillColor), height * 0.5f);
        }
        ImGui::Dummy({ width, height + 4.0f });
    }

    /*
     * colorCircle
     * Disegna un cerchio colorato provvisto di bordo e vi sovrappone un pulsante invisibile per renderlo cliccabile.
     *
     * Parametri di input:
     * - id: Stringa usata come identificatore univoco interno per il widget pulsante di ImGui.
     * - center: Oggetto ImVec2 contenente le coordinate (X, Y) dello schermo in cui centrare il cerchio.
     * - radius: Valore float che stabilisce il raggio del cerchio in pixel.
     * - color: Oggetto ImVec4 che definisce il colore di riempimento interno del cerchio.
     * - border: Oggetto ImVec4 che definisce il colore del bordo esterno del cerchio.
     * - bThk: Valore float che indica lo spessore del bordo in pixel (valore predefinito: 2.0f).
     *
     * Valori restituiti:
     * - bool: Restituisce true se l'utente ha fatto click sul cerchio in questo frame, false altrimenti.
     */
    static bool colorCircle(const char* id, ImVec2 center, float radius,
        ImVec4 color, ImVec4 border, float bThk = 2.0f) {
        ImDrawList* dl = ImGui::GetWindowDrawList();
        dl->AddCircleFilled(center, radius,
            ImGui::ColorConvertFloat4ToU32(color), 64);
        dl->AddCircle(center, radius,
            ImGui::ColorConvertFloat4ToU32(border), 64, bThk);
        ImGui::SetCursorScreenPos({ center.x - radius, center.y - radius });
        ImGui::InvisibleButton(id, { radius * 2, radius * 2 });
        return ImGui::IsItemClicked();
    }

    /*
     * drawWelcome
     * Rendering a schermo intero della schermata iniziale dell'applicazione. Presenta le istruzioni del test
     * e mostra un pulsante per avviare il test o l'addestramento in base allo stato del modello MLP.
     *
     * Parametri di input:
     * - state: Riferimento all'oggetto AppState che contiene e gestisce lo stato globale del programma.
     *.
     */
    inline void drawWelcome(AppState& state) {
        ImGuiIO& io = ImGui::GetIO();
        ImGui::SetNextWindowPos({ 0,0 });
        ImGui::SetNextWindowSize(io.DisplaySize);
        ImGui::Begin("##welcome", nullptr,
            ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove);

        float winW = io.DisplaySize.x;
        float winH = io.DisplaySize.y;

        ImGui::SetCursorPosY(winH * 0.10f);
        textCentered("Test di Percezione Cromatica", { 0.90f,0.90f,0.96f,1.0f });
        ImGui::Spacing();
        textCentered("Rilevamento adattivo del deficit di visione cromatica (CVD)",
            { 0.35f,0.35f,0.55f,1.0f });

        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 20);
        float sepX = winW * 0.2f;
        ImGui::GetWindowDrawList()->AddLine(
            { sepX, ImGui::GetCursorScreenPos().y },
            { winW - sepX, ImGui::GetCursorScreenPos().y },
            IM_COL32(40, 40, 80, 200), 1.0f);
        ImGui::Dummy({ 0,18 });

        struct Step { const char* num; const char* text; ImVec4 col; };
        Step steps[] = {
            {"1","Vedrai 3 cerchi colorati. Due sono identici, uno e' leggermente diverso.",
                 {1.0f,0.38f,0.38f,1.0f}},
            {"2","Clicca il cerchio che ti sembra diverso dagli altri due.",
                 {0.30f,0.85f,0.45f,1.0f}},
            {"3","Il test si adatta: circa 25 trial per asse, 3 assi in totale.",
                 {0.38f,0.62f,1.0f,1.0f}},
            {"4","Se non vedi differenze clicca uno a caso: e' normale.",
                 {0.90f,0.72f,0.30f,1.0f}},
        };

        float colW = winW * 0.55f;
        float startX = (winW - colW) * 0.5f;
        for (auto& s : steps) {
            ImGui::SetCursorPosX(startX);
            ImGui::TextColored(s.col, "%s", s.num);
            ImGui::SameLine(startX + 28);
            ImGui::PushTextWrapPos(startX + colW);
            ImGui::TextColored({ 0.65f,0.65f,0.80f,1.0f }, "%s", s.text);
            ImGui::PopTextWrapPos();
            ImGui::Spacing();
        }

        // Stato MLP
        ImGui::SetCursorPosY(winH * 0.70f);
        if (state.mlpReady) {
            textCentered("Modello MLP pronto", { 0.28f,0.85f,0.55f,1.0f });
        }
        else {
            textCentered("Il modello verra' addestrato prima del test",
                { 0.45f,0.45f,0.70f,1.0f });
        }

        ImGui::SetCursorPosY(winH * 0.76f);
        float btnW = 210, btnH = 46;
        ImGui::SetCursorPosX((winW - btnW) * 0.5f);
        ImGui::PushStyleColor(ImGuiCol_Button, { 0.18f,0.36f,0.82f,1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0.25f,0.45f,0.95f,1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, { 0.12f,0.25f,0.60f,1.0f });
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 23.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 0,0 });

        const char* btnLabel = state.mlpReady ? "  Inizia il Test  ->" : "  Avvia Training  ->";
        if (ImGui::Button(btnLabel, { btnW, btnH })) {
            if (state.mlpReady)
                state.startTest();
            else
                state.startTraining();
        }

        ImGui::PopStyleVar(2);
        ImGui::PopStyleColor(3);

        ImGui::SetCursorPosY(winH - 36);
        textCentered("Esegui il test in un ambiente con illuminazione costante",
            { 0.22f,0.22f,0.40f,1.0f });

        ImGui::End();
    }

    /*
     * drawTraining
     * Rendering a schermo intero della schermata di addestramento. Visualizza l'avanzamento
     * complessivo dell'MLP (barra di progresso), le informazioni in tempo reale sulle Epoche e sulla Loss
     * e mostra il pulsante di avvio test solo a completamento avvenuto con successo.
     *
     * Parametri di input:
     * - state: Riferimento all'oggetto AppState da cui attingere i dati atomici e i messaggi del thread di training.
     *
     */
    inline void drawTraining(AppState& state) {
        ImGuiIO& io = ImGui::GetIO();
        ImGui::SetNextWindowPos({ 0,0 });
        ImGui::SetNextWindowSize(io.DisplaySize);
        ImGui::Begin("##training", nullptr,
            ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove);

        float winW = io.DisplaySize.x;
        float winH = io.DisplaySize.y;

        ImGui::SetCursorPosY(winH * 0.22f);
        textCentered("Preparazione del Modello", { 0.90f,0.90f,0.96f,1.0f });
        ImGui::Spacing();
        textCentered("Generazione dataset e addestramento MLP in corso...",
            { 0.35f,0.35f,0.55f,1.0f });

        ImGui::SetCursorPosY(winH * 0.42f);
        float barW = winW * 0.60f;
        ImGui::SetCursorPosX((winW - barW) * 0.5f);
        ImGui::PushItemWidth(barW);

        int   prog = state.training.progress.load();
        float frac = prog / 100.0f;

        ImVec4 barColor = state.training.done
            ? ImVec4{ 0.28f, 0.85f, 0.55f, 1.0f }
        : ImVec4{ 0.90f, 0.62f, 0.20f, 1.0f };

        progressBar(frac, barColor, 14.0f);
        ImGui::PopItemWidth();

        char pctStr[16];
        snprintf(pctStr, sizeof(pctStr), "%d%%", prog);
        ImGui::SetCursorPosX((winW - barW) * 0.5f);
        ImGui::TextColored({ 0.55f,0.55f,0.80f,1.0f }, "%s", pctStr);

        ImGui::SetCursorPosY(winH * 0.56f);
        std::string msg = state.training.getMessage();
        textCentered(msg.c_str(), { 0.60f,0.60f,0.85f,1.0f });

        if (state.training.running && !state.training.done) {
            int    ep = state.training.epoch.load();
            double loss = state.training.loss.load();
            char info[64];
            snprintf(info, sizeof(info), "Epoch %d / %d     Loss: %.4f",
                ep, MLP::EPOCHS, loss);
            ImGui::SetCursorPosY(winH * 0.62f);
            textCentered(info, { 0.35f,0.35f,0.55f,1.0f });
        }

        ImGui::SetCursorPosY(winH * 0.72f);
        textCentered("Architettura MLP:  3  ->  32  ->  16  ->  [3 + 1]",
            { 0.25f,0.25f,0.45f,1.0f });
        ImGui::Spacing();
        textCentered("Input: [soglia_P, soglia_D, soglia_T]   "
            "Output: tipo CVD + percentuale",
            { 0.22f,0.22f,0.40f,1.0f });

        if (state.training.done && state.training.success) {
            ImGui::SetCursorPosY(winH * 0.84f);
            float btnW = 200, btnH = 44;
            ImGui::SetCursorPosX((winW - btnW) * 0.5f);
            ImGui::PushStyleColor(ImGuiCol_Button, { 0.18f,0.36f,0.82f,1.0f });
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0.25f,0.45f,0.95f,1.0f });
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, { 0.12f,0.25f,0.60f,1.0f });
            ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 22.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 0,0 });

            if (ImGui::Button("  Inizia il Test  ->", { btnW, btnH }))
                state.startTest();

            ImGui::PopStyleVar(2);
            ImGui::PopStyleColor(3);
        }

        ImGui::End();
    }

    /*
     * drawTesting
     * Rendering e gestione della schermata principale del test visivo adaptivo. Mostra i dati dell'asse
     * di sintonizzazione cromatica corrente, gestisce l'interazione/clic dell'utente con i 3 cerchi dinamici
     * e mostra i bordi colorati di feedback (es. Verde = Corretto, Rosso = Errato) basandosi sullo stato attuale del test.
     *
     * Parametri di input:
     * - state: Riferimento all'oggetto AppState contenente le info sul trial e l'asse corrente.
     * - deltaTime: Valore double che rappresenta il tempo trascorso dall'ultimo frame (usato per animare/aggiornare i timer di feedback).
     *
     */
    inline void drawTesting(AppState& state, double deltaTime) {
        ImGuiIO& io = ImGui::GetIO();
        ImGui::SetNextWindowPos({ 0,0 });
        ImGui::SetNextWindowSize(io.DisplaySize);
        ImGui::Begin("##testing", nullptr,
            ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove);

        float winW = io.DisplaySize.x;
        float winH = io.DisplaySize.y;

        state.updateFeedback(deltaTime);
        if (state.screen != AppScreen::TESTING) { ImGui::End(); return; }

        {
            auto ac = ColorMath::typeColor(state.currentAxis);
            ImGui::SetCursorPos({ 16,14 });
            ImGui::TextColored({ ac.r,ac.g,ac.b,1.0f }, "?");
            ImGui::SameLine();
            ImGui::TextColored({ ac.r,ac.g,ac.b,0.85f }, "%s",
                ColorMath::axisLabel(state.currentAxis));
            char info[32];
            snprintf(info, sizeof(info), "Asse %d / 3", state.currentAxis + 1);
            float tw = ImGui::CalcTextSize(info).x;
            ImGui::SetCursorPos({ winW - tw - 16, 14 });
            ImGui::TextColored({ 0.35f,0.35f,0.55f,1.0f }, "%s", info);
        }

        {
            char lbl[32];
            snprintf(lbl, sizeof(lbl), "Progresso: %d%%", state.axisProgress());
            float tw = ImGui::CalcTextSize(lbl).x;
            ImGui::SetCursorPos({ winW - tw - 16, 38 });
            ImGui::TextColored({ 0.25f,0.25f,0.45f,1.0f }, "%s", lbl);
            ImGui::SetCursorPos({ 16, 52 });
            auto ac = ColorMath::typeColor(state.currentAxis);
            progressBar(state.axisProgress() / 100.0f, { ac.r,ac.g,ac.b,0.85f }, 6.0f);
        }

        ImGui::SetCursorPosY(winH * 0.28f);
        textCentered("Clicca il cerchio che ti sembra diverso dagli altri due",
            { 0.75f,0.75f,0.88f,0.90f });

        {
            float radius = std::min(std::max(std::min(winW / 9.0f, winH / 5.5f), 50.0f), 95.0f);
            float spacing = radius * 3.0f;
            float totalW = spacing * 2 + radius * 2;
            float startX = (winW - totalW) * 0.5f + radius;
            float cy = winH * 0.54f;
            const char* ids[] = { "##c0","##c1","##c2" };
            const char* labels[] = { "A","B","C" };

            for (int i = 0; i < 3; ++i) {
                float  cx = startX + i * spacing;
                auto& c = state.trial.c[i];
                ImVec4 circleCol = { c.r, c.g, c.b, 1.0f };
                ImVec4 borderCol = { 1.0f,1.0f,1.0f,0.12f };
                float  borderThk = 1.5f;

                if (state.feedback != FeedbackState::NONE) {
                    if (state.clickedIdx == i) {
                        borderCol = state.feedback == FeedbackState::CORRECT
                            ? ImVec4{ 0.20f,0.90f,0.45f,1.0f }
                        : ImVec4{ 0.90f,0.22f,0.30f,1.0f };
                        borderThk = 4.0f;
                    }
                    if (state.feedback == FeedbackState::WRONG && i == state.trial.oddIdx) {
                        borderCol = { 0.20f,0.90f,0.45f,0.60f };
                        borderThk = 3.0f;
                    }
                }

                ImVec2 center = {
                    ImGui::GetWindowPos().x + cx,
                    ImGui::GetWindowPos().y + cy
                };

                if (ImGui::IsMouseHoveringRect(
                    { center.x - radius, center.y - radius },
                    { center.x + radius, center.y + radius })
                    && state.isWaitingForClick()) {
                    ImGui::GetWindowDrawList()->AddCircleFilled(
                        center, radius + 10, IM_COL32(255, 255, 255, 18), 64);
                }

                if (colorCircle(ids[i], center, radius, circleCol, borderCol, borderThk))
                    if (state.isWaitingForClick())
                        state.registerClick(i);

                float lblX = cx - ImGui::CalcTextSize(labels[i]).x * 0.5f;
                ImGui::SetCursorPos({ lblX, cy + radius + 12 });
                ImGui::TextColored({ 1.0f,1.0f,1.0f,0.22f }, "%s", labels[i]);
            }
        }

        char trialStr[32];
        snprintf(trialStr, sizeof(trialStr), "Trial %d",
            state.staircase.trialCount() + 1);
        ImGui::SetCursorPos({ 16, winH - 28 });
        ImGui::TextColored({ 0.22f,0.22f,0.40f,1.0f }, "%s", trialStr);

        ImGui::End();
    }

    /*
     * drawResults
     * Rendering a schermo intero dell'interfaccia dei risultati. È divisa in due colonne principali:
     * a sinistra mostra i dettagli grezzi dei tre assi e la classificazione finale dell'MLP (con confidenza);
     * a destra mostra una texture di simulazione del deficit calcolato.
     * In fondo fornisce pulsanti per ripetere il test o per lanciare il gioco calibrato.
     *
     * Parametri di input:
     * - state: Riferimento all'oggetto AppState contenente i risultati calcolati della sessione e dell'MLP.
     * - loader: Riferimento al manager ImageLoader responsabile del recupero della texture di simulazione grafica.
     *
     */
    inline void drawResults(AppState& state, ImageLoader& loader) {
        ImGuiIO& io = ImGui::GetIO();
        ImGui::SetNextWindowPos({ 0,0 });
        ImGui::SetNextWindowSize(io.DisplaySize);
        ImGui::Begin("##results", nullptr,
            ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove);

        float winW = io.DisplaySize.x;
        float winH = io.DisplaySize.y;

        ImGui::SetCursorPosY(winH * 0.05f);
        textCentered("Risultati del Test", { 0.90f,0.90f,0.96f,1.0f });
        ImGui::Spacing();
        textCentered("Classificazione tramite rete neurale MLP",
            { 0.28f,0.28f,0.48f,1.0f });

        ImGui::SetCursorPosY(winH * 0.16f);
        float colW = winW * 0.46f;
        float margin = winW * 0.04f;

        ImGui::SetCursorPos({ margin, ImGui::GetCursorPosY() });
        ImGui::BeginGroup();

        ImVec4 axisColors[3] = {
            {1.00f,0.35f,0.35f,1.0f},
            {0.28f,0.85f,0.40f,1.0f},
            {0.35f,0.60f,1.00f,1.0f},
        };
        const char* axisNames[3] = {
            "Protanomalia  (L-cono / Rosso)",
            "Deuteranomalia  (M-cono / Verde)",
            "Tritanomalia  (S-cono / Blu-Giallo)",
        };

        for (int i = 0; i < 3; ++i) {
            int    pct = state.session.percents[i];
            double thr = state.session.thresholds[i];

            ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4{ 0.07f,0.07f,0.16f,1.0f });
            ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 10.0f);
            char cardId[16]; snprintf(cardId, sizeof(cardId), "##card%d", i);
            ImGui::BeginChild(cardId, { colW, 82 }, true);

            ImGui::SetCursorPos({ 14,10 });
            ImGui::TextColored(axisColors[i], "%s", axisNames[i]);
            ImGui::SetCursorPos({ 14,32 });
            ImGui::PushItemWidth(colW - 28);
            progressBar(pct / 100.0f, axisColors[i], 7.0f);
            ImGui::PopItemWidth();
            ImGui::SetCursorPos({ 14,52 });
            ImGui::TextColored({ 0.32f,0.32f,0.55f,1.0f },
                "Soglia: %.4f  |  Stima grezza: %d%%  |  %s",
                thr, pct, TestSession::severityLabel(pct));

            ImGui::EndChild();
            ImGui::PopStyleVar();
            ImGui::PopStyleColor();
            ImGui::Spacing();
        }

        {
            ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4{ 0.05f,0.08f,0.18f,1.0f });
            ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 10.0f);
            ImGui::BeginChild("##mlpCard", { colW, 90 }, true);

            ImGui::SetCursorPos({ 14, 10 });
            ImGui::TextColored({ 0.60f,0.60f,0.90f,1.0f }, "Risultato MLP:");

            auto& r = state.mlpResult;
            auto  dc = ColorMath::typeColor(r.cvdType);

            if (r.percent < 5) {
                ImGui::SetCursorPos({ 14, 32 });
                ImGui::TextColored({ 0.28f,0.85f,0.55f,1.0f },
                    "Visione cromatica nella norma");
            }
            else {
                ImGui::SetCursorPos({ 14, 32 });
                ImGui::TextColored({ dc.r,dc.g,dc.b,1.0f },
                    "%s   —   %d%%   (%s)",
                    ColorMath::typeName(r.cvdType),
                    r.percent,
                    TestSession::severityLabel(r.percent));

                ImGui::SetCursorPos({ 14, 56 });
                char confLabel[32];
                snprintf(confLabel, sizeof(confLabel),
                    "Confidenza classificazione: %.0f%%",
                    r.confidence * 100.0f);
                ImGui::TextColored({ 0.35f,0.35f,0.60f,1.0f }, "%s", confLabel);
            }

            ImGui::EndChild();
            ImGui::PopStyleVar();
            ImGui::PopStyleColor();
        }

        ImGui::EndGroup();

        float imgColX = margin + colW + margin;
        float imgColW = winW - imgColX - margin;
        ImGui::SetCursorPos({ imgColX, winH * 0.16f });
        ImGui::BeginGroup();

        ImGui::TextColored({ 0.30f,0.30f,0.52f,1.0f }, "Simulazione corrispondente:");
        ImGui::Spacing();

        int domType = std::max(0, state.mlpResult.cvdType);
        int domPct = std::max(1, state.mlpResult.percent);

        GLuint tex = loader.getTexture(domType, domPct);
        float  imgH = winH * 0.52f;

        if (tex) {
            ImGui::Image((ImTextureID)(intptr_t)tex, { imgColW, imgH });
        }
        else {
            ImGui::PushStyleColor(ImGuiCol_ChildBg, { 0.06f,0.06f,0.14f,1.0f });
            ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 8.0f);
            ImGui::BeginChild("##noimg", { imgColW, imgH }, true);
            ImGui::SetCursorPosY(imgH * 0.38f);
            textCentered("Immagine non trovata", { 0.35f,0.35f,0.55f,1.0f });
            ImGui::Spacing();
            char pathMsg[256];
            snprintf(pathMsg, sizeof(pathMsg), "%s",
                loader.getPath(domType, domPct).c_str());
            textCentered(pathMsg, { 0.22f,0.22f,0.40f,1.0f });
            ImGui::EndChild();
            ImGui::PopStyleVar();
            ImGui::PopStyleColor();
        }

        ImGui::Spacing();
        char fnote[64];
        snprintf(fnote, sizeof(fnote), "%s_%03d.png",
            ImageLoader::typePrefix(domType).c_str(), domPct);
        ImGui::TextColored({ 0.22f,0.22f,0.38f,1.0f }, "%s", fnote);

        ImGui::EndGroup();

        float btnH = 48;
        float btnW1 = 170;   
        float btnW2 = 230;   
        float gap = 24;
        float totalBW = btnW1 + gap + btnW2;
        float startBX = (winW - totalBW) * 0.5f;
        float btnY = winH - 68;

        ImGui::SetCursorPos({ startBX, btnY });
        ImGui::PushStyleColor(ImGuiCol_Button, { 0.10f,0.10f,0.22f,1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0.14f,0.14f,0.30f,1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, { 0.07f,0.07f,0.16f,1.0f });
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 24.0f);
        if (ImGui::Button("<-  Ripeti il Test", { btnW1, btnH }))
            state.goToWelcome();
        ImGui::PopStyleVar();
        ImGui::PopStyleColor(3);

        ImGui::SetCursorPos({ startBX + btnW1 + gap, btnY });
        ImGui::PushStyleColor(ImGuiCol_Button, { 0.15f,0.55f,0.20f,1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0.20f,0.70f,0.28f,1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, { 0.10f,0.40f,0.14f,1.0f });
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 24.0f);

        if (ImGui::Button("  Inizia a Giocare  ->", { btnW2, btnH })) {
            launchGame(state.mlpResult.cvdType, state.mlpResult.percent);
        }

        ImGui::PopStyleVar();
        ImGui::PopStyleColor(3);

        ImGui::SetCursorPos({ startBX + btnW1 + gap, btnY + btnH + 6 });
        char launchNote[80];
        snprintf(launchNote, sizeof(launchNote),
            "Il gioco si avvia con correzione per %s (%d%%)",
            ColorMath::typeName(std::max(0, state.mlpResult.cvdType)),
            state.mlpResult.percent);
        ImGui::TextColored({ 0.25f,0.55f,0.28f,0.80f }, "%s", launchNote);

        ImGui::End();
    }

}