#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zip.h>

#define MAX_FILAS 200
#define MAX_COLUMNAS 8
#define MAX_LONGITUD_CADENA 100

// Estructura para almacenar los datos del CSV
typedef struct {
    char datos[MAX_FILAS][MAX_COLUMNAS][MAX_LONGITUD_CADENA];
    int filas;
    int columnas;
} MatrizCSV;

// Función para leer el archivo CSV y cargar los datos en la estructura MatrizCSV
int leer_csv(const char *nombre_archivo, MatrizCSV *matriz) {
    FILE *archivo = fopen(nombre_archivo, "r");
    if (archivo == NULL) {
        printf("No se pudo abrir el archivo %s\n", nombre_archivo);
        return 0;
    }

    char linea[MAX_LONGITUD_CADENA * MAX_COLUMNAS];
    int fila = 0;

    while (fgets(linea, sizeof(linea), archivo) && fila < MAX_FILAS) {
        char *token = strtok(linea, ",");
        int columna = 0;

        while (token != NULL && columna < MAX_COLUMNAS) {
            strncpy(matriz->datos[fila][columna], token, MAX_LONGITUD_CADENA - 1);
            token = strtok(NULL, ",");
            columna++;
        }

        if (fila == 0) {
            matriz->columnas = columna;
        }

        fila++;
    }

    matriz->filas = fila;
    fclose(archivo);
    return 1;
}

// Función para extraer el archivo CSV desde un ZIP
int extraer_csv_zip(const char *nombre_zip, const char *nombre_csv, const char *salida_csv) {
    int err = 0;
    zip_t *zip_archivo = zip_open(nombre_zip, 0, &err);

    if (zip_archivo == NULL) {
        printf("Error al abrir el archivo ZIP: %d\n", err);
        return 0;
    }

    struct zip_stat st;
    zip_stat_init(&st);
    if (zip_stat(zip_archivo, nombre_csv, 0, &st) != 0) {
        printf("No se encontró el archivo %s en el ZIP.\n", nombre_csv);
        zip_close(zip_archivo);
        return 0;
    }

    char *contenido_csv = malloc(st.size);
    if (contenido_csv == NULL) {
        printf("Error al asignar memoria para el contenido del CSV\n");
        zip_close(zip_archivo);
        return 0;
    }

    zip_file_t *archivo_csv = zip_fopen(zip_archivo, nombre_csv, 0);
    if (archivo_csv == NULL) {
        printf("Error al abrir el archivo CSV dentro del ZIP\n");
        free(contenido_csv);
        zip_close(zip_archivo);
        return 0;
    }

    zip_fread(archivo_csv, contenido_csv, st.size);
    contenido_csv[st.size] = '\0';  // Aseguramos que sea una cadena bien terminada

    // Guardar el contenido en un archivo temporal para leerlo con la función leer_csv
    FILE *temp = fopen(salida_csv, "w");
    if (temp == NULL) {
        printf("Error al crear el archivo temporal\n");
        free(contenido_csv);
        zip_fclose(archivo_csv);
        zip_close(zip_archivo);
        return 0;
    }

    fwrite(contenido_csv, sizeof(char), st.size, temp);
    fclose(temp);

    // Cerrar el archivo ZIP y liberar recursos
    zip_fclose(archivo_csv);
    zip_close(zip_archivo);
    free(contenido_csv);

    return 1;
}

// Función para mostrar el menú
void mostrar_menu() {
    printf("1. Cargar Dataset 1 desde ZIP\n");
    printf("2. Cargar Dataset 2 desde ZIP\n");
    printf("7. Salir\n");
}

// Función principal que maneja el menú
void ejecutar_menu() {
    MatrizCSV matriz1, matriz2;
    int opcion;

    do {
        mostrar_menu();
        printf("Selecciona una opción: ");
        scanf("%d", &opcion);

        switch (opcion) {
            case 1:
                // Extraer y cargar el primer dataset desde un ZIP
                if (extraer_csv_zip("dataset1.zip", "dataset1.csv", "dataset1_temporal.csv")) {
                    if (leer_csv("dataset1_temporal.csv", &matriz1)) {
                        printf("Datos del dataset 1 cargados correctamente.\n");
                    } else {
                        printf("Error al cargar los datos del dataset 1.\n");
                    }
                } else {
                    printf("Error al extraer el archivo CSV del ZIP dataset1.\n");
                }
                break;

            case 2:
                // Extraer y cargar el segundo dataset desde otro ZIP
                if (extraer_csv_zip("dataset2.zip", "dataset2.csv", "dataset2_temporal.csv")) {
                    if (leer_csv("dataset2_temporal.csv", &matriz2)) {
                        printf("Datos del dataset 2 cargados correctamente.\n");
                    } else {
                        printf("Error al cargar los datos del dataset 2.\n");
                    }
                } else {
                    printf("Error al extraer el archivo CSV del ZIP dataset2.\n");
                }
                break;

            case 7:
                printf("Saliendo de la aplicación...\n");
                break;

            default:
                printf("Opción no válida. Inténtalo de nuevo.\n");
        }
    } while (opcion != 7);
}

int main() {
    ejecutar_menu();
    return 0;
}
