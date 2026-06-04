#version 330 core

in  vec4 ourColor;
out vec4 FragColor;

uniform int   cvdType;
uniform float cvdSeverity;


const mat3 RGB2LMS = mat3(
    vec3(0.31399022, 0.15537241, 0.01775239),
    vec3(0.63951294, 0.75789446, 0.10944209),
    vec3(0.04649755, 0.08670142, 0.87256922)
);

mat3 LMS2RGB = inverse(RGB2LMS);

// Costanti di Machado
const float A_L      = 0.6887;
const float A_M      = 0.8013;
const float k_proto  = 0.96 * (A_L / A_M);   
const float k_deutan = 0.96 * (A_M / A_L);   

/*
 * buildRGB2LMS_PROTO
 * Modifica la matrice di conversione RGB->LMS originale per simulare la Protanopia
 * (assenza/deficit dei coni L), applicando le costanti di Machado sulla prima riga.
 *
 * Parametri di input:
 * - nessuno.
 *
 * Valori restituiti:
 * - mat3: Matrice 3x3 di trasformazione modificata per la protanopia.
 */
mat3 buildRGB2LMS_PROTO() {
    mat3 m = RGB2LMS;
    m[0][0] = RGB2LMS[0][1] * k_proto;
    m[1][0] = RGB2LMS[1][1] * k_proto;
    m[2][0] = RGB2LMS[2][1] * k_proto;
    return m;
}

/*
 * buildRGB2LMS_DEUTAN
 * Modifica la matrice di conversione RGB->LMS originale per simulare la Deuteranopia
 * (assenza/deficit dei coni M), applicando le costanti di Machado sulla seconda riga.
 *
 * Parametri di input:
 * - nessuno.
 *
 * Valori restituiti:
 * - mat3: Matrice 3x3 di trasformazione modificata per la deuteranopia.
 */
mat3 buildRGB2LMS_DEUTAN() {
    mat3 m = RGB2LMS;
    m[0][1] = RGB2LMS[0][0] * k_deutan;
    m[1][1] = RGB2LMS[1][0] * k_deutan;
    m[2][1] = RGB2LMS[2][0] * k_deutan;
    return m;
}

/*
 * buildRGB2LMS_TRIT
 * Modifica la matrice di conversione RGB->LMS originale per simulare la Tritanopia
 * (assenza/deficit dei coni S), azzerando la terza riga (canale del blu).
 *
 * Parametri di input:
 * - nessuno.
 *
 * Valori restituiti:
 * - mat3: Matrice 3x3 di trasformazione modificata per la tritanopia.
 */
mat3 buildRGB2LMS_TRIT() {
    mat3 m = RGB2LMS;
    m[0][2] = 0.0;
    m[1][2] = 0.0;
    m[2][2] = 0.0;
    return m;
}

mat3 RGB2LMS_PROTO  = buildRGB2LMS_PROTO();
mat3 RGB2LMS_DEUTAN = buildRGB2LMS_DEUTAN();
mat3 RGB2LMS_TRIT   = buildRGB2LMS_TRIT();

/*
 * toLinear
 * Converte i canali colore dello spazio sRGB non lineare (gamma-corrected)
 * nello spazio colore lineare, usando un'approssimazione con esponente 2.2.
 * Evita valori negativi o nulli tramite una funzione max di sicurezza.
 *
 * Parametri di input:
 * - c: vec3 che rappresenta il colore di input RGB in spazio Gamma.
 *
 * Valori restituiti:
 * - vec3: Il colore convertito in spazio lineare.
 */
vec3 toLinear(vec3 c) { return pow(max(c, vec3(0.0001)), vec3(2.2));       }

/*
 * toGamma
 * Converte i canali colore dallo spazio lineare allo spazio sRGB non lineare
 * (correzione gamma), applicando l'esponente inverso (1.0 / 2.2).
 * Evita valori negativi o nulli tramite una funzione max di sicurezza.
 *
 * Parametri di input:
 * - c: vec3 che rappresenta il colore di input RGB in spazio Lineare.
 *
 * Valori restituiti:
 * - vec3: Il colore convertito in spazio Gamma pronto per l'output video.
 */
vec3 toGamma (vec3 c) { return pow(max(c, vec3(0.0001)), vec3(1.0 / 2.2)); }

/*
 * simProtan
 * Simula la cecita' totale al colore rosso (Protanopia) trasformando il colore 
 * in spazio LMS compromesso e riconvertendolo poi in RGB lineare.
 *
 * Parametri di input:
 * - lin: vec3 che rappresenta il colore originale in spazio lineare.
 *
 * Valori restituiti:
 * - vec3: Colore lineare simulato per un soggetto protanope.
 */
vec3 simProtan (vec3 lin) { return LMS2RGB * (RGB2LMS_PROTO  * lin); }
/*
 * simDeutan
 * Simula la cecita' totale al colore verde (Deuteranopia) trasformando il colore 
 * in spazio LMS compromesso e riconvertendolo poi in RGB lineare.
 *
 * Parametri di input:
 * - lin: vec3 che rappresenta il colore originale in spazio lineare.
 *
 * Valori restituiti:
 * - vec3: Colore lineare simulato per un soggetto deuteranope.
 */
vec3 simDeutan (vec3 lin) { return LMS2RGB * (RGB2LMS_DEUTAN * lin); }
/*
 * simTrit
 * Simula la cecita' totale al colore blu (Tritanopia) trasformando il colore 
 * in spazio LMS compromesso e riconvertendolo poi in RGB lineare.
 *
 * Parametri di input:
 * - lin: vec3 che rappresenta il colore originale in spazio lineare.
 *
 * Valori restituiti:
 * - vec3: Colore lineare simulato per un soggetto tritanope.
 */
vec3 simTrit   (vec3 lin) { return LMS2RGB * (RGB2LMS_TRIT   * lin); }

/*
 * simAnomala
 * Simula l'anomalia tricromatica specifica miscelando (tramite interpolazione mix)
 * il colore originale con la rispettiva simulazione dicromatica pura,
 * usando come peso il grado di severita' (cvdSeverity) calcolato dal test.
 *
 * Parametri di input:
 * - lin: vec3 che rappresenta il colore originale in spazio lineare.
 *
 * Valori restituiti:
 * - vec3: Colore lineare parzialmente alterato in base al tipo e alla severita' del deficit.
 */
vec3 simAnomala(vec3 lin) {
    if (cvdType == 0) return mix(lin, simProtan(lin), cvdSeverity);
    if (cvdType == 1) return mix(lin, simDeutan(lin), cvdSeverity);
                      return mix(lin, simTrit  (lin), cvdSeverity);
}


const mat3 E_proto = mat3(
    vec3(0.0, 0.7, 0.3),   
    vec3(0.0, 0.0, 0.1),   
    vec3(0.0, 0.0, 0.0)    
);
const mat3 E_deutan = mat3(
    vec3(0.0, 0.0, 0.1),   
    vec3(0.7, 0.0, 0.3),   
    vec3(0.0, 0.0, 0.0)    
);
const mat3 E_trit = mat3(
    vec3(0.0, 0.1, 0.0),   
    vec3(0.0, 0.0, 0.0),   
    vec3(0.7, 0.3, 0.0)    
);

/*
 * correggi
 * Applica l'algoritmo di daltonizzazione di Fidaner. Calcola la perdita di 
 * informazione cromatica (differenza tra colore vero e colore simulato) e 
 * ridistribuisce tale "errore" sui canali sani per ripristinare il contrasto perduto.
 *
 * Parametri di input:
 * - lin: vec3 che rappresenta il colore originale sRGB in spazio lineare.
 *
 * Valori restituiti:
 * - vec3: Il colore daltonizzato (corretto) in spazio lineare.
 */
vec3 correggi(vec3 lin) {
    vec3 sim = simAnomala(lin);
    vec3 err = lin - sim;

    if (cvdType == 0) return lin + E_proto  * err;
    if (cvdType == 1) return lin + E_deutan * err;
                      return lin + E_trit   * err;
}

/*
 * main
 * Entry point del Fragment Shader. Gestisce il flusso principale: effettua un controllo 
 * preventivo sulla severita'; se significativa, linearizza il colore in ingresso (ourColor), 
 * ne calcola la correzione cromatica adattiva, ne limita il range nei confini [0.0, 1.0] 
 * e infine riapplica la correzione Gamma prima di scrivere l'output in FragColor.
 *
 * Parametri di input:
 * - nessuno (utilizza implicitamente le variabili globali "in/uniform" dello shader).
 *
 * Valori restituiti:
 * - nessuno (void, scrive il risultato direttamente nella variabile globale di output "FragColor").
 */
void main()
{
    if (cvdSeverity < 0.01) {
        FragColor = ourColor;
        return;
    }

    vec3 lin       = toLinear(ourColor.rgb);
    vec3 corrected = correggi(lin);

    FragColor = vec4(toGamma(clamp(corrected, 0.0, 1.0)), ourColor.a);
}
