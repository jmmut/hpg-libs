#ifndef VCF_FILE_H
#define VCF_FILE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <limits.h>

#include <commons/file_utils.h>
#include <commons/log.h>
#include <containers/array_list.h>
#include <containers/list.h>

#include "vcf_file_structure.h"
#include "vcf_reader.h"
#include "vcf_util.h"
#include "vcf_write.h"

/**
 * @file vcf_file.h
 * @author Cristina Yenyxe Gonzalez Garcia
 * @brief Entry point for VCF files management (open/close, reading/writing)
 * @details This file acts as the entry point to the library. It includes functions for opening and closing 
 * VCF files, reading them using different policies and also writing its contents to another destination.
 * 
 * For the sake of memory usage and load balancing, files are loaded in blocks known as "batches", whose size can 
 * be specified in lines or bytes. Files can also be stored in plain text or compressed in GZIP format. The last
 * configurable parameter sets whether VCF files will be read using the FILE API or the mmap function (setting 
 * global variable <em>mmap_vcf</em>).
 * 
 * @see vcf_file_structure.h
 * @see vcf_util.h
 */


/** @cond PRIVATE */
extern int mmap_vcf;
/** @endcond */


/* **************************************
 *       General file management        *
 * **************************************/

/**
 * @brief Open a file stream and initialize the associated vcf_file_t structure.
 * @details Open a file stream and initialize the associated vcf_file_t structure.
 * 
 * @param filename The name of the file to open
 * @param max_simultaneous_batches Maximum number of batches that can be loaded in memory (<= 0 if unlimited)
 */
vcf_file_t *vcf_open(char *filename, size_t max_simultaneous_batches);

/**
 * @brief Creates a vcf_file_t structure from scratch.
 * @details Creates a vcf_file_t structure from scratch, without loading contents from any physical file.
 * 
 * @param filename The name of the file to create
 * @param max_simultaneous_batches Maximum number of batches that can be loaded in memory (<= 0 if unlimited)
 */
vcf_file_t *vcf_file_new(char *filename, size_t max_simultaneous_batches);

/**
 * @brief Closes the file stream associated to a vcf_file_t type.
 * @details Closes the file stream associated to a vcf_file_t type and deallocates memory.
 * 
 * @param file vcf_file_t whose file stream is about to being closed
 */
void vcf_close(vcf_file_t *file);


/* **************************************
 *              File reading            *
 * **************************************/


/**
 * @brief Reads and parses the contents of a VCF file, storing them in batches
 * @details Read and parse the given number of lines from a VCF file. The file can be stored as plain text 
 * or compressed using GZIP format. The data read will be stored in members of the vcf_file_t structure.
 * 
 * The user can also decide whether to parse and store the value of each sample or ignore them for 
 * the sake of speed and memory usage.
 *
 * @param batch_lines The number of lines to read and parse
 * @param file The file the data will be read from
 * @param read_samples Whether to parse the values of the sequenced samples
 * @return 0 if the file was successfully read and parsed, 1 otherwise
 **/
int vcf_parse_batches(size_t batch_lines, vcf_file_t *file, int read_samples);

/**
 * @brief Read and parse the given number of bytes from a VCF file
 * @details Read and parse the given number of bytes from a VCF file. The file can be stored as plain text 
 * or compressed using GZIP format. If the last byte does not correspond to a linebreak character, it keeps 
 * reading until next linebreak is found. The data read will be stored in members of the vcf_file_t structure.
 * 
 * The user can also decide whether to parse and store the value of each sample or ignore them for 
 * the sake of speed and memory usage.
 * 
 * @param batch_bytes The number of bytes to read and parse
 * @param file The file the data will be read from
 * @param read_samples Whether to parse the values of the sequenced samples
 * @return 0 if the file was successfully read and parsed, 1 otherwise
 **/
int vcf_parse_batches_in_bytes(size_t batch_bytes, vcf_file_t *file, int read_samples);

/**
 * @brief Read (without parsing) the given number of lines from a VCF file
 * @details Read the given number of lines from a VCF file. The file can be stored as plain text 
 * or compressed using GZIP format. The data read will be stored in the list given as argument.
 *
 * @param text_list [out] List of blocks where the current text block will be queued
 * @param batch_lines The number of lines to read
 * @param file The file the data will be read from
 * @return 0 if the file was successfully read, 1 otherwise
 **/
int vcf_read_batches(list_t *text_list, size_t batch_lines, vcf_file_t *file);

/**
 * @brief Read (without parsing) the given number of bytes from a VCF file
 * @details Read the given number of bytes from a VCF file. The file can be stored as plain text 
 * or compressed using GZIP format. The data read will be stored in the list given as argument.
 *
 * @param text_list [out] List of blocks where the current text block will be queued
 * @param batch_bytes The number of bytes to read
 * @param file The file the data will be read from
 * @return 0 if the file was successfully read, 1 otherwise
 **/
int vcf_read_batches_in_bytes(list_t *text_list, size_t batch_bytes, vcf_file_t *file);

/** @cond PRIVATE */
// int vcf_multiread_batches(list_t **batches_list, size_t batch_size, vcf_file_t **vcf_files, int num_files);
/** @endcond */

/**
 * @brief Notifies when the VCF file has been fully read.
 * @details Notifies when the VCF file has been fully read, performing the corresonding clean-up tasks.
 *
 * @param file The read file
 **/
void notify_end_reading(vcf_file_t *file);


/* **************************************
 *              File writing            *
 * **************************************/

/**
 * @brief Writes the contents of the vcf_file_t given as argument to the given path.
 * @details Writes the contents of the vcf_file_t given as argument to the given path.
 * 
 * @param file The vcf_file_t whose information will be written
 * @param filename The path of the file to write the information to
 */
int vcf_write(vcf_file_t *file, char *filename);


#endif
