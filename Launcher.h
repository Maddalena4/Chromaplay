#pragma once

extern bool g_launchPacman;
extern int  g_launchCvdType;
extern int  g_launchCvdPct;

/**
 * Richiede l'avvio del minigioco Pac-Man.
 *
 * La funzione aggiorna le variabili globali utilizzate dal
 * sistema principale per lanciare il gioco con il tipo e
 * la severità del deficit cromatico selezionati.
 *
 * @param cvdType Tipo di deficit cromatico stimato:
 *                0 = Protan,
 *                1 = Deutan,
 *                2 = Tritan.
 *
 * @param cvdPercent Percentuale stimata di severità.
 *
 * @return void
 */
inline void launchGame(int cvdType, int cvdPercent)
{
    g_launchPacman = true;
    g_launchCvdType = cvdType;
    g_launchCvdPct = cvdPercent;
}