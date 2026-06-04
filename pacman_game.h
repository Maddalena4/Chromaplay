#pragma once
/**
 * Avvia il minigioco Pac-Man personalizzato.
 *
 * Il gioco viene eseguito utilizzando la tipologia e il livello
 * di deficit cromatico stimati dal sistema, consentendo
 * all'utente di sperimentare una simulazione coerente con
 * il proprio profilo visivo.
 *
 * @param cvdType Tipo di deficit cromatico:
 *                0 = Protan,
 *                1 = Deutan,
 *                2 = Tritan.
 *
 * @param cvdPercent Percentuale stimata di severità.
 *
 * @return void
 */
void runPacMan(int cvdType, int cvdPercent);