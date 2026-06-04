#pragma once
#include <string>
#include <unordered_map>
#include <algorithm>
#include <glad/glad.h>

extern "C" {
    unsigned char* stbi_load(const char*, int*, int*, int*, int);
    void stbi_image_free(void*);
}

class ImageLoader {
public:
    ImageLoader() : imageDir_("images") {}

    /**
     * Imposta la directory contenente il dataset di immagini.
     *
     * Prima di aggiornare il percorso, libera tutte le texture
     * attualmente presenti nella cache GPU per evitare perdite
     * di memoria.
     *
     * @param dir Percorso della cartella contenente le immagini.
     *
     * @return void
     */
    void setImageDir(const std::string& dir) {
        imageDir_ = dir;
        for (auto& kv : cache_)
            glDeleteTextures(1, &kv.second);
        cache_.clear();
    }
    /**
     * Restituisce il prefisso associato ad una tipologia di
     * deficit cromatico.
     *
     * Il prefisso viene utilizzato per costruire il nome dei
     * file immagine presenti nel dataset.
     *
     * Mappatura:
     * - 0 -> protanomalia
     * - 1 -> deuteranomalia
     * - 2 -> tritanomalia
     *
     * @param cvdType Tipo di deficit cromatico.
     *
     * @return Stringa contenente il prefisso del file.
     */
    static std::string typePrefix(int cvdType) {
        switch (cvdType) {
        case 0: return "protanomalia";
        case 1: return "deuteranomalia";
        case 2: return "tritanomalia";
        default: return "unknown";
        }
    }
    /**
     * Restituisce il percorso completo dell'immagine associata
     * ad una determinata tipologia e severità di deficit.
     *
     * @param cvdType Tipo di deficit cromatico.
     * @param percent Percentuale di severità (1-100).
     *
     * @return Percorso completo del file immagine.
     */
    std::string getPath(int cvdType, int percent) const {
        return buildPath(cvdType, percent);
    }

    /**
     * Carica e restituisce una texture OpenGL.
     *
     * La funzione verifica innanzitutto la presenza della texture
     * nella cache. Se già caricata, restituisce direttamente
     * l'identificatore OpenGL esistente.
     *
     * In caso contrario:
     * - costruisce il percorso del file;
     * - carica l'immagine dal disco;
     * - crea la texture sulla GPU;
     * - memorizza la texture nella cache.
     *
     * @param cvdType Tipo di deficit cromatico.
     * @param percent Percentuale di severità (1-100).
     *
     * @return Identificatore OpenGL della texture (GLuint).
     */
    GLuint getTexture(int cvdType, int percent) {
        percent = std::max(1, std::min(100, percent));
        int key = cvdType * 1000 + percent;

        auto it = cache_.find(key);
        if (it != cache_.end()) return it->second;

        std::string path = buildPath(cvdType, percent);
        GLuint tex = loadFromFile(path);
        cache_[key] = tex;
        return tex;
    }

    /**
     * Libera tutte le texture presenti nella cache.
     *
     * La funzione elimina le texture dalla memoria GPU tramite
     * OpenGL e svuota la struttura dati utilizzata per il caching.
     *
     * @param Nessuno.
     *
     * @return void
     */
    void release() {
        for (auto& kv : cache_)
            glDeleteTextures(1, &kv.second);
        cache_.clear();
    }

    /**
     * Distruttore della classe ImageLoader.
     *
     * Garantisce il rilascio automatico di tutte le risorse GPU
     * associate alle texture caricate.
     *
     * @param Nessuno.
     *
     * @return Nessuno.
     */
    ~ImageLoader() { release(); }

    public:
    std::string imageDir_;
    std::unordered_map<int, GLuint> cache_;

    /**
     * Costruisce il nome completo del file immagine.
     *
     * Il formato generato è:
     *
     *   <directory>/<tipo>_XXX.png
     *
     * dove XXX rappresenta la severità formattata a tre cifre.
     *
     * Esempio:
     *
     *   images/deuteranomalia_055.png
     *
     * @param cvdType Tipo di deficit cromatico.
     * @param percent Percentuale di severità.
     *
     * @return Percorso completo del file.
     */
    std::string buildPath(int cvdType, int percent) const {
        char num[8];
        snprintf(num, sizeof(num), "%03d", percent);
        return imageDir_ + "/" + typePrefix(cvdType) + "_" + num + ".png";
    }

    /**
     * Carica un'immagine dal disco e crea la corrispondente
     * texture OpenGL.
     *
     * L'immagine viene letta tramite la libreria stb_image e
     * successivamente trasferita nella memoria GPU utilizzando
     * le API OpenGL.
     *
     * I parametri di filtraggio della texture vengono impostati
     * in modalità lineare.
     *
     * @param path Percorso dell'immagine da caricare.
     *
     * @return Identificatore OpenGL della texture generata.
     *         Restituisce 0 in caso di errore.
     */
    GLuint loadFromFile(const std::string& path) {
        int w, h, ch;
        unsigned char* data = stbi_load(path.c_str(), &w, &h, &ch, 4);
        if (!data) return 0;

        GLuint tex;
        glGenTextures(1, &tex);
        glBindTexture(GL_TEXTURE_2D, tex);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0,
            GL_RGBA, GL_UNSIGNED_BYTE, data);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
        return tex;
    }
};