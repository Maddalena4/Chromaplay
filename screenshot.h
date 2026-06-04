#pragma once
#define _CRT_SECURE_NO_WARNINGS


#include <glad/glad.h>
#include <cstdint>
#include <cstring>
#include <cstdio>
#include <string>
#include <vector>

/**
 * Inserisce un valore intero a 32 bit all'interno di un buffer
 * di byte utilizzando il formato big-endian.
 *
 * Utilizzata durante la costruzione manuale della struttura
 * interna del file PNG.
 *
 * @param b Buffer di destinazione.
 * @param v Valore a 32 bit da inserire.
 *
 * @return void
 */
static inline void _pu32(std::vector<uint8_t>& b, uint32_t v)
{
    b.push_back((v >> 24) & 0xFF); b.push_back((v >> 16) & 0xFF);
    b.push_back((v >> 8) & 0xFF); b.push_back((v) & 0xFF);
}

/**
 * Inserisce un valore intero a 16 bit all'interno di un buffer
 * utilizzando il formato little-endian.
 *
 * Utilizzata durante la costruzione dei blocchi zlib.
 *
 * @param b Buffer di destinazione.
 * @param v Valore a 16 bit.
 *
 * @return void
 */
static inline void _pu16le(std::vector<uint8_t>& b, uint16_t v)
{
    b.push_back(v & 0xFF); b.push_back((v >> 8) & 0xFF);
}

static uint32_t _crc32_table[256];
static bool     _crc32_ready = false;

/**
 * Inizializza la tabella utilizzata per il calcolo del checksum CRC32.
 *
 * L'inizializzazione viene eseguita una sola volta e riutilizzata
 * per tutti i file PNG generati successivamente.
 *
 * @param Nessuno.
 *
 * @return void
 */
static void _crc32_init() {
    if (_crc32_ready) return;
    for (uint32_t i = 0; i < 256; i++) {
        uint32_t c = i;
        for (int k = 0; k < 8; k++) c = (c & 1) ? (0xEDB88320u ^ (c >> 1)) : (c >> 1);
        _crc32_table[i] = c;
    }
    _crc32_ready = true;
}

/**
 * Calcola il checksum CRC32 di una sequenza di byte.
 *
 * Il CRC viene utilizzato dal formato PNG per verificare
 * l'integrità dei chunk memorizzati nel file.
 *
 * @param crc Valore iniziale del CRC.
 * @param data Puntatore ai dati da elaborare.
 * @param len Numero di byte da processare.
 *
 * @return Valore CRC32 calcolato.
 */
static uint32_t _crc32(uint32_t crc, const uint8_t* data, size_t len) {
    _crc32_init(); crc = ~crc;
    for (size_t i = 0; i < len; i++) crc = _crc32_table[(crc ^ data[i]) & 0xFF] ^ (crc >> 8);
    return ~crc;
}

/**
 * Costruisce un chunk PNG e lo aggiunge al file in memoria.
 *
 * Ogni chunk contiene:
 * - lunghezza;
 * - identificatore del tipo;
 * - dati;
 * - checksum CRC32.
 *
 * @param out Buffer contenente il file PNG.
 * @param type Tipo del chunk (IHDR, IDAT, IEND).
 * @param data Contenuto del chunk.
 *
 * @return void
 */
static void _png_chunk(std::vector<uint8_t>& out, const char type[4], const std::vector<uint8_t>& data) {
    _pu32(out, (uint32_t)data.size());
    uint32_t crc = _crc32(0, (const uint8_t*)type, 4);
    out.push_back(type[0]); out.push_back(type[1]); out.push_back(type[2]); out.push_back(type[3]);
    if (!data.empty()) { crc = _crc32(crc, data.data(), data.size()); out.insert(out.end(), data.begin(), data.end()); }
    _pu32(out, crc);
}

/**
 * Calcola il checksum Adler-32.
 *
 * Utilizzato dal formato zlib per verificare l'integrità
 * dei dati compressi.
 *
 * @param data Puntatore ai dati.
 * @param len Numero di byte.
 *
 * @return Checksum Adler-32.
 */
static uint32_t _adler32(const uint8_t* data, size_t len) {
    uint32_t s1 = 1, s2 = 0;
    for (size_t i = 0; i < len; i++) { s1 = (s1 + data[i]) % 65521; s2 = (s2 + s1) % 65521; }
    return (s2 << 16) | s1;
}

/**
 * Costruisce il contenuto del chunk IDAT di un file PNG.
 *
 * I dati vengono incapsulati in una struttura zlib senza
 * compressione, mantenendo la compatibilità con il formato PNG.
 *
 * @param raw Buffer contenente i pixel dell'immagine.
 * @param rawLen Numero di byte presenti nel buffer.
 *
 * @return Vettore contenente il contenuto del chunk IDAT.
 */
static std::vector<uint8_t> _build_idat(const uint8_t* raw, size_t rawLen) {
    std::vector<uint8_t> idat;
    idat.push_back(0x78); idat.push_back(0x01); 
    const uint32_t BLOCK = 65535;
    size_t offset = 0;
    while (offset < rawLen) {
        size_t  chunkLen = rawLen - offset;
        bool    last = (chunkLen <= BLOCK);
        uint16_t len = (uint16_t)(last ? chunkLen : BLOCK);
        uint16_t nlen = ~len;
        idat.push_back(last ? 0x01 : 0x00);
        _pu16le(idat, len); _pu16le(idat, nlen);
        idat.insert(idat.end(), raw + offset, raw + offset + len);
        offset += len;
    }
    uint32_t a = _adler32(raw, rawLen);
    _pu32(idat, a);
    return idat;
}

/**
 * Acquisisce il framebuffer OpenGL corrente e lo salva
 * in formato PNG.
 *
 * La funzione legge i pixel renderizzati tramite OpenGL,
 * costruisce manualmente la struttura PNG e scrive il file
 * sul disco.
 *
 * Deve essere invocata dopo il rendering della scena e
 * prima dello scambio dei buffer video.
 *
 * @param filepath Percorso del file PNG da creare.
 * @param w Larghezza dell'immagine in pixel.
 * @param h Altezza dell'immagine in pixel.
 *
 * @return void
 */
inline void salvaScreenshot(const std::string& filepath, int w, int h)
{
    std::vector<uint8_t> pixels((size_t)w * h * 3);
    glReadPixels(0, 0, w, h, GL_RGB, GL_UNSIGNED_BYTE, pixels.data());

    const int stride = w * 3;
    std::vector<uint8_t> raw((size_t)(stride + 1) * (size_t)h);
    for (int row = 0; row < h; row++) {
        uint8_t* dst = raw.data() + (size_t)row * (stride + 1);
        const uint8_t* src = pixels.data() + (size_t)(h - 1 - row) * stride;
        dst[0] = 0x00;
        std::memcpy(dst + 1, src, stride);
    }

    std::vector<uint8_t> png;
    const uint8_t sig[] = { 0x89,'P','N','G','\r','\n',0x1A,'\n' };
    png.insert(png.end(), sig, sig + 8);

    { 
        std::vector<uint8_t> ihdr(13);
        ihdr[0] = (w >> 24) & 0xFF; ihdr[1] = (w >> 16) & 0xFF; ihdr[2] = (w >> 8) & 0xFF; ihdr[3] = w & 0xFF;
        ihdr[4] = (h >> 24) & 0xFF; ihdr[5] = (h >> 16) & 0xFF; ihdr[6] = (h >> 8) & 0xFF; ihdr[7] = h & 0xFF;
        ihdr[8] = 8; ihdr[9] = 2; ihdr[10] = 0; ihdr[11] = 0; ihdr[12] = 0;
        _png_chunk(png, "IHDR", ihdr);
    }
    { 
        auto idat = _build_idat(raw.data(), raw.size());
        _png_chunk(png, "IDAT", idat);
    }
    _png_chunk(png, "IEND", {}); 

    FILE* f = nullptr;
    fopen_s(&f, filepath.c_str(), "wb");
    if (f) { fwrite(png.data(), 1, png.size(), f); fclose(f); printf("[Screenshot] %s\n", filepath.c_str()); }
    else { fprintf(stderr, "[ERRORE] Non riesco ad aprire: %s\n", filepath.c_str()); }
}