//
// Created by Olcay Taner YILDIZ on 29.08.2023.
//

#include <Memory/Memory.h>
#include "QueryResult.h"
#include "QueryResultItem.h"
#include "Heap/Heap.h"

/**
 * Empty constructor for the QueryResult object.
 */
Query_result_ptr create_query_result() {
    Query_result_ptr result = malloc_(sizeof(Query_result), "create_query_result");
    result->items = create_array_list();
    return result;
}

/**
 * Adds a new result item to the list of query result.
 * @param doc_id Document id of the result
 * @param score Score of the result
 */
void add_to_query_result(Query_result_ptr query_result, int doc_id, double score) {
    array_list_add(query_result->items, create_query_result_item(doc_id, score));
}

/**
 * Adds a new result item with score 0 to the list of query result.
 * @param doc_id Document id of the result
 */
void add_to_query_result2(Query_result_ptr query_result, int doc_id) {
    array_list_add(query_result->items, create_query_result_item(doc_id, 0.0));
}

/**
 * Returns number of results for query
 * @return Number of results for query
 */
int size_of_query_result(const Query_result *query_result) {
    return query_result->items->size;
}

/**
 * The method returns K best results from the query result using min heap in O(K log N + N log K) time.
 * @param K Size of the best subset.
 */
void get_best(Query_result_ptr query_result, int K) {
    Heap_ptr min_heap = create_heap(K, (int (*)(void *, void *)) compare_query_result_item);
    for (int i = 0; i < K && i < size_of_query_result(query_result); i++){
        heap_insert(min_heap, array_list_get(query_result->items, i));
    }
    for (int i = K + 1; i < size_of_query_result(query_result); i++){
        Query_result_item_ptr top = delete_top(min_heap);
        Query_result_item_ptr item = array_list_get(query_result->items, i);
        if (compare_query_result_item(top, item) > 0){
            heap_insert(min_heap, top);
        } else {
            heap_insert(min_heap, item);
        }
    }
    array_list_clear(query_result->items, NULL);
    for (int i = 0; i < K && !is_heap_empty(min_heap); i++){
        array_list_insert(query_result->items, 0, delete_top(min_heap));
    }
    free_heap(min_heap, (void (*)(void *)) free_query_result_item);
}

/**
 * Given two query results, this method identifies the intersection of those two results by doing parallel iteration
 * in O(N).
 * @param second Second query result to be intersected.
 * @return Intersection of this query result with the second query result
 */
Query_result_ptr intersection_fast_search(const Query_result *first, const Query_result *second) {
    Query_result_ptr result = create_query_result();
    int i = 0, j = 0;
    while (i < size_of_query_result(first) && j < size_of_query_result(second)){
        Query_result_item_ptr item1 = array_list_get(first->items, i);
        Query_result_item_ptr item2 = array_list_get(second->items, j);
        if (item1->doc_id == item2->doc_id){
            add_to_query_result2(result, item1->doc_id);
            i++;
            j++;
        } else {
            if (item1->doc_id < item2->doc_id){
                i++;
            } else {
                j++;
            }
        }
    }
    return result;
}

/**
 * Given two query results, this method identifies the intersection of those two results by doing binary search on
 * the second list in O(N log N).
 * @param second Second query result to be intersected.
 * @return Intersection of this query result with the second query result
 */
Query_result_ptr intersection_binary_search(const Query_result *first, const Query_result *second) {
    Query_result_ptr result = create_query_result();
    for (int i = 0; i < size_of_query_result(first); i++){
        Query_result_item_ptr searched_item = array_list_get(first->items, i);
        int low = 0;
        int high = size_of_query_result(first) - 1;
        int middle = (low + high) / 2;
        bool found = false;
        while (low <= high){
            if (searched_item->doc_id > ((Query_result_item_ptr) array_list_get(second->items, middle))->doc_id){
                low = middle + 1;
            } else {
                if (searched_item->doc_id < ((Query_result_item_ptr) array_list_get(second->items, middle))->doc_id){
                    high = middle - 1;
                } else {
                    found = true;
                    break;
                }
            }
            middle = (low + high) / 2;
        }
        if (found){
            add_to_query_result(result, searched_item->doc_id, searched_item->score);
        }
    }
    return result;
}

/**
 * Given two query results, this method identifies the intersection of those two results by doing exhaustive search
 * on the second list in O(N^2).
 * @param second Second query result to be intersected.
 * @return Intersection of this query result with the second query result
 */
Query_result_ptr intersection_linear_search(const Query_result *first, const Query_result *second) {
    Query_result_ptr result = create_query_result();
    for (int i = 0; i < size_of_query_result(first); i++) {
        Query_result_item_ptr searched_item = array_list_get(first->items, i);
        for (int j = 0; j < size_of_query_result(second); j++) {
            Query_result_item_ptr item = array_list_get(first->items, j);
            if (searched_item->doc_id == item->doc_id){
                add_to_query_result(result, searched_item->doc_id, searched_item->score);
            }
        }
    }
    return result;
}

/**
 * Frees memory allocated to the QueryResult object. Frees array list of items.
 * @param query_result Query result to be deallocated.
 */
void free_query_result(Query_result_ptr query_result) {
    free_array_list(query_result->items, (void (*)(void *)) free_query_result_item);
    free_(query_result);
}
