#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <curl/curl.h>

// Structure pour stocker l'URL et le fichier de sortie
typedef struct {
    char *url;
    char *filename;
} download_info_t;

// Fonction pour afficher la progression de chaque téléchargement
size_t write_callback(void *ptr, size_t size, size_t nmemb, FILE *stream) {
    size_t written = fwrite(ptr, size, nmemb, stream);
    return written;
}

// Fonction pour calculer et afficher la progression du téléchargement
static size_t progress_callback(void *clientp, curl_off_t dltotal, curl_off_t dlnow,
                                curl_off_t ultotal, curl_off_t ulnow) {
    if (dltotal == 0) {
        return 0;
    }
    double progress = (dlnow / (double) dltotal) * 100;
    printf("Téléchargement progress : %.2f%%\n", progress);
    fflush(stdout);
    return 0;
}

// Fonction de téléchargement pour chaque fichier
void* download_file(void* arg) {
    download_info_t *info = (download_info_t*)arg;
    CURL *curl;
    CURLcode res;
    FILE *fp;
    
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    
    if (curl) {
        fp = fopen(info->filename, "wb");
        if (!fp) {
            perror("Erreur d'ouverture du fichier");
            return NULL;
        }

        // Configurer l'URL et le fichier de sortie
        curl_easy_setopt(curl, CURLOPT_URL, info->url);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
        curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, progress_callback);
        curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L); // Activer la progression
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L); // Activer le mode verbose pour le debug

        // Effectuer le téléchargement
        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            fprintf(stderr, "Erreur de téléchargement: %s\n", curl_easy_strerror(res));
        }

        fclose(fp);
        curl_easy_cleanup(curl);
    }

    curl_global_cleanup();
    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <url1> <fichier1> [<url2> <fichier2> ...]\n", argv[0]);
        return EXIT_FAILURE;
    }

    pthread_t threads[argc / 2];  // Un thread par téléchargement
    download_info_t download_info[argc / 2];

    // Créer un thread pour chaque téléchargement
    for (int i = 1; i < argc; i += 2) {
        download_info[i / 2].url = argv[i];
        download_info[i / 2].filename = argv[i + 1];

        if (pthread_create(&threads[i / 2], NULL, download_file, &download_info[i / 2]) != 0) {
            perror("Erreur lors de la création du thread");
            return EXIT_FAILURE;
        }
    }

    // Attendre que tous les threads terminent
    for (int i = 0; i < argc / 2; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("Tous les téléchargements sont terminés.\n");

    return EXIT_SUCCESS;
}

