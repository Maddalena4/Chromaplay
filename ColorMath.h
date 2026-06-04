#pragma once
#include <cmath>
#include <algorithm>


namespace ColorMath {

    enum CVDType { PROTAN = 0, DEUTAN = 1, TRITAN = 2 };

    struct xy { double x, y; };
    struct XYZ { double X, Y, Z; };
    struct RGBf { double r, g, b; };
    struct RGB { int r, g, b; };
    struct RGBf32 { float r, g, b; };  

    struct Mat3 {
        double m[9];
        double operator()(int r, int c) const { return m[r * 3 + c]; }
    };


    static const Mat3 PROTAN_MAT[11] = {
        // 0.0 identita
        {{1.000000, 0.000000, 0.000000,  0.000000, 1.000000, 0.000000,  0.000000, 0.000000, 1.000000}},
        // 0.1
        {{0.856167, 0.182038,-0.038205,  0.029342, 0.955115, 0.015544, -0.002880,-0.001563, 1.004443}},
        // 0.2
        {{0.734766, 0.334872,-0.069637,  0.051840, 0.919198, 0.028963, -0.004928,-0.004209, 1.009137}},
        // 0.3
        {{0.630323, 0.465641,-0.095964,  0.069181, 0.890046, 0.040773, -0.006308,-0.007302, 1.013610}},
        // 0.4
        {{0.539009, 0.579343,-0.118352,  0.082546, 0.866121, 0.051332, -0.007136,-0.010248, 1.017384}},
        // 0.5
        {{0.458064, 0.679578,-0.137642,  0.092785, 0.846313, 0.060902, -0.007494,-0.012617, 1.020112}},
        // 0.6
        {{0.385450, 0.769005,-0.154455,  0.100526, 0.829802, 0.069673, -0.007442,-0.015030, 1.022472}},
        // 0.7
        {{0.319627, 0.849633,-0.169261,  0.106241, 0.815969, 0.077790, -0.007025,-0.017286, 1.024311}},
        // 0.8
        {{0.259411, 0.923008,-0.182420,  0.110296, 0.804340, 0.085364, -0.006276,-0.019597, 1.025873}},
        // 0.9
        {{0.203876, 0.990338,-0.194214,  0.112975, 0.794542, 0.092483, -0.005222,-0.021902, 1.027124}},
        // 1.0 protanopia
        {{0.152286, 1.052583,-0.204868,  0.114503, 0.786281, 0.099216, -0.003882,-0.048116, 1.051998}},
    };

    static const Mat3 DEUTAN_MAT[11] = {
        // 0.0
        {{1.000000, 0.000000, 0.000000,  0.000000, 1.000000, 0.000000,  0.000000, 0.000000, 1.000000}},
        // 0.1
        {{0.866435, 0.177704,-0.044139,  0.049567, 0.939063, 0.011370, -0.003453, 0.007233, 0.996220}},
        // 0.2
        {{0.760729, 0.319078,-0.079807,  0.090568, 0.889315, 0.020117, -0.006027, 0.013325, 0.992702}},
        // 0.3
        {{0.664012, 0.448490,-0.112680,  0.122600, 0.849400, 0.028000, -0.007600, 0.018800, 0.988800}},
        // 0.4
        {{0.572000, 0.558160,-0.130160,  0.140210, 0.827560, 0.032230, -0.008650, 0.023190, 0.985460}},
        // 0.5
        {{0.484860, 0.653180,-0.138040,  0.167710, 0.790520, 0.041770, -0.009990, 0.028950, 0.981040}},
        // 0.6
        {{0.399440, 0.745720,-0.145160,  0.188590, 0.757370, 0.054040, -0.010590, 0.033440, 0.977150}},
        // 0.7
        {{0.318270, 0.834400,-0.152670,  0.205670, 0.726940, 0.067390, -0.010650, 0.037500, 0.973150}},
        // 0.8
        {{0.240520, 0.920780,-0.161300,  0.220860, 0.698610, 0.080530, -0.010670, 0.040720, 0.969950}},
        // 0.9
        {{0.166420, 1.003880,-0.170300,  0.234140, 0.672940, 0.092920, -0.010430, 0.043560, 0.966870}},
        // 1.0 deuteranopia
        {{0.085822, 0.871422, 0.042756,  0.085822, 0.871422, 0.042756, -0.007660,-0.028100, 1.035760}},
    };

    static const Mat3 TRITAN_MAT[11] = {
        // 0.0
        {{1.000000, 0.000000, 0.000000,  0.000000, 1.000000, 0.000000,  0.000000, 0.000000, 1.000000}},
        // 0.1
        {{0.926670, 0.092514,-0.019184,  0.021191, 0.964503, 0.014306,  0.008437, 0.054813, 0.936750}},
        // 0.2
        {{0.895720, 0.133330,-0.029050,  0.029800, 0.945400, 0.024800,  0.013600, 0.101300, 0.885100}},
        // 0.3
        {{0.905820, 0.127560,-0.033380,  0.033460, 0.940020, 0.026520,  0.017910, 0.139880, 0.842210}},
        // 0.4
        {{0.910650, 0.126600,-0.037250,  0.039750, 0.933860, 0.026390,  0.022270, 0.175320, 0.802410}},
        // 0.5
        {{0.917820, 0.123700,-0.041520,  0.046130, 0.926650, 0.027220,  0.026710, 0.207080, 0.766210}},
        // 0.6
        {{0.922840, 0.121640,-0.044480,  0.051930, 0.920270, 0.027800,  0.030540, 0.234550, 0.734910}},
        // 0.7
        {{0.928270, 0.119440,-0.047710,  0.057490, 0.913710, 0.028800,  0.034310, 0.259480, 0.706210}},
        // 0.8
        {{0.931760, 0.118460,-0.050220,  0.062350, 0.907540, 0.030110,  0.037650, 0.280850, 0.681500}},
        // 0.9
        {{0.934220, 0.118310,-0.052530,  0.067230, 0.901210, 0.031560,  0.040750, 0.299400, 0.659850}},
        // 1.0 tritanopia
        {{0.950020, 0.050940,-0.000960,  0.017680, 0.966250, 0.016070,  0.012400, 0.375120, 0.612480}},
    };

    struct RGB3d { double r, g, b; };

    /**
     * Applica una matrice di simulazione Machado ad un colore RGB.
     *
     * La funzione simula l'effetto di una specifica alterazione
     * della visione dei colori trasformando il vettore RGB tramite
     * una matrice 3x3.
     *
     * @param M Matrice di trasformazione Machado.
     * @param r Componente rossa normalizzata [0,1].
     * @param g Componente verde normalizzata [0,1].
     * @param b Componente blu normalizzata [0,1].
     *
     * @return Colore trasformato nello spazio RGB.
     */
    inline RGB3d applyMachadoMatrix(const Mat3& M, double r, double g, double b) {
        RGB3d out;
        out.r = std::max(0.0, std::min(1.0, M(0, 0) * r + M(0, 1) * g + M(0, 2) * b));
        out.g = std::max(0.0, std::min(1.0, M(1, 0) * r + M(1, 1) * g + M(1, 2) * b));
        out.b = std::max(0.0, std::min(1.0, M(2, 0) * r + M(2, 1) * g + M(2, 2) * b));
        return out;
    }

    /**
     * Interpola linearmente due matrici 3x3.
     *
     * Utilizzata per ottenere matrici di severità intermedie tra
     * due livelli discreti della simulazione Machado.
     *
     * @param A Prima matrice.
     * @param B Seconda matrice.
     * @param t Fattore di interpolazione [0,1].
     *
     * @return Matrice interpolata.
     */
    inline Mat3 interpolateMat(const Mat3& A, const Mat3& B, double t) {
        Mat3 R;
        for (int i = 0; i < 9; ++i)
            R.m[i] = A.m[i] * (1.0 - t) + B.m[i] * t;
        return R;
    }

    /**
     * Restituisce la matrice Machado associata ad una specifica
     * tipologia e severità di deficit cromatico.
     *
     * Se la severità non coincide con un livello discreto viene
     * effettuata un'interpolazione lineare tra le matrici adiacenti.
     *
     * @param cvdType Tipo di deficit cromatico.
     * @param severity Severità normalizzata [0,1].
     *
     * @return Matrice di simulazione 3x3.
     */
    inline Mat3 getMachadoMatrix(int cvdType, double severity) {
        severity = std::max(0.0, std::min(1.0, severity));
        int    idx = (int)(severity * 10.0);
        double t = severity * 10.0 - idx;
        if (idx > 9) idx = 9;

        const Mat3* T = (cvdType == PROTAN) ? PROTAN_MAT
            : (cvdType == DEUTAN) ? DEUTAN_MAT
            : TRITAN_MAT;

        if (t < 1e-9) return T[idx];
        return interpolateMat(T[idx], T[idx + 1], t);
    }

    /**
     * Restituisce il confusion point associato ad uno specifico
     * difetto della visione dei colori.
     *
     * Il confusion point rappresenta il punto verso cui convergono
     * le confusion lines nel diagramma cromatico CIE xy.
     *
     * @param cvdType Tipo di deficit cromatico.
     *
     * @return Coordinate xy del confusion point.
     */
    inline xy confusionPoint(int cvdType) {
        switch (cvdType) {
        case PROTAN: { xy p = { 0.7465,  0.2535 }; return p; }
        case DEUTAN: { xy p = { 1.4000, -0.4000 }; return p; }
        case TRITAN: { xy p = { 0.1748,  0.0000 }; return p; }
        default: { xy p = { 0.3333,  0.3333 }; return p; }
        }
    }

    /**
     * Converte coordinate cromatiche CIE xyY nello spazio XYZ.
     *
     * @param x Coordinata cromatica x.
     * @param y Coordinata cromatica y.
     * @param Y Luminanza.
     *
     * @return Colore nello spazio CIE XYZ.
     */
    inline XYZ xyYtoXYZ(double x, double y, double Y = 0.4) {
        if (y < 1e-10) { XYZ r = { 0,0,0 }; return r; }
        XYZ r = { (x / y) * Y, Y, ((1.0 - x - y) / y) * Y };
        return r;
    }

    /**
     * Converte un colore dallo spazio CIE XYZ allo spazio RGB lineare.
     *
     * @param xyz Colore nello spazio XYZ.
     *
     * @return Colore RGB lineare.
     */
    inline RGBf XYZtoLinearRGB(XYZ xyz) {
        RGBf r = {
             3.2406 * xyz.X - 1.5372 * xyz.Y - 0.4986 * xyz.Z,
            -0.9689 * xyz.X + 1.8758 * xyz.Y + 0.0415 * xyz.Z,
             0.0557 * xyz.X - 0.2040 * xyz.Y + 1.0570 * xyz.Z
        };
        return r;
    }

    /**
     * Applica la correzione gamma sRGB ad una componente RGB lineare.
     *
     * @param c Componente lineare normalizzata.
     *
     * @return Componente corretta secondo lo standard sRGB.
     */
    inline double toSRGB(double c) {
        c = std::max(0.0, std::min(1.0, c));
        return c <= 0.0031308 ? 12.92 * c : 1.055 * std::pow(c, 1.0 / 2.4) - 0.055;
    }

    /**
     * Converte coordinate cromatiche CIE xy in un colore RGB a 8 bit.
     *
     * @param x Coordinata cromatica x.
     * @param y Coordinata cromatica y.
     * @param Y Luminanza.
     *
     * @return Colore RGB con componenti intere [0,255].
     */
    inline RGB xyToRGB(double x, double y, double Y = 0.40) {
        XYZ  xyz = xyYtoXYZ(x, y, Y);
        RGBf lin = XYZtoLinearRGB(xyz);
        RGB  out = {
            (int)std::round(toSRGB(lin.r) * 255.0),
            (int)std::round(toSRGB(lin.g) * 255.0),
            (int)std::round(toSRGB(lin.b) * 255.0)
        };
        return out;
    }

    /**
     * Converte coordinate CIE xy in un colore RGB normalizzato
     * utilizzabile direttamente da ImGui.
     *
     * @param x Coordinata cromatica x.
     * @param y Coordinata cromatica y.
     * @param Y Luminanza.
     *
     * @return Colore RGB normalizzato [0,1].
     */
    inline RGBf32 xyToImGui(double x, double y, double Y = 0.40) {
        XYZ    xyz = xyYtoXYZ(x, y, Y);
        RGBf   lin = XYZtoLinearRGB(xyz);
        RGBf32 out = {
            (float)toSRGB(lin.r),
            (float)toSRGB(lin.g),
            (float)toSRGB(lin.b)
        };
        return out;
    }

    struct Anchor { double x, y; };

    /**
     * Restituisce uno dei colori di riferimento utilizzati dal test.
     *
     * Ogni asse cromatico dispone di due anchor selezionate
     * sperimentalmente per generare gli stimoli.
     *
     * @param cvdType Tipo di asse cromatico.
     * @param idx Indice dell'anchor.
     *
     * @return Coordinate cromatiche dell'anchor.
     */
    inline Anchor getAnchor(int cvdType, int idx) {
        static const Anchor T[3][2] = {
            { {0.450, 0.320}, {0.390, 0.345} },
            { {0.290, 0.510}, {0.255, 0.460} },
            { {0.240, 0.310}, {0.205, 0.355} },
        };
        return T[cvdType][idx % 2];
    }

    /**
     * Genera un colore spostato lungo la confusion line.
     *
     * Lo spostamento avviene nella direzione del confusion point
     * associato al deficit cromatico considerato.
     *
     * @param bx Coordinata x del colore base.
     * @param by Coordinata y del colore base.
     * @param cvdType Tipo di deficit cromatico.
     * @param delta Entità dello spostamento.
     *
     * @return Nuovo colore espresso nel piano CIE xy.
     */
    inline xy getShiftedColor(double bx, double by, int cvdType, double delta) {
        xy     cp = confusionPoint(cvdType);
        double dx = cp.x - bx;
        double dy = cp.y - by;
        double len = std::sqrt(dx * dx + dy * dy);
        if (len < 1e-10) { xy r = { bx, by }; return r; }
        xy r = { bx + (dx / len) * delta, by + (dy / len) * delta };
        return r;
    }

    inline const char* typeName(int t) {
        switch (t) {
        case PROTAN: return "Protanomalia";
        case DEUTAN: return "Deuteranomalia";
        case TRITAN: return "Tritanomalia";
        default:     return "Normale";
        }
    }

    inline const char* axisLabel(int t) {
        switch (t) {
        case PROTAN: return "Asse Rosso (L-cono)";
        case DEUTAN: return "Asse Verde (M-cono)";
        case TRITAN: return "Asse Blu-Giallo (S-cono)";
        default:     return "";
        }
    }

    inline RGBf32 typeColor(int t) {
        switch (t) {
        case PROTAN: { RGBf32 c = { 1.00f, 0.35f, 0.35f }; return c; }
        case DEUTAN: { RGBf32 c = { 0.30f, 0.85f, 0.40f }; return c; }
        case TRITAN: { RGBf32 c = { 0.35f, 0.60f, 1.00f }; return c; }
        default: { RGBf32 c = { 0.70f, 0.70f, 0.70f }; return c; }
        }
    }

}