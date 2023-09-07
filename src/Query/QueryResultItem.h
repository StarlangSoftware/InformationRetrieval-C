//
// Created by Olcay Taner YILDIZ on 29.08.2023.
//

#ifndef INFORMATIONRETRIEVAL_QUERYRESULTITEM_H
#define INFORMATIONRETRIEVAL_QUERYRESULTITEM_H

struct query_result_item{
    int doc_id;
    double score;
};

typedef struct query_result_item Query_result_item;

typedef Query_result_item *Query_result_item_ptr;

Query_result_item_ptr create_query_result_item(int doc_id, double score);

Query_result_item_ptr create_query_result_item2();

void free_query_result_item(Query_result_item_ptr query_result_item);

int compare_query_result_item(const Query_result_item* first, const Query_result_item* second);

#endif //INFORMATIONRETRIEVAL_QUERYRESULTITEM_H
