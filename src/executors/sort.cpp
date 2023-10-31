#include"global.h"
/**
 * @brief File contains method to process SORT commands.
 * 
 * syntax:
 * R <- SORT relation_name BY column_name IN sorting_order
 * 
 * sorting_order = ASC | DESC 
 */
bool syntacticParseSORT(){
    logger.log("syntacticParseSORT");
    auto byIt = find(tokenizedQuery.begin(), tokenizedQuery.end(), "BY");
    auto inIt = find(tokenizedQuery.begin(), tokenizedQuery.end(), "IN");
    if(tokenizedQuery.size() < 6 || byIt == tokenizedQuery.end() || inIt == tokenizedQuery.end()) {
        cout<<"SYNTAX ERROR"<<endl;
        return false;
    }

    parsedQuery.queryType = SORT;
    parsedQuery.sortRelationName = tokenizedQuery[1];
    for(int i = byIt - tokenizedQuery.begin() + 1; i < inIt - tokenizedQuery.begin(); i++) {
        parsedQuery.sortAttributes.push_back(tokenizedQuery[i]);
    }
    for(int i = inIt - tokenizedQuery.begin() + 1; i < tokenizedQuery.size(); i++) {
        parsedQuery.sortAttributesStrategy.push_back(tokenizedQuery[i]);
    }
    return true;
}

bool semanticParseSORT(){
    logger.log("semanticParseSORT");

    if(!tableCatalogue.isTable(parsedQuery.sortRelationName)){
        cout<<"SEMANTIC ERROR: Relation doesn't exist"<<endl;
        return false;
    }

    for(auto it: parsedQuery.sortAttributes) {
        if(!tableCatalogue.isColumnFromTable(it, parsedQuery.sortRelationName)) {
            cout<<"SEMANTIC ERROR: Column doesn't exist in relation"<<endl;
            return false;
        }
    }

    return true;
}

void executeSORT(){
    logger.log("executeSORT");
    Table *table = tableCatalogue.getTable(parsedQuery.sortRelationName);
    table->sortRelation(parsedQuery.sortAttributes, parsedQuery.sortAttributesStrategy, "");
    return;
}