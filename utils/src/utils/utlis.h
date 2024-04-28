#ifndef UTILS_H
#define UTILS_H

#include <string.h>
#include <commons/string.h>
#include <commons/collections/list.h>
#include <stdio.h>
#include<time.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdint.h>

#define BUFFER_MAX_LENGTH 100

/**
 * @fn    string_arr_as_string
 * @brief convierte un array de strings en un string del tipo "[str1,str2,str3]" SI O SI TIENE QUE TERMINAR EN NULL SI NO ANDA
 */
char *string_arr_as_string(char **string_arr);

/**
 * @fn    file_get_list_of_lines
 * @brief de un archivo, devuelve una lista con todas las lineas sin el '\n'
 */
t_list *file_get_list_of_lines(char *file_path);

/**
 * @fn    file_get_nth_line
 * @brief de un archivo, devuelve la enesima linea sin el '\n' arranca en 0
 */
char *file_get_nth_line(char *file_path, int n);

int msToSeconds(int seconds);

int msleep(long msec);

#endif