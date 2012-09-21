#ifndef VCF_FILTERS_H 
#define VCF_FILTERS_H

#include <assert.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include <cprops/heap.h>

#include <containers/array_list.h>
#include <containers/list.h>
#include <containers/region_table_utils.h>
#include <commons/log.h>

#include "vcf_file_structure.h"
#include "vcf_stats.h"
#include "vcf_util.h"

/**
 * @file vcf_filters.h
 * @author Cristina Yenyxe Gonzalez Garcia
 * @brief Filters for VCF files
 * @details This file includes functions for filtering VCF files. All filters receive as input a list of 
 * records, and return two lists: one with the records that passed the filters and another with the ones 
 * that were rejected.
 */



/**
 * @brief The type of the filter to apply
 **/
enum filter_type { COVERAGE, NUM_ALLELES, QUALITY, REGION, SNP  };

/**
 * @brief Arguments for the filter by coverage
 * @details The only argument of a filter by coverage is the minimum coverage of a record, as specified 
 * on its INFO field.
 **/
typedef struct {
    int min_coverage;   /**< Minimum coverage a record must have */
} coverage_filter_args;

/**
 * @brief Arguments for the filter by number of alleles
 * @details The only argument of a filter by number of alleles is precisely that number.
 **/
typedef struct {
    int num_alleles;    /**< Number of alleles a record must have */
} num_alleles_filter_args;

/**
 * @brief Arguments for the filter by quality
 * @details The only argument of a filter by quality is the minimum quality of a record, as specified on its 
 * QUAL field.
 **/
typedef struct {
    int min_quality;   /**< Minimum quality a record must have */
} quality_filter_args;

/**
 * @brief Arguments for the filter by region
 * @details The argument of a filter by region is a set of one or more regions of the form 
 * chromosome:position:ref_allele:alt_allele.
 **/
typedef struct {
    region_table_t *regions;    /**< Region(s) where a record must be placed */
} region_filter_args;

/**
 * @brief Arguments for the filter by SNP
 * @details The only argument of a filter by SNP specifies whether to include or exclude a SNP.
 **/
typedef struct {
    int include_snps;   /**< Whether to include (1) or exclude (0) a SNP */
} snp_filter_args;

/**
 * @brief A filter selects a subcollection of records which fulfill some condition.
 * @details A filter selects a subcollection of records which fulfill some condition.
 * It is mandatory to provide the list of records to filter and a list to store in 
 * the records that failed the filter's test.
 * 
 * If more than one filter is applied, they must be ordered by priority (max = 0).
 */
typedef struct filter {
    unsigned int priority;  /**< Sorting criteria when several filters are applied */
    enum filter_type type;  /**< Filtering criteria */
    array_list_t* (*filter_func) (array_list_t *input_records, array_list_t *failed, void *args);  /**< Filtering function itself */
    void (*free_func) (struct filter *f);   /**< Filter deallocation function */
    void *args;             /**< Filter-dependant arguments */
} filter_t;

/**
 * @brief Chain of filters to apply.
 * @details Chain of filters to apply.
 **/
typedef cp_heap filter_chain;


//====================================================================================
//  Filtering functions prototypes
//====================================================================================

/**
 * @brief Given a list of records, check which ones have a coverage greater or equals to the one specified.
 * @details Given a list of records, check which ones have a coverage greater or equals than 
 * the one specified.
 * 
 * @param input_records List of records to filter
 * @param[out] failed Records that failed the filter's test
 * @param args Filter arguments
 * @return Records that passed the filter's test
 */
array_list_t *coverage_filter(array_list_t *input_records, array_list_t *failed, void *args);

/**
 * @brief Given a list of records, check which ones have a num_alleles equals to the one specified.
 * @details Given a list of records, check which ones have a num_alleles equals to 
 * the one specified.
 * 
 * @param input_records List of records to filter
 * @param[out] failed Records that failed the filter's test
 * @param args Filter arguments
 * @return Records that passed the filter's test
 */
array_list_t *num_alleles_filter(array_list_t *input_records, array_list_t *failed, void *args);

/**
 * @brief Given a list of records, check which ones have a quality greater or equals to the one specified.
 * 
 * @param input_records List of records to filter
 * @param[out] failed Records that failed the filter's test
 * @param args Filter arguments
 * @return Records that passed the filter's test
 */
array_list_t *quality_filter(array_list_t *input_records, array_list_t *failed, void *args);

/**
 * @brief Given a list of records, check which ones are positioned in certain genome region.
 * @details Given a list of records, check which ones are positioned in certain genome region.
 * A region is defined by a pair of fields: the chromosome and a position or range 
 * of positions.
 * 
 * @param input_records List of records to filter
 * @param[out] failed Records that failed the filter's test
 * @param args Filter arguments
 * @return Records that passed the filter's test
 */
array_list_t *region_filter(array_list_t *input_records, array_list_t *failed, void *args);

/**
 * @brief Given a list of records, check which ones represent a SNP.
 * @details Given a list of records, check which ones represent a SNP.
 * 
 * @param input_records List of records to filter
 * @param[out] failed Records that failed the filter's test
 * @param args Filter arguments
 * @return Records that passed the filter's test
 */
array_list_t *snp_filter(array_list_t *input_records, array_list_t *failed, void *args);


//====================================================================================
//  Filter management (creation, comparison...) functions prototypes
//====================================================================================

/**
 * @brief Creates a new filter by minimum coverage.
 * @details Creates a new filter by minimum coverage.
 *
 * @param min_coverage Minimum coverage for the records to pass the filter
 * @return The new filter
 **/
filter_t *coverage_filter_new(int min_coverage);

/**
 * @brief Deallocates memory of a filter by minimum coverage.
 * @details Deallocates memory of a filter by minimum coverage.
 *
 * @param filter The filter to deallocate
 **/
void coverage_filter_free(filter_t *filter);

/**
 * @brief Creates a new filter by number of alleles.
 * @details Creates a new filter by number of alleles.
 *
 * @param num_alleles Number of alleles of the records that pass the filter
 * @return The new filter
 **/
filter_t *num_alleles_filter_new(int num_alleles);

/**
 * @brief Deallocates memory of a filter by number of alleles.
 * @details Deallocates memory of a filter by number of alleles.
 *
 * @param filter The filter to deallocate
 **/
void num_alleles_filter_free(filter_t *filter);

/**
 * @brief Creates a new filter by minimum quality.
 * @details Creates a new filter by minimum quality.
 *
 * @param min_quality Minimum quality for the records to pass the filter
 * @return The new filter
 **/
filter_t *quality_filter_new(int min_quality);

/**
 * @brief Deallocates memory of a filter by minimum quality.
 * @details Deallocates memory of a filter by minimum quality.
 *
 * @param filter The filter to deallocate
 **/
void quality_filter_free(filter_t *filter);

/**
 * @brief Creates a new filter by region(s), considering them as ranges.
 * @details Creates a new filter by region(s), considering them as ranges.
 *
 * @param region_descriptor List of regions or filename where to extract the list from
 * @param use_region_file Whether to use a file containing regions
 * @param url URL of the web service to check for chromosomes order
 * @param species Species against the filter is applied
 * @param version Version of the web service to check for chromosome order
 * @return The new filter
 **/
filter_t *region_filter_new(char *region_descriptor, int use_region_file, const char *url, const char *species, const char *version);

/**
 * @brief Creates a new filter by region(s), considering them as exact positions.
 * @details Creates a new filter by region(s), considering them as exact positions.
 *
 * @param region_descriptor List of regions or filename where to extract the list from
 * @param use_region_file Whether to use a file containing regions
 * @param url URL of the web service to check for chromosomes order
 * @param species Species against the filter is applied
 * @param version Version of the web service to check for chromosome order
 * @return The new filter
 **/
filter_t *region_exact_filter_new(char *region_descriptor, int use_region_file, const char *url, const char *species, const char *version);

/**
 * @brief Deallocates memory of a filter by region(s).
 * @details Deallocates memory of a filter by region(s).
 *
 * @param filter The filter to deallocate
 **/
void region_filter_free(filter_t *filter);

/**
 * @brief Creates a new filter by SNP.
 * @details Creates a new filter by SNP.
 *
 * @param include_snps Whether to include or exclude a SNP.
 * @return The new filter
 **/
filter_t *snp_filter_new(int include_snps);

/**
 * @brief Deallocates memory of a filter by SNP.
 * @details Deallocates memory of a filter by SNP.
 *
 * @param filter The filter to deallocate
 **/
void snp_filter_free(filter_t *filter);


/**
 * @brief Compares the priority of two filters.
 * @details Compares the priority of two filters.
 *
 * @param filter1 First filter to compare
 * @param filter2 Second filter to compare
 * @return 0 if both filters have the same priority, less than zero if the 1st filter 
 * has less priority, and more than zero if it has more priority
 **/
int filter_compare(const void *filter1, const void *filter2);


//====================================================================================
//  Filter chain functions prototypes
//====================================================================================

/**
 * @brief Adds a filter to the given filter chain
 * @details Adds a filter to the given filter chain. If the chain is NULL, the filter is added 
 * after creating that chain.
 * 
 * @param filter Filter to add to the filter chain
 * @param chain Filter chain the filter is inserted in
 * @return The new state of the filter chain
 */
filter_chain *add_to_filter_chain(filter_t *filter, filter_chain *chain);

/**
 * @brief Sorts a chain of several filters by priority
 * @details Given a chain of several filters, creates a list sorted by priority.
 * 
 * @param chain Filter chain to sort
 * @param num_filters Number of filters in the chain
 * @return Sorted list of filters
 */
filter_t **sort_filter_chain(filter_chain *chain, int *num_filters);

/**
 * @brief Deallocates memory allocated to store a filter chain
 * @details Deallocates memory allocated to store a filter chain
 * 
 * @param chain Chain of filters to free
 */
void free_filter_chain(filter_chain *chain);

/**
 * @brief Applies a collection of filters to a list of records.
 * @details Applies a collection of filters to a list of records.
 * 
 * @param input_records List of records to filter
 * @param[out] failed Records that failed the filter's test
 * @param filters Filters to apply
 * @param num_filters Number of filters to apply
 * @return Records that passed the filters' tests
 */
array_list_t *run_filter_chain(array_list_t *input_records, array_list_t *failed, filter_t **filters, int num_filters);

#endif

