cmake_minimum_required(VERSION 3.23)
project(InformationRetrieval)
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_FLAGS "-O3")

find_package(corpus_c REQUIRED)

add_library(InformationRetrieval Index/CategoryNode.c Index/CategoryNode.h Index/CategoryTree.c Index/CategoryTree.h Index/Posting.c Index/Posting.h Index/PostingSkip.c Index/PostingSkip.h Index/PostingList.c Index/PostingList.h Index/TermType.h Index/TermWeighting.h Document/IndexType.h Document/DocumentWeighting.h Document/DocumentType.h Query/CategoryDeterminationType.h Query/FocusType.h Query/RetrievalType.h Document/Parameter.c Document/Parameter.h Index/Term.c Index/Term.h Index/TermOccurrence.c Index/TermOccurrence.h Query/SearchParameter.c Query/SearchParameter.h Index/PositionalPosting.c Index/PositionalPosting.h Query/QueryResultItem.c Query/QueryResultItem.h Query/QueryResult.c Query/QueryResult.h Query/VectorSpaceModel.c Query/VectorSpaceModel.h Query/Query.c Query/Query.h Index/PositionalPostingList.c Index/PositionalPostingList.h Index/TermDictionary.c Index/TermDictionary.h Index/IncidenceMatrix.c Index/IncidenceMatrix.h Index/InvertedIndex.c Index/InvertedIndex.h Index/PositionalIndex.c Index/PositionalIndex.h Document/Document.c Document/Document.h Document/DocumentText.c Document/DocumentText.h Document/MemoryCollection.c Document/MemoryCollection.h)
target_link_libraries(InformationRetrieval corpus_c::corpus_c m)
