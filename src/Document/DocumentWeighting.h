//
// Created by Olcay Taner YILDIZ on 28.08.2023.
//

#ifndef INFORMATIONRETRIEVAL_DOCUMENTWEIGHTING_H
#define INFORMATIONRETRIEVAL_DOCUMENTWEIGHTING_H

enum document_weighting{
    NO_IDF = 0,
    IDF,
    PROBABILISTIC_IDF
};

typedef enum document_weighting Document_weighting;

#endif //INFORMATIONRETRIEVAL_DOCUMENTWEIGHTING_H
