#pragma once

class ShaderMaker
{
    public:
		/**
		 * @brief Crea un programma shader completo da file sorgente.
		 *
		 * Compila i file di vertex e fragment shader e li collega in un unico programma.
		 * In caso di errore di compilazione, stampa il log di errore su console.
		 *
		 * @param vertexfilename Percorso del file vertex shader.
		 * @param fragmentfilename Percorso del file fragment shader.
		 * @return ID del programma shader OpenGL (GLuint).
		*/
		static char* readShaderSource(const char* shaderFile);

		/**
		 * @brief Legge il contenuto di un file shader.
		 *
		 * Apre un file di testo e ne legge tutto il contenuto, restituendolo come
		 * stringa null-terminata.
		 *
		 * @param shaderFile Percorso del file shader.
		 * @return Puntatore a buffer contenente il codice sorgente dello shader,
		 *         oppure NULL in caso di errore.
		 */
		static GLuint createProgram(char* vertexfilename, char *fragmentfilename);
		
	
    private:
		ShaderMaker() { }	
};