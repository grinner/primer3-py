#include    <string.h>
#include    <stdio.h>
#include    <Python.h>
#include    <libprimer3_mod.h>

// Check python dictionary `d` for key `k` and (if it exists) assign the
// value to Py_Object o. Return 1 on success or 0 if the key is not in the dict
#define DICT_GET_OBJ(o, d, k) ((o = PyDict_GetItemString(d, k)) != NULL)
// Wraps DICT_GET_OBJ and takes the dictionary value object, extracts a long,
// casts it to an int and assigns its value to `st`
#define DICT_GET_AND_ASSIGN_INT(o, d, k, st, err)                              \
    if (DICT_GET_OBJ(o, d, k)) {                                               \
        *st = (int)PyInt_AsLong(o);                                            \
        if (st == NULL) {                                                      \
            sprintf(err, "Value of %s is not an integer.", k);                 \
            return NULL;}                                                      \
    }
// Wraps DICT_GET_OBJ and takes the dictionary value object, extracts a long,
// casts it to `type` and assigns its value to `st`
#define DICT_GET_AND_ASSIGN_INT_TYPE(o, d, k, st, err, t)                      \
    if (DICT_GET_OBJ(o, d, k)) {                                               \
        *st = (t)PyInt_AsLong(o);                                              \
        if (st == NULL) {                                                      \
            sprintf(err, "Value of %s is not an integer.", k);                 \
            return NULL;}                                                      \
    }
// Wraps DICT_GET_OBJ and takes the dictionary value object, extracts a double,
// and assigns its value to `st`
#define DICT_GET_AND_ASSIGN_DOUBLE(o, d, k, st, err)                           \
    if (DICT_GET_OBJ(o, d, k)) {                                               \
        *st = PyFloat_AsDouble(o);                                             \
        if (st == NULL) {                                                      \
            sprintf(err, "Value of %s is not an integer or float.", k);        \
            return NULL;}                                                      \
    }
// PyString_AsString exposes the internal buffer of the string (null terminated)
// It must not be changed or freed, so we have to malloc new memory for the
// param value
#define DICT_GET_AND_COPY_STR(o, d, k, st, err)                                \
    if (DICT_GET_OBJ(o, d, k)) {                                               \
        if (PyString_AsString(o) == NULL){                                     \
            sprintf(err, "Value of %s is not a string.", k);                   \
            return NULL;}                                                      \
        *st = (char *) malloc(strlen(PyString_AsString(o)));                   \
        if (st == NULL) {                                                      \
            strcpy(err, "Primer3 out of memory");                              \
            return NULL;}                                                      \
        strcpy(*st, PyString_AsString(o));                                     \
    }


static p3_global_settings*
parseGlobalParams(PyObject *self, PyObject *p3s_dict, char *err) {
    /* Creates a new p3_global_settings struct and initializes it with 
     * defaults using p3_create_global_settings() from libprimer3.c.
     * Parses the user-provided settings from p3_settings_dict and 
     * overwrites the defaults (note that minimal error checking is 
     * performed in this function). If there is an error during the process
     * (e.g., a param is not of the correct time), `err` will be set to the
     * error string and the function will return NULL.
     */

    p3_global_settings      *pa;
    PyObject                *p_obj;

    pa = p3_create_global_settings();

    DICT_GET_AND_ASSIGN_INT(p_obj, p3s_dict, "PRIMER_OPT_SIZE", &pa->p_args.opt_size, err);
    DICT_GET_AND_ASSIGN_INT(p_obj, p3s_dict, "PRIMER_MIN_SIZE", &pa->p_args.min_size, err);
    DICT_GET_AND_ASSIGN_INT(p_obj, p3s_dict, "PRIMER_MAX_SIZE", &pa->p_args.max_size, err);
    DICT_GET_AND_ASSIGN_INT(p_obj, p3s_dict, "PRIMER_MAX_POLY_X", &pa->p_args.max_poly_x, err);
    DICT_GET_AND_ASSIGN_INT(p_obj, p3s_dict, "PRIMER_OPT_TM", &pa->p_args.opt_tm, err);
    DICT_GET_AND_ASSIGN_INT(p_obj, p3s_dict, "PRIMER_OPT_GC_PERCENT", &pa->p_args.opt_gc_content, err);
    DICT_GET_AND_ASSIGN_DOUBLE(p_obj, p3s_dict, "PRIMER_MIN_TM", &pa->p_args.min_tm, err);
    DICT_GET_AND_ASSIGN_DOUBLE(p_obj, p3s_dict, "PRIMER_MAX_TM", &pa->p_args.max_tm, err);
    DICT_GET_AND_ASSIGN_DOUBLE(p_obj, p3s_dict, "PRIMER_PAIR_MAX_DIFF_TM", &pa->max_diff_tm, err);
    DICT_GET_AND_ASSIGN_INT_TYPE(p_obj, p3s_dict, "PRIMER_TM_FORMULA", &pa->tm_santalucia, err, tm_method_type);
    DICT_GET_AND_ASSIGN_INT_TYPE(p_obj, p3s_dict, "PRIMER_SALT_CORRECTIONS", &pa->salt_corrections, err, salt_correction_type);
    DICT_GET_AND_ASSIGN_DOUBLE(p_obj, p3s_dict, "PRIMER_MIN_GC", &pa->p_args.min_gc, err);
    DICT_GET_AND_ASSIGN_DOUBLE(p_obj, p3s_dict, "PRIMER_MAX_GC", &pa->p_args.max_gc, err);
    DICT_GET_AND_ASSIGN_DOUBLE(p_obj, p3s_dict, "PRIMER_SALT_MONOVALENT", &pa->p_args.salt_conc, err);
    DICT_GET_AND_ASSIGN_DOUBLE(p_obj, p3s_dict, "PRIMER_SALT_DIVALENT", &pa->p_args.divalent_conc, err);
    DICT_GET_AND_ASSIGN_DOUBLE(p_obj, p3s_dict, "PRIMER_DNTP_CONC", &pa->p_args.dntp_conc, err);
    DICT_GET_AND_ASSIGN_DOUBLE(p_obj, p3s_dict, "PRIMER_DNA_CONC", &pa->p_args.dna_conc, err);
    DICT_GET_AND_ASSIGN_INT(p_obj, p3s_dict, "PRIMER_MAX_NS_ACCEPTED", &pa->p_args.num_ns_accepted, err);
    DICT_GET_AND_ASSIGN_INT(p_obj, p3s_dict, "PRIMER_PRODUCT_OPT_SIZE", &pa->product_opt_size, err);
    DICT_GET_AND_ASSIGN_DOUBLE(p_obj, p3s_dict, "PRIMER_MAX_SELF_ANY", &pa->p_args.max_self_any, err);
    DICT_GET_AND_ASSIGN_DOUBLE(p_obj, p3s_dict, "PRIMER_MAX_SELF_END", &pa->p_args.max_self_end, err);
    DICT_GET_AND_ASSIGN_DOUBLE(p_obj, p3s_dict, "PRIMER_MAX_SELF_ANY_TH", &pa->p_args.max_self_any_th, err);
    DICT_GET_AND_ASSIGN_DOUBLE(p_obj, p3s_dict, "PRIMER_MAX_SELF_END_TH", &pa->p_args.max_self_end_th, err);
    DICT_GET_AND_ASSIGN_DOUBLE(p_obj, p3s_dict, "PRIMER_MAX_HAIRPIN_TH", &pa->p_args.max_hairpin_th, err);
    DICT_GET_AND_ASSIGN_DOUBLE(p_obj, p3s_dict, "PRIMER_PAIR_MAX_COMPL_ANY", &pa->pair_compl_any, err);
    DICT_GET_AND_ASSIGN_DOUBLE(p_obj, p3s_dict, "PRIMER_PAIR_MAX_COMPL_END", &pa->pair_compl_end, err);
    DICT_GET_AND_ASSIGN_DOUBLE(p_obj, p3s_dict, "PRIMER_PAIR_MAX_COMPL_ANY_TH", &pa->pair_compl_any_th, err);
    DICT_GET_AND_ASSIGN_DOUBLE(p_obj, p3s_dict, "PRIMER_PAIR_MAX_COMPL_END_TH", &pa->p_args.divalent_conc, err);
    // DICT_GET_AND_ASSIGN_DOUBLE(p_obj, p3s_dict, "P3_FILE_FLAG", &pa->p_args.divalent_conc, err);
    DICT_GET_AND_ASSIGN_INT(p_obj, p3s_dict, "PRIMER_PICK_ANYWAY", &pa->pick_anyway, err);
    DICT_GET_AND_ASSIGN_INT(p_obj, p3s_dict, "PRIMER_GC_CLAMP", &pa->gc_clamp, err);
    DICT_GET_AND_ASSIGN_INT(p_obj, p3s_dict, "PRIMER_MAX_END_GC", &pa->max_end_gc, err);
    // DICT_GET_AND_ASSIGN_INT(p_obj, p3s_dict, "PRIMER_EXPLAIN_FLAG", &pa->explain_flag, err);
    DICT_GET_AND_ASSIGN_INT(p_obj, p3s_dict, "PRIMER_LIBERAL_BASE", &pa->liberal_base, err);
    DICT_GET_AND_ASSIGN_INT(p_obj, p3s_dict, "PRIMER_FIRST_BASE_INDEX", &pa->first_base_index, err);
    DICT_GET_AND_ASSIGN_INT(p_obj, p3s_dict, "PRIMER_NUM_RETURN", &pa->num_return, err);
    DICT_GET_AND_ASSIGN_INT(p_obj, p3s_dict, "PRIMER_MIN_QUALITY", &pa->p_args.min_quality, err);
    DICT_GET_AND_ASSIGN_INT(p_obj, p3s_dict, "PRIMER_MIN_END_QUALITY", &pa->p_args.min_end_quality, err);
    DICT_GET_AND_ASSIGN_INT(p_obj, p3s_dict, "PRIMER_MIN_LEFT_THREE_PRIME_DISTANCE", &pa->min_left_three_prime_distance, err);
    DICT_GET_AND_ASSIGN_INT(p_obj, p3s_dict, "PRIMER_MIN_RIGHT_THREE_PRIME_DISTANCE", &pa->min_right_three_prime_distance, err);
    DICT_GET_AND_ASSIGN_INT(p_obj, p3s_dict, "PRIMER_MIN_THREE_PRIME_DISTANCE", &pa->min_left_three_prime_distance, err);
    DICT_GET_AND_ASSIGN_INT(p_obj, p3s_dict, "PRIMER_MIN_THREE_PRIME_DISTANCE", &pa->min_right_three_prime_distance, err);
    DICT_GET_AND_ASSIGN_INT(p_obj, p3s_dict, "PRIMER_QUALITY_RANGE_MIN", &pa->quality_range_min, err);
    DICT_GET_AND_ASSIGN_INT(p_obj, p3s_dict, "PRIMER_QUALITY_RANGE_MAX", &pa->quality_range_max, err);
    DICT_GET_AND_ASSIGN_DOUBLE(p_obj, p3s_dict, "PRIMER_PRODUCT_MAX_TM", &pa->product_max_tm, err);
    DICT_GET_AND_ASSIGN_DOUBLE(p_obj, p3s_dict, "PRIMER_PRODUCT_MIN_TM", &pa->product_min_tm, err);
    DICT_GET_AND_ASSIGN_DOUBLE(p_obj, p3s_dict, "PRIMER_PRODUCT_OPT_TM", &pa->product_opt_tm, err);
    DICT_GET_AND_ASSIGN_DOUBLE(p_obj, p3s_dict, "PRIMER_SEQUENCING_LEAD", &pa->sequencing.lead, err);
    DICT_GET_AND_ASSIGN_DOUBLE(p_obj, p3s_dict, "PRIMER_SEQUENCING_SPACING", &pa->sequencing.spacing, err);
    DICT_GET_AND_ASSIGN_DOUBLE(p_obj, p3s_dict, "PRIMER_SEQUENCING_INTERVAL", &pa->sequencing.interval, err);
    DICT_GET_AND_ASSIGN_DOUBLE(p_obj, p3s_dict, "PRIMER_SEQUENCING_ACCURACY", &pa->sequencing.accuracy, err);
    DICT_GET_AND_ASSIGN_INT(p_obj, p3s_dict, "PRIMER_MIN_5_PRIME_OVERLAP_OF_JUNCTION", &pa->min_5_prime_overlap_of_junction, err);
    DICT_GET_AND_ASSIGN_INT(p_obj, p3s_dict, "PRIMER_MIN_3_PRIME_OVERLAP_OF_JUNCTION", &pa->min_3_prime_overlap_of_junction, err);
    DICT_GET_AND_ASSIGN_INT(p_obj, p3s_dict, "PRIMER_PICK_RIGHT_PRIMER", &pa->pick_right_primer, err);
    DICT_GET_AND_ASSIGN_INT(p_obj, p3s_dict, "PRIMER_PICK_INTERNAL_OLIGO", &pa->pick_internal_oligo, err);
    DICT_GET_AND_ASSIGN_INT(p_obj, p3s_dict, "PRIMER_PICK_LEFT_PRIMER", &pa->pick_left_primer, err);
    DICT_GET_AND_ASSIGN_INT(p_obj, p3s_dict, "PRIMER_INTERNAL_OPT_SIZE", &pa->o_args.opt_size, err);
    DICT_GET_AND_ASSIGN_INT(p_obj, p3s_dict, "PRIMER_INTERNAL_MAX_SIZE", &pa->o_args.max_size, err);
    DICT_GET_AND_ASSIGN_INT(p_obj, p3s_dict, "PRIMER_INTERNAL_MIN_SIZE", &pa->o_args.min_size, err);
    DICT_GET_AND_ASSIGN_INT(p_obj, p3s_dict, "PRIMER_INTERNAL_MAX_POLY_X", &pa->o_args.max_poly_x, err);
    DICT_GET_AND_ASSIGN_DOUBLE(p_obj, p3s_dict, "PRIMER_INTERNAL_OPT_TM", &pa->o_args.opt_tm, err);
    DICT_GET_AND_ASSIGN_DOUBLE(p_obj, p3s_dict, "PRIMER_INTERNAL_OPT_GC_PERCENT", &pa->o_args.opt_gc_content, err);
    DICT_GET_AND_ASSIGN_DOUBLE(p_obj, p3s_dict, "PRIMER_INTERNAL_MAX_TM", &pa->o_args.max_tm, err);
    DICT_GET_AND_ASSIGN_DOUBLE(p_obj, p3s_dict, "PRIMER_INTERNAL_MIN_TM", &pa->o_args.min_tm, err);
    DICT_GET_AND_ASSIGN_DOUBLE(p_obj, p3s_dict, "PRIMER_INTERNAL_MIN_GC", &pa->o_args.min_gc, err);
    DICT_GET_AND_ASSIGN_DOUBLE(p_obj, p3s_dict, "PRIMER_INTERNAL_MAX_GC", &pa->o_args.max_gc, err);
    DICT_GET_AND_ASSIGN_DOUBLE(p_obj, p3s_dict, "PRIMER_INTERNAL_SALT_MONOVALENT", &pa->o_args.salt_conc, err);
    DICT_GET_AND_ASSIGN_DOUBLE(p_obj, p3s_dict, "PRIMER_INTERNAL_SALT_DIVALENT", &pa->o_args.divalent_conc, err);
    DICT_GET_AND_ASSIGN_DOUBLE(p_obj, p3s_dict, "PRIMER_INTERNAL_DNTP_CONC", &pa->o_args.dntp_conc, err);
    DICT_GET_AND_ASSIGN_DOUBLE(p_obj, p3s_dict, "PRIMER_INTERNAL_DNA_CONC", &pa->o_args.dna_conc, err);
    DICT_GET_AND_ASSIGN_INT(p_obj, p3s_dict, "PRIMER_INTERNAL_MAX_NS_ACCEPTED", &pa->o_args.num_ns_accepted, err);
    DICT_GET_AND_ASSIGN_INT(p_obj, p3s_dict, "PRIMER_INTERNAL_MIN_QUALITY", &pa->o_args.min_quality, err);
    DICT_GET_AND_ASSIGN_DOUBLE(p_obj, p3s_dict, "PRIMER_INTERNAL_MAX_SELF_ANY", &pa->o_args.max_self_any, err);
    DICT_GET_AND_ASSIGN_DOUBLE(p_obj, p3s_dict, "PRIMER_INTERNAL_MAX_SELF_END", &pa->p_args.max_self_end, err);
    DICT_GET_AND_ASSIGN_DOUBLE(p_obj, p3s_dict, "PRIMER_INTERNAL_MAX_SELF_ANY_TH", &pa->o_args.max_self_any_th, err);
    DICT_GET_AND_ASSIGN_DOUBLE(p_obj, p3s_dict, "PRIMER_INTERNAL_MAX_SELF_END_TH", &pa->o_args.max_self_end_th, err);
    DICT_GET_AND_ASSIGN_DOUBLE(p_obj, p3s_dict, "PRIMER_INTERNAL_MAX_HAIRPIN_TH", &pa->o_args.max_hairpin_th, err);
    DICT_GET_AND_ASSIGN_DOUBLE(p_obj, p3s_dict, "PRIMER_MAX_LIBRARY_MISPRIMING", &pa->p_args.max_repeat_compl, err);
    DICT_GET_AND_ASSIGN_DOUBLE(p_obj, p3s_dict, "PRIMER_INTERNAL_MAX_LIBRARY_MISHYB", &pa->o_args.max_repeat_compl, err);
    DICT_GET_AND_ASSIGN_DOUBLE(p_obj, p3s_dict, "PRIMER_PAIR_MAX_LIBRARY_MISPRIMING", &pa->pair_repeat_compl, err);
    DICT_GET_AND_ASSIGN_DOUBLE(p_obj, p3s_dict, "PRIMER_MAX_TEMPLATE_MISPRIMING", &pa->p_args.max_template_mispriming, err);
    DICT_GET_AND_ASSIGN_DOUBLE(p_obj, p3s_dict, "PRIMER_MAX_TEMPLATE_MISPRIMING_TH", &pa->p_args.max_template_mispriming_th, err);
    DICT_GET_AND_ASSIGN_DOUBLE(p_obj, p3s_dict, "PRIMER_PAIR_MAX_TEMPLATE_MISPRIMING", &pa->pair_max_template_mispriming, err);
    DICT_GET_AND_ASSIGN_DOUBLE(p_obj, p3s_dict, "PRIMER_PAIR_MAX_TEMPLATE_MISPRIMING_TH", &pa->pair_max_template_mispriming_th, err);
    DICT_GET_AND_ASSIGN_INT(p_obj, p3s_dict, "PRIMER_LIB_AMBIGUITY_CODES_CONSENSUS", &pa->lib_ambiguity_codes_consensus, err);
    DICT_GET_AND_ASSIGN_DOUBLE(p_obj, p3s_dict, "PRIMER_INSIDE_PENALTY", &pa->inside_penalty, err);
    DICT_GET_AND_ASSIGN_DOUBLE(p_obj, p3s_dict, "PRIMER_OUTSIDE_PENALTY", &pa->outside_penalty, err);
    // DICT_GET_AND_ASSIGN_DOUBLE(p_obj, p3s_dict, "PRIMER_MISPRIMING_LIBRARY",    &pa->p_args, err);
    // DICT_GET_AND_ASSIGN_DOUBLE(p_obj, p3s_dict, "PRIMER_INTERNAL_MISHYB_LIBRARY",    &pa->p_args, err);
    DICT_GET_AND_ASSIGN_DOUBLE(p_obj, p3s_dict, "PRIMER_MAX_END_STABILITY", &pa->max_end_stability, err);
    DICT_GET_AND_ASSIGN_INT(p_obj, p3s_dict, "PRIMER_LOWERCASE_MASKING", &pa->lowercase_masking, err);
    DICT_GET_AND_ASSIGN_INT(p_obj, p3s_dict, "PRIMER_THERMODYNAMIC_OLIGO_ALIGNMENT", &pa->thermodynamic_oligo_alignment, err);
    DICT_GET_AND_ASSIGN_INT(p_obj, p3s_dict, "PRIMER_THERMODYNAMIC_TEMPLATE_ALIGNMENT", &pa->thermodynamic_template_alignment, err);
    // GPO(p_obj, p3s_dict, "PRIMER_THERMODYNAMIC_PARAMETERS_PATH",    &pa->p_args., err);
    DICT_GET_AND_COPY_STR(p_obj, p3s_dict, "PRIMER_MUST_MATCH_FIVE_PRIME", &pa->p_args.must_match_five_prime, err);
    DICT_GET_AND_COPY_STR(p_obj, p3s_dict, "PRIMER_MUST_MATCH_THREE_PRIME", &pa->p_args.must_match_three_prime, err);
    DICT_GET_AND_COPY_STR(p_obj, p3s_dict, "PRIMER_INTERNAL_MUST_MATCH_FIVE_PRIME", &pa->o_args.must_match_five_prime, err);
    DICT_GET_AND_COPY_STR(p_obj, p3s_dict, "PRIMER_INTERNAL_MUST_MATCH_THREE_PRIME", &pa->o_args.must_match_three_prime, err);
    DICT_GET_AND_ASSIGN_DOUBLE(p_obj, p3s_dict, "PRIMER_WT_TM_GT", &pa->p_args.weights.temp_gt, err);
    DICT_GET_AND_ASSIGN_DOUBLE(p_obj, p3s_dict, "PRIMER_WT_TM_LT", &pa->p_args.weights.temp_lt, err);
    DICT_GET_AND_ASSIGN_DOUBLE(p_obj, p3s_dict, "PRIMER_WT_GC_PERCENT_GT", &pa->p_args.weights.gc_content_gt, err);
    DICT_GET_AND_ASSIGN_DOUBLE(p_obj, p3s_dict, "PRIMER_WT_GC_PERCENT_LT", &pa->p_args.weights.gc_content_gt, err);
    DICT_GET_AND_ASSIGN_DOUBLE(p_obj, p3s_dict, "PRIMER_WT_SIZE_LT", &pa->p_args.weights.gc_content_lt, err);
    DICT_GET_AND_ASSIGN_DOUBLE(p_obj, p3s_dict, "PRIMER_WT_SIZE_GT", &pa->p_args.weights.length_lt, err);
    DICT_GET_AND_ASSIGN_DOUBLE(p_obj, p3s_dict, "PRIMER_WT_SELF_ANY", &pa->p_args.weights.length_gt, err);
    DICT_GET_AND_ASSIGN_DOUBLE(p_obj, p3s_dict, "PRIMER_WT_SELF_END", &pa->p_args.weights.compl_any, err);
    DICT_GET_AND_ASSIGN_DOUBLE(p_obj, p3s_dict, "PRIMER_WT_SELF_ANY_TH", &pa->p_args.weights.compl_end, err);
    DICT_GET_AND_ASSIGN_DOUBLE(p_obj, p3s_dict, "PRIMER_WT_SELF_END_TH", &pa->p_args.weights.compl_any_th, err);
    DICT_GET_AND_ASSIGN_DOUBLE(p_obj, p3s_dict, "PRIMER_WT_HAIRPIN_TH", &pa->p_args.weights.hairpin_th, err);
    DICT_GET_AND_ASSIGN_DOUBLE(p_obj, p3s_dict, "PRIMER_WT_NUM_NS", &pa->p_args.weights.num_ns, err);
    DICT_GET_AND_ASSIGN_DOUBLE(p_obj, p3s_dict, "PRIMER_WT_LIBRARY_MISPRIMING", &pa->p_args.weights.repeat_sim, err);
    DICT_GET_AND_ASSIGN_DOUBLE(p_obj, p3s_dict, "PRIMER_WT_SEQ_QUAL", &pa->p_args.weights.seq_quality, err);
    DICT_GET_AND_ASSIGN_DOUBLE(p_obj, p3s_dict, "PRIMER_WT_END_QUAL", &pa->p_args.weights.end_quality, err);
    DICT_GET_AND_ASSIGN_DOUBLE(p_obj, p3s_dict, "PRIMER_WT_POS_PENALTY", &pa->p_args.weights.pos_penalty, err);
    DICT_GET_AND_ASSIGN_DOUBLE(p_obj, p3s_dict, "PRIMER_WT_END_STABILITY", &pa->p_args.weights.end_stability, err);
    DICT_GET_AND_ASSIGN_DOUBLE(p_obj, p3s_dict, "PRIMER_WT_TEMPLATE_MISPRIMING", &pa->p_args.weights.template_mispriming, err);
    DICT_GET_AND_ASSIGN_DOUBLE(p_obj, p3s_dict, "PRIMER_WT_TEMPLATE_MISPRIMING_TH", &pa->p_args.weights.template_mispriming_th, err);
    DICT_GET_AND_ASSIGN_DOUBLE(p_obj, p3s_dict, "PRIMER_INTERNAL_WT_TM_GT", &pa->o_args.weights.temp_gt, err);
    DICT_GET_AND_ASSIGN_DOUBLE(p_obj, p3s_dict, "PRIMER_INTERNAL_WT_TM_LT", &pa->o_args.weights.temp_lt, err);
    DICT_GET_AND_ASSIGN_DOUBLE(p_obj, p3s_dict, "PRIMER_INTERNAL_WT_GC_PERCENT_GT", &pa->o_args.weights.gc_content_gt, err);
    DICT_GET_AND_ASSIGN_DOUBLE(p_obj, p3s_dict, "PRIMER_INTERNAL_WT_GC_PERCENT_LT", &pa->o_args.weights.gc_content_lt, err);
    DICT_GET_AND_ASSIGN_DOUBLE(p_obj, p3s_dict, "PRIMER_INTERNAL_WT_SIZE_LT", &pa->o_args.weights.length_lt, err);
    DICT_GET_AND_ASSIGN_DOUBLE(p_obj, p3s_dict, "PRIMER_INTERNAL_WT_SIZE_GT", &pa->o_args.weights.length_gt, err);
    DICT_GET_AND_ASSIGN_DOUBLE(p_obj, p3s_dict, "PRIMER_INTERNAL_WT_SELF_ANY", &pa->o_args.weights.compl_any, err);
    DICT_GET_AND_ASSIGN_DOUBLE(p_obj, p3s_dict, "PRIMER_INTERNAL_WT_SELF_END", &pa->o_args.weights.compl_end, err);
    DICT_GET_AND_ASSIGN_DOUBLE(p_obj, p3s_dict, "PRIMER_INTERNAL_WT_SELF_ANY_TH", &pa->o_args.weights.compl_any_th, err);
    DICT_GET_AND_ASSIGN_DOUBLE(p_obj, p3s_dict, "PRIMER_INTERNAL_WT_SELF_END_TH", &pa->o_args.weights.compl_end_th, err);
    DICT_GET_AND_ASSIGN_DOUBLE(p_obj, p3s_dict, "PRIMER_INTERNAL_WT_HAIRPIN_TH", &pa->o_args.weights.hairpin_th, err);
    DICT_GET_AND_ASSIGN_DOUBLE(p_obj, p3s_dict, "PRIMER_INTERNAL_WT_NUM_NS", &pa->o_args.weights.num_ns, err);
    DICT_GET_AND_ASSIGN_DOUBLE(p_obj, p3s_dict, "PRIMER_INTERNAL_WT_LIBRARY_MISHYB", &pa->o_args.weights.repeat_sim, err);
    DICT_GET_AND_ASSIGN_DOUBLE(p_obj, p3s_dict, "PRIMER_INTERNAL_WT_SEQ_QUAL", &pa->o_args.weights.seq_quality, err);
    DICT_GET_AND_ASSIGN_DOUBLE(p_obj, p3s_dict, "PRIMER_INTERNAL_WT_END_QUAL", &pa->o_args.weights.end_quality, err);
    DICT_GET_AND_ASSIGN_DOUBLE(p_obj, p3s_dict, "PRIMER_WT_TEMPLATE_MISPRIMING_TH", &pa->o_args.weights.template_mispriming_th, err);
    DICT_GET_AND_ASSIGN_DOUBLE(p_obj, p3s_dict, "PRIMER_PAIR_WT_PR_PENALTY", &pa->pr_pair_weights.primer_quality, err);
    DICT_GET_AND_ASSIGN_DOUBLE(p_obj, p3s_dict, "PRIMER_PAIR_WT_IO_PENALTY", &pa->pr_pair_weights.io_quality, err);
    DICT_GET_AND_ASSIGN_DOUBLE(p_obj, p3s_dict, "PRIMER_PAIR_WT_DIFF_TM", &pa->pr_pair_weights.diff_tm, err);
    DICT_GET_AND_ASSIGN_DOUBLE(p_obj, p3s_dict, "PRIMER_PAIR_WT_COMPL_ANY", &pa->pr_pair_weights.compl_any, err);
    DICT_GET_AND_ASSIGN_DOUBLE(p_obj, p3s_dict, "PRIMER_PAIR_WT_COMPL_END", &pa->pr_pair_weights.compl_end, err);
    DICT_GET_AND_ASSIGN_DOUBLE(p_obj, p3s_dict, "PRIMER_PAIR_WT_COMPL_ANY_TH", &pa->pr_pair_weights.compl_any_th, err);
    DICT_GET_AND_ASSIGN_DOUBLE(p_obj, p3s_dict, "PRIMER_PAIR_WT_COMPL_END_TH", &pa->pr_pair_weights.compl_end_th, err);
    DICT_GET_AND_ASSIGN_DOUBLE(p_obj, p3s_dict, "PRIMER_PAIR_WT_PRODUCT_TM_LT", &pa->pr_pair_weights.product_tm_lt, err);
    DICT_GET_AND_ASSIGN_DOUBLE(p_obj, p3s_dict, "PRIMER_PAIR_WT_PRODUCT_TM_GT", &pa->pr_pair_weights.product_tm_gt, err);
    DICT_GET_AND_ASSIGN_DOUBLE(p_obj, p3s_dict, "PRIMER_PAIR_WT_PRODUCT_SIZE_GT", &pa->pr_pair_weights.product_size_gt, err);
    DICT_GET_AND_ASSIGN_DOUBLE(p_obj, p3s_dict, "PRIMER_PAIR_WT_PRODUCT_SIZE_LT", &pa->pr_pair_weights.product_size_lt, err);
    DICT_GET_AND_ASSIGN_DOUBLE(p_obj, p3s_dict, "PRIMER_PAIR_WT_LIBRARY_MISPRIMING", &pa->pr_pair_weights.repeat_sim, err);
    DICT_GET_AND_ASSIGN_DOUBLE(p_obj, p3s_dict, "PRIMER_PAIR_WT_TEMPLATE_MISPRIMING", &pa->pr_pair_weights.template_mispriming, err);
    DICT_GET_AND_ASSIGN_DOUBLE(p_obj, p3s_dict, "PRIMER_PAIR_WT_TEMPLATE_MISPRIMING_TH", &pa->pr_pair_weights.template_mispriming_th, err);

    return pa;
}
