//
// Created by Olcay Taner YILDIZ on 30.08.2023.
//

#ifndef INFORMATIONRETRIEVAL_QUERY_H
#define INFORMATIONRETRIEVAL_QUERY_H

#include <ArrayList.h>
#include "HashMap/HashSet.h"

static char* shortcuts[] = {"cc", "cm2", "cm", "gb", "ghz", "gr", "gram", "hz", "inc", "inch",
                            "inç", "kg", "kw", "kva", "litre", "lt", "m2", "m3", "mah", "mb",
                            "metre", "mg", "mhz", "ml", "mm", "mp", "ms", "mt", "mv", "tb",
                            "tl", "va", "volt", "watt", "ah", "hp", "oz", "rpm", "dpi", "ppm",
                            "ohm", "kwh", "kcal", "kbit", "mbit", "gbit", "bit", "byte", "mbps", "gbps",
                            "cm3", "mm2", "mm3", "khz", "ft", "db", "sn", "g", "v", "m",
                            "l", "w", "s"};

struct query{
    Array_list_ptr terms;
};

typedef struct query Query;

typedef Query *Query_ptr;

Query_ptr create_query(char* query);

Query_ptr create_query2();

void free_query(Query_ptr query);

char* get_term(Query_ptr query, int index);

int size_of_query(Query_ptr query);

Query_ptr filter_attributes(Query_ptr query,
                            Hash_set_ptr attribute_list,
                            Query_ptr term_attributes,
                            Query_ptr phrase_attributes);

#endif //INFORMATIONRETRIEVAL_QUERY_H
